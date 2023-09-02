#pragma once

#include <any>
#include <array>
#include <optional>

#include "Windows.h"
#include "boost/signals2.hpp"
#include "dinput.h"
#include "smgm/utils/format_helpers.h"
#include "smgm/utils/guid.h"
#include "smgm/utils/logging.h"

namespace smgm::dinput {

struct Device {
  Device() = default;
  Device(LPDIRECTINPUTDEVICE8 instance, const DIDEVICEINSTANCE &info)
      : instance(instance),
        guid(FromGUID(info.guidInstance)),
        name(std::string(info.tszInstanceName)),
        product_name(std::string(info.tszProductName)),
        type(info.dwDevType) {}

  operator bool() const noexcept { return instance; }

  LPDIRECTINPUTDEVICE8 instance;
  std::string guid;
  std::string name;
  std::string product_name;
  DWORD type;
};

using DeviceMap = std::unordered_map<std::string, Device>;
using KeyboardState = std::array<BYTE, 256>;
using JoystickState = DIJOYSTATE2;

extern IDirectInput8 *dinput_inst;
extern boost::signals2::signal<void(const Device &)> sig_device_created;
extern boost::signals2::signal<void(const Device &)> sig_device_released;
extern boost::signals2::signal<void(const Device &, std::any)> sig_device_state;

bool Init(HINSTANCE hinst);

void Release(HINSTANCE hinst);

bool CreateDevice(const std::string &guid, Device *out_device = nullptr);

bool ReleaseDevice(const std::string &guid);

bool IsDeviceCreated(const std::string &guid);

Device GetDevice(const std::string &guid);

const DeviceMap &GetDeviceMap();

void PollDeviceStates();

}  // namespace smgm::dinput
