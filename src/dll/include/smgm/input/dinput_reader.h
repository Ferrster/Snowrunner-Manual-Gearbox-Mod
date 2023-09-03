#pragma once

#include <boost/signals2.hpp>
#include <unordered_map>

#include "smgm/utils/dinput_tools.h"

namespace smgm::input {

class DirectInputReader {
 public:
  struct Keybinding {};

  bool Process(const dinput::Device& dev_info,
               const dinput::DeviceState& state);

 private:
  bool ProcessJoystick(const dinput::Device& dev_info,
                       const dinput::DeviceState& state);
  void ProcessKeys(const dinput::Device& dev_info,
                   const dinput::DeviceState& state);
  void OnKeyStateChanged(const dinput::Device& dev_info,
                         const dinput::DeviceState& dev_state, dinput::Key key,
                         dinput::KeyState key_state);

  std::unordered_map<std::string, dinput::DeviceState> last_states_;
  boost::signals2::signal<void(const dinput::Device&,
                               const dinput::DeviceState&, dinput::Key,
                               dinput::KeyState)>
      sig_key_state_changed;
};

}  // namespace smgm::input
