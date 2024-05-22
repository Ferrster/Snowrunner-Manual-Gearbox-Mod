#include "smgm/utils/dinput_tools.h"

#include <dinput.h>

#include <array>
#include <shared_mutex>
#include <unordered_map>
#include <utility>

#include "detours.h"
#include "smgm/smgm.h"
#include "smgm/utils/format_helpers.h"
#include "smgm/utils/guid.h"
#include "smgm/utils/logging.h"
#include "smgm/utils/utils.h"

namespace smgm::dinput {
IDirectInput8 *dinput_inst = nullptr;
boost::signals2::signal<void(std::shared_ptr<Device>)> sig_device_created;
boost::signals2::signal<void(std::shared_ptr<Device>)> sig_device_released;
boost::signals2::signal<void(std::shared_ptr<Device>, const DeviceState &)>
    sig_device_state;
DeviceMap created_devices;

std::shared_mutex mtx_devs;

template <typename T>
HRESULT ProcessDeviceState(const Device &dev_info, T *out_state,
                           std::size_t size) {
  if (!out_state) return -1;

  HRESULT res;

  if (FAILED(res = dev_info.GetInstance()->GetDeviceState(
                 static_cast<DWORD>(size), out_state))) {
    LOG_WARNING(fmt::format("[{}] GetDeviceState failed ({})",
                            dev_info.GetName(), res));
  }

  return res;
}

bool Init(HINSTANCE hinst) {
  IDirectInput8 *dinput = nullptr;

  if (DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8,
                         (LPVOID *)&dinput, NULL) != DI_OK) {
    LOG_ERROR("Failed to create DirectInput8 instance!");

    return false;
  }

  LPDIRECTINPUTDEVICE8 lpDInputDevice;

  if (dinput->CreateDevice(GUID_SysKeyboard, &lpDInputDevice, NULL) != DI_OK) {
    dinput->Release();

    LOG_ERROR("Failed to create dinput device!");

    return false;
  }

  // {
  //   void *vftable = *(void **)lpDInputDevice;

  //   g_oDInputGetDeviceState =
  //       (FncDInputGetDeviceState) *
  //       (uintptr_t *)((BYTE *)vftable + 9 * sizeof(void *));
  //   g_oDInputGetDeviceData =
  //       (FncDInputGetDeviceData) *
  //       (uintptr_t *)((BYTE *)vftable + 10 * sizeof(void *));
  // }

  // DetourAttach(&(PVOID &)g_oDInputGetDeviceState,
  //              (PVOID)Hook_DInputGetDeviceState);
  // DetourAttach(&(PVOID &)g_oDInputGetDeviceData,
  //              (PVOID)Hook_DInputGetDeviceData);

  lpDInputDevice->Release();
  dinput_inst = dinput;

  LOG_INFO("DirectInput8 initialized");

  return true;
}

void Release(HINSTANCE hinst) {
  // DetourDetach(&(PVOID &)g_oDInputGetDeviceState,
  //              (PVOID)Hook_DInputGetDeviceState);
  // DetourDetach(&(PVOID &)g_oDInputGetDeviceData,
  //              (PVOID)Hook_DInputGetDeviceData);

  GetInstance()->Release();

  LOG_INFO("DirectInput8 released");
}

IDirectInput8 *GetInstance() { return dinput_inst; }

const auto FromObjType = [](DWORD dwType) -> std::string {
  // switch (DIDFT_GETTYPE(dwType)) {
  //   case DIDFT_AXIS:
  //     return "DIDFT_AXIS";
  //   case DIDFT_BUTTON:
  //     return "DIDFT_BUTTON";
  //   case DIDFT_POV:
  //     return "DIDFT_POV";
  //   default:
  //     return "???";
  // }
  const DWORD type = DIDFT_GETTYPE(dwType);

  if (type & DIDFT_BUTTON) {
    return fmt::format("Button ({})",
                       type & DIDFT_PSHBUTTON ? "push" : "toggle");
  } else if (type & DIDFT_AXIS) {
    return fmt::format("Axis ({})", type & DIDFT_ABSAXIS ? "abs" : "rel");
  } else if (type == DIDFT_POV) {
    return "POV";
  } else {
    return "???";
  }
};

// FIXME: DEBUG
BOOL DEBUG_EnumObjects(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef) {
  LOG_DEBUG(FormatDataTable(
      fmt::format("Device object {}", std::string(lpddoi->tszName)),
      std::make_pair("type (dwType)", FromObjType(lpddoi->dwType)),
      std::make_pair("dwOfs", lpddoi->dwOfs),
      std::make_pair("dwType", lpddoi->dwType),
      std::make_pair("dwFlags", lpddoi->dwFlags),
      std::make_pair("Instance number", DIDFT_GETINSTANCE(lpddoi->dwType))));

  return DIENUM_CONTINUE;
}

std::shared_ptr<Device> CreateDevice(const std::string &str_guid) {
  LOG_INFO(fmt::format("Creating DirectInput device with GUID {}", str_guid));

  auto opt_guid = ToGUID(str_guid);

  if (!opt_guid.has_value()) {
    LOG_ERROR(
        fmt::format("Cannot create dinput device {}: invalid GUID format"));

    return nullptr;
  }

  GUID guid = opt_guid.value();
  LPDIRECTINPUTDEVICE8 device;

  if (auto res = GetInstance()->CreateDevice(guid, &device, NULL);
      res != DI_OK) {
    LOG_ERROR(
        fmt::format("Cannot create dinput device {}: CreateDevice failed ({})!",
                    str_guid, res));

    return nullptr;
  }

  DIDEVICEINSTANCE didi;

  didi.dwSize = sizeof(didi);

  if (HRESULT res; FAILED(res = device->GetDeviceInfo(&didi))) {
    LOG_ERROR(fmt::format(
        "Cannot create dinput device {}: GetDeviceInfo failed ({})!", str_guid,
        res));

    device->Release();

    return nullptr;
  }

  const auto *df = [&]() -> LPCDIDATAFORMAT {
    switch (didi.dwDevType & 0xFF) {
      case DI8DEVTYPE_KEYBOARD:
        return &c_dfDIKeyboard;
      case DI8DEVTYPE_DRIVING:
      case DI8DEVTYPE_GAMEPAD:
      case DI8DEVTYPE_JOYSTICK:
      case DI8DEVTYPE_SUPPLEMENTAL: {
        // for (std::size_t i = 0; i < c_dfDIJoystick2.dwNumObjs; ++i) {
        //   const DIOBJECTDATAFORMAT &diodf = c_dfDIJoystick2.rgodf[i];

        //   LOG_DEBUG(fmt::format(
        //       "{} | dwOfs={}, dwType={}, FIELD_OFFSET={}",
        //       diodf.pguid ? FromGuidType(*diodf.pguid) : "[no guid]",
        //       diodf.dwOfs, diodf.dwType,
        //       FIELD_OFFSET(DIJOYSTATE2, rglVSlider[0])));
        // }

        return &c_dfDIJoystick2;
      }
      default:
        return nullptr;
    }
  }();

  if (!df) {
    LOG_ERROR(fmt::format(
        "Cannot create dinput device {}: Unsupported device type {}", str_guid,
        didi.dwDevType));

    device->Release();

    return nullptr;
  }

  if (auto res = device->SetDataFormat(df); res != DI_OK) {
    LOG_ERROR(fmt::format(
        "Cannot create dinput device {}: SetDataFormat failed ({})!", str_guid,
        res));

    device->Release();

    return nullptr;
  }

  if (auto res = device->SetCooperativeLevel(
          smgm::game_window, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
      res != DI_OK) {
    LOG_ERROR(fmt::format(
        "Cannot create dinput device {}: SetCooperativeLevel failed ({})!",
        str_guid, res));

    device->Release();

    return nullptr;
  }

  auto new_dev = std::make_shared<Device>(device, didi, df);

  new_dev->sig_device_released.connect([wp_dev = std::weak_ptr(new_dev)] {
    if (auto dev = wp_dev.lock()) {
      sig_device_released(dev);
    }
  });
  created_devices.insert({str_guid, new_dev});
  sig_device_created(new_dev);

  LOG_INFO(fmt::format("Device {} created = {}", str_guid, (void *)device));

  // FIXME: DEBUG
  {
    // {
    //   DIDEVICEOBJECTINSTANCE didoi;
    //   didoi.dwSize = sizeof(didoi);

    //   auto *lpddoi = &didoi;

    //   if (auto hr = new_dev->GetInstance()->GetObjectInfo(
    //           &didoi, FIELD_OFFSET(DIJOYSTATE2, lZ), DIPH_BYOFFSET);
    //       hr == DI_OK) {
    //     LOG_DEBUG(FormatDataTable(
    //         fmt::format("SUCK object {}", std::string(lpddoi->tszName)),
    //         std::make_pair("type (guidType)",
    //         FromGuidType(lpddoi->guidType)), std::make_pair("dwOfs",
    //         lpddoi->dwOfs), std::make_pair("dwType", lpddoi->dwType),
    //         std::make_pair("dwFlags", lpddoi->dwFlags),
    //         std::make_pair("Instance number",
    //                        DIDFT_GETINSTANCE(lpddoi->dwType))));
    //   } else {
    //     LOG_DEBUG(fmt::format("COCK FAILED: {}", hr));
    //   }
    // }
    // {
    //   DIDEVICEOBJECTINSTANCE doi;
    //   doi.dwSize = sizeof(doi);

    //   for (std::size_t i = 0; i < c_dfDIJoystick2.dwNumObjs; ++i) {
    //     const DIOBJECTDATAFORMAT &diodf = c_dfDIJoystick2.rgodf[i];

    //     if (auto hr = new_dev->GetInstance()->GetObjectInfo(&doi,
    //     diodf.dwOfs,
    //                                                         DIPH_BYOFFSET);
    //         hr == DI_OK) {
    //       LOG_DEBUG(fmt::format(
    //           "name={}, type_name={}, diodf.dwType={}, doi.dwType={}, "
    //           "diodf.dwOfs={}, doi.dwOfs={}",
    //           std::string{doi.tszName}, FromObjType(doi.dwType),
    //           diodf.dwType, doi.dwType, diodf.dwOfs, doi.dwOfs));
    //     }
    //     // } else {
    //     //   LOG_DEBUG(fmt::format(
    //     //       "FAILED={}, type_name={}, diodf.dwType={}, "
    //     //       "diodf.dwOfs={}",
    //     //       hr, FromObjType(diodf.dwType), diodf.dwType, diodf.dwOfs));
    //     // }

    //     // LOG_DEBUG(fmt::format(
    //     //     "{} | dwOfs={}, dwType={}, FIELD_OFFSET={}",
    //     //     diodf.pguid ? FromGuidType(*diodf.pguid) : "[no guid]",
    //     //     diodf.dwOfs, diodf.dwType, FIELD_OFFSET(DIJOYSTATE2,
    //     //     rglVSlider[0])));
    //   }
    // }
    // new_dev->GetInstance()->EnumObjects(&DEBUG_EnumObjects, nullptr,
    //                                     DIDFT_AXIS | DIDFT_BUTTON |
    //                                     DIDFT_POV);
  }

  return new_dev;
}

bool ReleaseDevice(const std::string &guid) {
  LOG_DEBUG(fmt::format("Releasing dinput device with GUID {}", guid));

  if (created_devices.count(guid) == 0) {
    LOG_ERROR(fmt::format("Cannot delete device {}: not found", guid));

    return false;
  }

  std::unique_lock lck(mtx_devs);
  std::shared_ptr<Device> dev = created_devices[guid];

  dev->Release();
  created_devices.erase(guid);

  LOG_INFO(fmt::format("Device {} was released", guid));

  return true;
}

bool IsDeviceCreated(const std::string &guid) {
  return created_devices.count(guid) != 0;
}

std::shared_ptr<Device> GetDevice(const std::string &guid) {
  return IsDeviceCreated(guid) ? created_devices[guid] : nullptr;
}

const DeviceMap &GetDeviceMap() { return created_devices; }

void PollDeviceStates() {
  const auto &devices = dinput::GetDeviceMap();
  std::unique_lock lck(mtx_devs);

  if (devices.empty()) {
    return;
  }

  for (const auto &[_, dev_info] : devices) {
    LPDIRECTINPUTDEVICE8 dev = dev_info->GetInstance();
    HRESULT hr = dev->Poll();

    if (FAILED(hr)) {
      hr = dev->Acquire();

      while (hr == DIERR_INPUTLOST) hr = dev->Acquire();

      continue;
    }

    DeviceState state;

    switch (dev_info->GetType() & 0xFF) {
      case DI8DEVTYPE_KEYBOARD: {
        std::array<BYTE, 256> kb_state;

        if (SUCCEEDED(hr = ProcessDeviceState(*dev_info, kb_state.data(),
                                              kb_state.size())))
          state = kb_state;

        break;
      }
      case DI8DEVTYPE_DRIVING:
      case DI8DEVTYPE_GAMEPAD:
      case DI8DEVTYPE_JOYSTICK:
      case DI8DEVTYPE_SUPPLEMENTAL: {
        DIJOYSTATE2 js;

        if (SUCCEEDED(hr = ProcessDeviceState(*dev_info, &js, sizeof(js))))
          state = js;

        break;
      }
    }

    if (SUCCEEDED(hr)) {
      sig_device_state(dev_info, state);
    }
  }
}

}  // namespace smgm::dinput
