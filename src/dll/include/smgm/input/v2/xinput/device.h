#pragma once

#include "Windows.h"
#include "smgm/input/v2/device.h"

namespace smgm::input::v2::xinput {
class XI_Device final : public Device {
 public:
  XI_Device(DWORD user_id);

  std::shared_ptr<InputState> GetCurrentState() const override;
  inline DWORD GetUserID() const noexcept { return user_id_; }

 protected:
  DWORD user_id_;
};
}  // namespace smgm::input::v2::xinput
