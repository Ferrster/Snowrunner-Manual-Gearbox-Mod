#pragma once

#include <boost/signals2/signal.hpp>
#include <vector>

#include "smgm/actions/action.h"
#include "smgm/input/v2/device_id.h"
#include "smgm/input/v2/input_id.h"

namespace smgm::input::v2 {
class InputBinding {
 public:
  InputBinding(const DeviceID& target_device,
               const std::vector<InputID>& inputs,
               std::shared_ptr<Action> action);

  inline const auto& GetTargetDeviceID() const noexcept {
    return target_device_;
  }

  inline const auto& GetInputs() const noexcept { return inputs_; }

  inline bool IsActive() const noexcept { return is_active_; }

  void SetActive(bool state);

  bool BindAction(std::shared_ptr<Action> action);

  bool Trigger();

 public:
  boost::signals2::signal<void(const InputBinding&, bool)> sig_active_changed;

 protected:
  DeviceID target_device_;
  std::vector<InputID> inputs_;
  bool is_active_ = false;
  std::shared_ptr<Action> action_;
};
}  // namespace smgm::input::v2
