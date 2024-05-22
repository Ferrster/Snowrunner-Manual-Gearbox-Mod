#pragma once

#include "Windows.h"
#include "Xinput.h"
#include "smgm/input/v2/input_state.h"

namespace smgm::input::v2::xinput {
class XI_InputState : public InputState {
 public:
  inline const auto& GetData() const noexcept { return data_; }
  virtual bool ReadFromDevice(const Device& device) override;
  virtual KeyInputObject::InputValueType GetKeyState(
      const KeyInputObject& input_obj) const override;
  virtual AxisInputObject::InputValueType GetAxisState(
      const AxisInputObject& input_obj) const override;

 protected:
  XINPUT_STATE data_;
};
}  // namespace smgm::input::v2::xinput
