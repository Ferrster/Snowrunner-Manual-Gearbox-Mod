#pragma once

#include <unordered_map>

#include "smgm/input/v2/device.h"
#include "smgm/input/v2/device_id.h"
#include "smgm/input/v2/input_binding.h"

namespace smgm::input::v2 {
class InputBindingsManager final {
 public:
  bool ProcessBindings(const Device& device, const InputState& state);
  bool AddBinding(std::shared_ptr<InputBinding> binding);

 private:
  bool CanTrigger(const Device& device, const InputState& state,
                  std::shared_ptr<InputBinding>& binding);

 private:
  std::unordered_map<DeviceID, std::vector<std::shared_ptr<InputBinding>>>
      bindings_;
};
}  // namespace smgm::input::v2
