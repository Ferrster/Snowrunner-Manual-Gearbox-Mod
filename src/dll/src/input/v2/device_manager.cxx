#include "smgm/input/v2/device_manager.h"

#include <fmt/format.h>

#include <boost/range/algorithm_ext/erase.hpp>

#include "Xinput.h"
#include "smgm/input/v2/dinput/device.h"
#include "smgm/input/v2/xinput/device.h"
#include "smgm/smgm.h"
#include "smgm/utils/dinput_tools.h"
#include "smgm/utils/guid.h"
#include "smgm/utils/logging.h"

namespace smgm::input::v2 {
bool DeviceManager::Add(std::shared_ptr<Device> dev) {
  if (std::find(begin(devs_), end(devs_), dev) != end(devs_)) {
    return false;
  }

  devs_.emplace_back(dev);

  return true;
}

void DeviceManager::Remove(std::shared_ptr<Device> dev) {
  boost::remove_erase(devs_, dev);
}

void DeviceManager::RemoveByID(const std::string& id) {
  boost::remove_erase_if(devs_, [&](const std::shared_ptr<Device>& el) {
    return el->GetID() == id;
  });
}

std::shared_ptr<dinput::DI_Device> DeviceManager::CreateDirectInputDevice(
    const std::string& str_guid) {
  LOG_DEBUG("DeviceManager");
  LOG_INFO(fmt::format("Creating DirectInput device with GUID {}", str_guid));

  auto opt_guid = ToGUID(str_guid);

  if (!opt_guid.has_value()) {
    LOG_ERROR(
        fmt::format("Cannot create dinput device {}: invalid GUID format"));

    return nullptr;
  }

  GUID guid = opt_guid.value();
  LPDIRECTINPUTDEVICE8 device;

  if (auto res = smgm::dinput::GetInstance()->CreateDevice(guid, &device, NULL);
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

  const auto* df = [&]() -> LPCDIDATAFORMAT {
    switch (didi.dwDevType & 0xFF) {
      case DI8DEVTYPE_KEYBOARD:
        return &c_dfDIKeyboard;
      case DI8DEVTYPE_DRIVING:
      case DI8DEVTYPE_GAMEPAD:
      case DI8DEVTYPE_JOYSTICK:
      case DI8DEVTYPE_SUPPLEMENTAL: {
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

  auto new_dev = std::make_shared<dinput::DI_Device>(device, didi, df);

  // new_dev->sig_device_released.connect([wp_dev = std::weak_ptr(new_dev)] {
  //   if (auto dev = wp_dev.lock()) {
  //     sig_device_released(dev);
  //   }
  // });
  // created_devices.insert({str_guid, new_dev});
  // sig_device_created(new_dev);
  devs_.emplace_back(new_dev);

  LOG_INFO(fmt::format("Device {} created = {}", str_guid, (void*)device));

  return new_dev;
}

std::vector<std::shared_ptr<Device>> DeviceManager::RefreshXInputDevices() {
  std::vector<std::shared_ptr<Device>> new_devices;

  new_devices.reserve(4);

  std::uint8_t present_user_ids_mask = 0;
  static XINPUT_STATE dummy_state;

  // Remove disconnected devices
  {
    auto it = begin(devs_);

    while (it != end(devs_)) {
      it = std::find_if(
          it, end(devs_),
          [&present_user_ids_mask](const std::shared_ptr<Device>& device) {
            if (device->GetAPI() != Device::API::kXInput) return false;

            const auto& xi_device =
                std::static_pointer_cast<const xinput::XI_Device>(device);
            const DWORD err =
                XInputGetState(xi_device->GetUserID(), &dummy_state);

            if (err == ERROR_SUCCESS) {
              present_user_ids_mask |= (1 << xi_device->GetUserID());
            }

            return err == ERROR_DEVICE_NOT_CONNECTED;
          });

      if (it != end(devs_)) {
        devs_.erase(it);
      }
    }
  }

  for (DWORD user_idx = 0; user_idx < XUSER_MAX_COUNT; ++user_idx) {
    if (present_user_ids_mask & (1 << user_idx)) continue;

    const DWORD err = XInputGetState(user_idx, &dummy_state);

    LOG_DEBUG(fmt::format("id={}, err={}", user_idx, err));

    if (err != ERROR_SUCCESS) continue;

    auto new_device = std::make_shared<xinput::XI_Device>(user_idx);

    LOG_INFO(fmt::format("Created XInput device \"{}\"",
                         new_device->GetDisplayName()));

    devs_.emplace_back(new_device);
    new_devices.emplace_back(new_device);
  }

  return new_devices;
}
}  // namespace smgm::input::v2
