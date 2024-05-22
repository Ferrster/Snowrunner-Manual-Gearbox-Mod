#pragma once

#include "smgm/input/v2/input_object.h"

namespace {}

namespace smgm::input::v2::xinput {
enum class XI_InputType { kUnknown = -1, kButton, kAxis, kLTrigger, kRTrigger };
enum class AxisInstance { kLX, kLY, kRX, kRY };

class XI_InputObject : virtual public InputObject {
 public:
  XI_InputObject(const InputID& id, XI_InputType input_type)
      : InputObject(id), input_type_(input_type) {}

  inline XI_InputType GetInputType() const noexcept { return input_type_; }

 protected:
  XI_InputType input_type_ = XI_InputType::kUnknown;
};

class XI_KeyInputObject final : public XI_InputObject, public KeyInputObject {
 public:
  XI_KeyInputObject(const InputID& id, WORD btn)
      : InputObject(id),
        XI_InputObject(id, XI_InputType::kButton),
        KeyInputObject(id),
        btn_(btn) {}

  inline WORD GetButton() const noexcept { return btn_; }

 protected:
  WORD btn_;
};

class XI_AxisInputObject final : public XI_InputObject, public AxisInputObject {
 public:
  XI_AxisInputObject(const InputID& id, XI_InputType input_type)
      : InputObject(id), XI_InputObject(id, input_type), AxisInputObject(id) {}
};
}  // namespace smgm::input::v2::xinput
