#pragma once

#include "smgm/utils/dinput_tools.h"

namespace smgm::input {

class DirectInputReader {
 public:
  struct Keybinding {};

  bool Process(const dinput::Device& dev_info, const std::any& state);

 private:
  bool ProcessKeyboard(const dinput::Device& dev_info,
                       const dinput::KeyboardState& state);
  bool ProcessJoystick(const dinput::Device& dev_info,
                       const dinput::JoystickState& state);
};

}  // namespace smgm::input
