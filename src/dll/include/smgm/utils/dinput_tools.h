#pragma once

#include <array>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

#include "Windows.h"
#include "boost/signals2.hpp"
#include "dinput.h"
#include "smgm/dinput/device.h"
#include "smgm/input/key.h"
#include "smgm/utils/format_helpers.h"
#include "smgm/utils/logging.h"
#include "smgm/utils/type_traits.h"

namespace smgm::dinput {

using DeviceMap = std::unordered_map<std::string, std::shared_ptr<Device>>;

extern boost::signals2::signal<void(std::shared_ptr<Device>)>
    sig_device_created;
extern boost::signals2::signal<void(std::shared_ptr<Device>)>
    sig_device_released;
extern boost::signals2::signal<void(std::shared_ptr<Device>,
                                    const DeviceState &)>
    sig_device_state;

bool Init(HINSTANCE hinst);

void Release(HINSTANCE hinst);

IDirectInput8 *GetInstance();

std::shared_ptr<Device> CreateDevice(const std::string &guid);

bool ReleaseDevice(const std::string &guid);

bool IsDeviceCreated(const std::string &guid);

std::shared_ptr<Device> GetDevice(const std::string &guid);

const DeviceMap &GetDeviceMap();

void PollDeviceStates();

inline input::KeyState GetKeyState(const dinput::DeviceState &state,
                                   input::Key key) {
  input::Key key_state = std::visit(
      [key](auto &&arg) -> input::Key {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, KeyboardState>) {
          return arg[key];
        } else if constexpr (std::is_same_v<T, JoystickState>) {
          return arg.rgbButtons[key];
        } else {
          return 0;
        }
      },
      state);
  return key_state > 0 ? input::KeyState::kPressed : input::KeyState::kReleased;
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
          return std::numeric_limits<std::size_t>::max();
        }
      },
      state);
}

}  // namespace smgm::dinput
