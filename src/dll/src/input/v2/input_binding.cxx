#include "smgm/input/v2/input_binding.h"

namespace smgm::input::v2 {
InputBinding::InputBinding(const DeviceID& target_device,
                           const std::vector<InputID>& inputs,
                           std::shared_ptr<Action> action)
    : target_device_(target_device), inputs_(inputs), action_(action) {
  sig_active_changed.connect([this](const InputBinding&, bool state) {
    if (state && action_) {
      Trigger();
    }
  });
}

void InputBinding::SetActive(bool state) {
  is_active_ = state;
  sig_active_changed(*this, state);
}

bool InputBinding::Trigger() {
  if (!action_) {
    return false;
  }

  return action_->Trigger();
}
}  // namespace smgm::input::v2
