#include "smgm/input/v2/input_reader.h"

#include <fmt/format.h>

#include "smgm/utils/logging.h"

namespace smgm::input::v2 {
bool InputReader::ProcessInput(
    const std::vector<std::shared_ptr<Device>>& devices) {
  if (devices.empty()) return false;

  for (const auto& device : devices) {
    const auto cur_state = device->GetCurrentState();

    if (last_states_.count(device) == 0) {
      if (cur_state) {
        last_states_.insert({device, cur_state});
        device->sig_destroyed.connect([this, device] { ClearStates(device); });
      }

      continue;
    }

    if (!cur_state) continue;

    const auto& input_objs = device->GetInputObjects();
    const auto& last_state = last_states_[device];

    for (const auto& input_obj : input_objs) {
      const InputID& input_id = input_obj->GetInputID();

      switch (input_id.type) {
        case InputType::kKey:
        case InputType::kButton: {
          const auto key_input_obj =
              std::dynamic_pointer_cast<const KeyInputObject>(input_obj);
          const KeyInputObject::InputValueType cur_key_state =
                                                   cur_state->GetKeyState(
                                                       *key_input_obj),
                                               last_key_state =
                                                   last_state->GetKeyState(
                                                       *key_input_obj);

          if (cur_key_state != last_key_state) {
            LOG_DEBUG(fmt::format("Key {} changed state to {}",
                                  input_id.ToString(), (int)cur_key_state));
          }
          break;
        }
        case InputType::kAxis: {
          const auto axis_input_obj =
              std::dynamic_pointer_cast<const AxisInputObject>(input_obj);
          const AxisInputObject::InputValueType cur_axis_state =
                                                    cur_state->GetAxisState(
                                                        *axis_input_obj),
                                                last_axis_state =
                                                    last_state->GetAxisState(
                                                        *axis_input_obj);

          if (cur_axis_state != last_axis_state) {
            LOG_DEBUG(fmt::format("Axis {} changed value to {}",
                                  input_id.ToString(), cur_axis_state));
          }

          break;
        }
        case InputType::kPOV: {
          const auto pov_input_obj =
              std::dynamic_pointer_cast<const POVInputObject>(input_obj);
          const POVInputObject::InputValueType cur_pov_state =
                                                   cur_state->GetPOVState(
                                                       *pov_input_obj),
                                               last_pov_state =
                                                   last_state->GetPOVState(
                                                       *pov_input_obj);

          if (cur_pov_state != last_pov_state) {
            LOG_DEBUG(fmt::format("POV {} changed value to {}",
                                  input_id.ToString(), cur_pov_state));
          }

          break;
        }
        case InputType::kUnknown:
        default:
          LOG_DEBUG(fmt::format("Unknown input {}", input_id.ToString()));
      }
    }

    UpdateLastState(device, cur_state);
  }

  return true;
}

void InputReader::ClearStates(const std::shared_ptr<Device>& device) {
  last_states_.erase(device);
}

void InputReader::UpdateLastState(const std::shared_ptr<Device>& device,
                                  const std::shared_ptr<InputState>& state) {
  last_states_.insert_or_assign(device, state);
}

std::shared_ptr<InputState> InputReader::GetLastInputState(
    const std::shared_ptr<Device>& device) const {
  if (last_states_.count(device) == 0) {
    return nullptr;
  }

  return last_states_.at(device);
}
}  // namespace smgm::input::v2