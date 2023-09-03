#pragma once

#include <array>
#include <optional>
#include <type_traits>
#include <variant>

#include "Windows.h"
#include "boost/signals2.hpp"
#include "dinput.h"
#include "smgm/utils/format_helpers.h"
#include "smgm/utils/guid.h"
#include "smgm/utils/logging.h"
#include "smgm/utils/type_traits.h"

namespace smgm::dinput {

enum class KeyState { kReleased, kPressed };

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

using Key = std::uint8_t;
using DeviceMap = std::unordered_map<std::string, Device>;
using KeyboardState = std::array<Key, 256>;
using JoystickState = DIJOYSTATE2;
using DeviceState = std::variant<KeyboardState, JoystickState>;

extern IDirectInput8 *dinput_inst;
extern boost::signals2::signal<void(const Device &)> sig_device_created;
extern boost::signals2::signal<void(const Device &)> sig_device_released;
extern boost::signals2::signal<void(const Device &, const DeviceState &)>
    sig_device_state;

bool Init(HINSTANCE hinst);

void Release(HINSTANCE hinst);

bool CreateDevice(const std::string &guid, Device *out_device = nullptr);

bool ReleaseDevice(const std::string &guid);

bool IsDeviceCreated(const std::string &guid);

Device GetDevice(const std::string &guid);

const DeviceMap &GetDeviceMap();

void PollDeviceStates();

inline KeyState GetKeyState(const dinput::DeviceState &state, Key key) {
  Key key_state = std::visit(
      [key](auto &&arg) -> dinput::Key {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, KeyboardState>) {
          return arg[key];
        } else if constexpr (std::is_same_v<T, JoystickState>) {
          return arg.rgbButtons[key];
        } else {
          static_assert(smgm::always_false_v<T>,
                        "GetKeyState: invalid device state in std::visit");
        }
      },
      state);
  return key_state > 0 ? KeyState::kPressed : KeyState::kReleased;
}

inline constexpr std::size_t GetMaxKeys(const dinput::DeviceState &state) {
  return std::visit(
      [](auto &&arg) -> std::size_t {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, KeyboardState>) {
          return arg.size();
        } else if constexpr (std::is_same_v<T, JoystickState>) {
          return sizeof(arg.rgbButtons);
        } else {
          static_assert(smgm::always_false_v<T>,
                        "GetMaxKeys: invalid device state in std::visit");
        }
      },
      state);
}

}  // namespace smgm::dinput
