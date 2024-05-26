#include "smgm/input/v2/input_bindings_manager.h"

#include "smgm/utils/logging.h"

namespace smgm::input::v2 {
bool InputBindingsManager::ProcessBindings(const Device& device,
                                           const InputState& state) {
  if (bindings_.count(device.GetID()) == 0) return false;

  auto& device_bindings = bindings_[device.GetID()];

  for (auto& binding : device_bindings) {
    if (CanTrigger(device, state, binding)) {
      if (!binding->IsActive()) {
        binding->SetActive(true);
      }
    } else {
      binding->SetActive(false);
    }
  }

  return true;
}

bool InputBindingsManager::AddBinding(std::shared_ptr<InputBinding> binding) {
  bindings_[binding->GetTargetDeviceID()].emplace_back(binding);

  LOG_INFO(fmt::format("Added binding for device '{}'",
                       binding->GetTargetDeviceID()));

  return true;
}

bool InputBindingsManager::CanTrigger(const Device& device,
                                      const InputState& state,
                                      std::shared_ptr<InputBinding>& binding) {
  const auto& inputs = binding->GetInputs();

  for (const auto& input_id : inputs) {
    // TODO: test code for keys only
    if (input_id.type != InputType::kButton) return false;

    const auto input_obj = device.FindInputObject(input_id);

    if (!input_obj) {
      return false;
    }

    switch (input_id.type) {
      case InputType::kButton: {
        const auto key_input_obj =
            std::dynamic_pointer_cast<const KeyInputObject>(input_obj);
        const auto key_state = state.GetKeyState(*key_input_obj);

        if (key_state != KeyState::kPressed) {
          return false;
        }
        break;
      }
      default:
        return false;
    }
  }

  return true;
}
}  // namespace smgm::input::v2
