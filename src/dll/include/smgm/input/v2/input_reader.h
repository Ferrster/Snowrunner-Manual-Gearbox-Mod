#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "smgm/input/v2/device.h"
#include "smgm/input/v2/input_state.h"

namespace smgm::input::v2 {
class InputReader {
 public:
  bool ProcessInput(const std::vector<std::shared_ptr<Device>>& devices);

 protected:
  void ClearStates(const std::shared_ptr<Device>& device);
  void UpdateLastState(const std::shared_ptr<Device>& device,
                       const std::shared_ptr<InputState>& state);

 private:
  std::unordered_map<std::shared_ptr<Device>, std::shared_ptr<InputState>>
      last_states_;
};
}  // namespace smgm::input::v2
