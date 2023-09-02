#include "smgm/input/dinput_reader.h"

namespace smgm::input {
bool DirectInputReader::Process(const dinput::Device& dev_info,
                                const std::any& state) {
  switch (dev_info.type & 0xFF) {
    case DI8DEVTYPE_KEYBOARD: {
      ProcessKeyboard(dev_info, std::any_cast<dinput::KeyboardState>(state));

      break;
    }
    case DI8DEVTYPE_DRIVING:
    case DI8DEVTYPE_GAMEPAD:
    case DI8DEVTYPE_JOYSTICK:
    case DI8DEVTYPE_SUPPLEMENTAL: {
      ProcessJoystick(dev_info, std::any_cast<dinput::JoystickState>(state));

      break;
    }
  }

  return true;
}

bool DirectInputReader::ProcessKeyboard(const dinput::Device& dev_info,
                                        const dinput::KeyboardState& state) {
  return true;
}

bool DirectInputReader::ProcessJoystick(const dinput::Device& dev_info,
                                        const dinput::JoystickState& state) {
  // LOG_DEBUG(FormatDataTable("Joystick state", std::pair{"Xpos", state.lX},
  //                           std::pair{"Ypos", state.lY},
  //                           std::pair{"Zpos", state.lZ}));

  return true;
}

}  // namespace smgm::input
