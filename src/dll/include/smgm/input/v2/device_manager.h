#pragma once

#include <vector>

#include "smgm/input/v2/device.h"
#include "smgm/input/v2/dinput/device.h"

namespace smgm::input::v2 {
class DeviceManager final {
 public:
  bool Add(std::shared_ptr<Device> dev);
  void Remove(std::shared_ptr<Device> dev);
  void RemoveByID(const std::string& id);
  const auto& GetDevices() const noexcept { return devs_; }
  std::shared_ptr<dinput::DI_Device> CreateDirectInputDevice(
      const std::string& str_guid);
  std::vector<std::shared_ptr<Device>> RefreshXInputDevices();

 private:
  std::vector<std::shared_ptr<Device>> devs_;
};
}  // namespace smgm::input::v2
