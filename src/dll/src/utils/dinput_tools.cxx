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
boost::signals2::signal<void(const Device &)> sig_device_created;
boost::signals2::signal<void(const Device &)> sig_device_released;
boost::signals2::signal<void(const Device &, std::any)> sig_device_state;
std::unordered_map<std::string, Device> created_devices;

std::shared_mutex mtx_devs;

template <typename T>
HRESULT ProcessDeviceState(const Device &dev_info, T *out_state,
                           std::size_t size) {
  if (!out_state) return -1;

  HRESULT res;

  if (FAILED(res = dev_info.instance->GetDeviceState(static_cast<DWORD>(size),
                                                     out_state))) {
    LOG_WARNING(
        fmt::format("[{}] GetDeviceState failed ({})", dev_info.name, res));
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

  dinput_inst->Release();

  LOG_INFO("DirectInput8 released");
}

bool CreateDevice(const std::string &str_guid, Device *out_dev) {
  LOG_INFO(fmt::format("Creating DirectInput device with GUID {}", str_guid));

  auto opt_guid = ToGUID(str_guid);

  if (!opt_guid.has_value()) {
    LOG_ERROR(
        fmt::format("Cannot create dinput device {}: invalid GUID format"));

    return false;
  }

  GUID guid = opt_guid.value();
  LPDIRECTINPUTDEVICE8 device;

  if (auto res = dinput::dinput_inst->CreateDevice(guid, &device, NULL);
      res != DI_OK) {
    LOG_ERROR(
        fmt::format("Cannot create dinput device {}: CreateDevice failed ({})!",
                    str_guid, res));

    return false;
  }

  DIDEVICEINSTANCE didi;

  didi.dwSize = sizeof(didi);

  if (HRESULT res; FAILED(res = device->GetDeviceInfo(&didi))) {
    LOG_ERROR(fmt::format(
        "Cannot create dinput device {}: GetDeviceInfo failed ({})!", str_guid,
        res));

    device->Release();

    return false;
  }

  const auto *df = [&]() -> const DIDATAFORMAT * {
    switch (didi.dwDevType & 0xFF) {
      case DI8DEVTYPE_KEYBOARD:
        return &c_dfDIKeyboard;
      case DI8DEVTYPE_DRIVING:
      case DI8DEVTYPE_GAMEPAD:
      case DI8DEVTYPE_JOYSTICK:
      case DI8DEVTYPE_SUPPLEMENTAL:
        return &c_dfDIJoystick2;
      default:
        return nullptr;
    }
  }();

  if (!df) {
    LOG_ERROR(fmt::format(
        "Cannot create dinput device {}: Unsupported device type {}", str_guid,
        didi.dwDevType));

    device->Release();

    return false;
  }

  if (auto res = device->SetDataFormat(df); res != DI_OK) {
    LOG_ERROR(fmt::format(
        "Cannot create dinput device {}: SetDataFormat failed ({})!", str_guid,
        res));

    device->Release();

    return false;
  }

  if (auto res = device->SetCooperativeLevel(
          smgm::game_window, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
      res != DI_OK) {
    LOG_ERROR(fmt::format(
        "Cannot create dinput device {}: SetCooperativeLevel failed ({})!",
        str_guid, res));

    device->Release();

    return false;
  }

  Device new_dev(device, didi);

  created_devices.insert({str_guid, new_dev});
  if (out_dev) *out_dev = new_dev;
  sig_device_created(new_dev);

  LOG_INFO(fmt::format("Device {} created = {}", str_guid, (void *)device));

  return true;
}

bool ReleaseDevice(const std::string &guid) {
  LOG_DEBUG(fmt::format("Releasing dinput device with GUID {}", guid));

  if (created_devices.count(guid) == 0) {
    LOG_ERROR(fmt::format("Cannot delete device {}: not found", guid));

    return false;
  }

  std::unique_lock lck(mtx_devs);
  Device dev = created_devices[guid];

  dev.instance->Release();
  created_devices.erase(guid);
  sig_device_released(dev);

  LOG_INFO(fmt::format("Device {} was released", guid));

  return true;
}

bool IsDeviceCreated(const std::string &guid) {
  return created_devices.count(guid) != 0;
}

Device GetDevice(const std::string &guid) {
  return IsDeviceCreated(guid) ? created_devices[guid] : Device{};
}

const DeviceMap &GetDeviceMap() { return created_devices; }

void PollDeviceStates() {
  const auto &devices = dinput::GetDeviceMap();
  std::unique_lock lck(mtx_devs);

  if (devices.empty()) {
    return;
  }

  for (const auto &[_, dev_info] : devices) {
    LPDIRECTINPUTDEVICE8 dev = dev_info.instance;
    HRESULT hr = dev->Poll();

    if (FAILED(hr)) {
      hr = dev->Acquire();

      while (hr == DIERR_INPUTLOST) hr = dev->Acquire();

      continue;
    }

    std::any state;

    switch (dev_info.type & 0xFF) {
      case DI8DEVTYPE_KEYBOARD: {
        std::array<BYTE, 256> kb_state;

        if (SUCCEEDED(hr = ProcessDeviceState(dev_info, kb_state.data(),
                                              kb_state.size())))
          state = kb_state;

        break;
      }
      case DI8DEVTYPE_DRIVING:
      case DI8DEVTYPE_GAMEPAD:
      case DI8DEVTYPE_JOYSTICK:
      case DI8DEVTYPE_SUPPLEMENTAL: {
        DIJOYSTATE2 js;

        if (SUCCEEDED(hr = ProcessDeviceState(dev_info, &js, sizeof(js))))
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
