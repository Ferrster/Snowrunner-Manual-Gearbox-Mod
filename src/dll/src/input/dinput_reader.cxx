#include "smgm/input/dinput_reader.h"

namespace smgm::input {
bool DirectInputReader::Process(const dinput::Device& dev_info,
                                const dinput::DeviceState& state) {
  switch (dev_info.type & 0xFF) {
    case DI8DEVTYPE_KEYBOARD: {
      ProcessKeys(dev_info, state);

      break;
    }
    case DI8DEVTYPE_DRIVING:
    case DI8DEVTYPE_GAMEPAD:
    case DI8DEVTYPE_JOYSTICK:
    case DI8DEVTYPE_SUPPLEMENTAL: {
      ProcessJoystick(dev_info, state);

      break;
    }
  }

  last_states_.insert_or_assign(dev_info.guid, state);

  return true;
}

void DirectInputReader::ProcessKeys(const dinput::Device& dev_info,
                                    const dinput::DeviceState& state) {
  const dinput::DeviceState* last_state = last_states_.count(dev_info.guid) != 0
                                              ? &last_states_[dev_info.guid]
                                              : nullptr;
  for (std::size_t i = 0; i < dinput::GetMaxKeys(state); ++i) {
    const dinput::KeyState last_key_state =
        last_state
            ? dinput::GetKeyState(*last_state, static_cast<dinput::Key>(i))
            : dinput::KeyState::kReleased;
    const dinput::KeyState cur_key_state =
        dinput::GetKeyState(state, static_cast<dinput::Key>(i));

    if (cur_key_state != last_key_state) {
      OnKeyStateChanged(dev_info, state, static_cast<dinput::Key>(i),
                        cur_key_state);
    }
  }
}

bool DirectInputReader::ProcessJoystick(const dinput::Device& dev_info,
                                        const dinput::DeviceState& var_state) {
  ProcessKeys(dev_info, var_state);

  return true;
}

void DirectInputReader::OnKeyStateChanged(
    const dinput::Device& dev_info, const dinput::DeviceState& var_dev_state,
    dinput::Key key, dinput::KeyState key_state) {
  LOG_DEBUG(fmt::format(
      "[ {} | {}] Key {} {}", dev_info.name, dev_info.guid, key,
      key_state == dinput::KeyState::kPressed ? "pressed" : "released"));

  sig_key_state_changed(dev_info, var_dev_state, key, key_state);
}
}  // namespace smgm::input
