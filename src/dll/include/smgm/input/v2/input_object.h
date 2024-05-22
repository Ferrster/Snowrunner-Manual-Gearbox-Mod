#pragma once

#include <cstdint>
#include <string>

#include "Windows.h"
#include "dinput.h"
#include "smgm/input/v2/input_id.h"

namespace smgm::input::v2 {

class InputObject {
 public:
  InputObject(const InputID& id);

  inline const InputID& GetInputID() const noexcept { return id_; }

 protected:
  virtual ~InputObject() = default;

 protected:
  InputID id_;
};

enum class KeyState { kUnknown = -1, kReleased, kPressed };

class KeyInputObject : virtual public InputObject {
 public:
  using InputValueType = KeyState;
  inline static InputValueType InvalidValue = KeyState::kUnknown;
  using InputObject::InputObject;
};

class AxisInputObject : virtual public InputObject {
 public:
  using InputValueType = std::int32_t;
  inline static InputValueType InvalidValue =
      std::numeric_limits<InputValueType>::min();
  using InputObject::InputObject;
};

class POVInputObject : virtual public InputObject {
 public:
  using InputValueType = std::int32_t;
  inline static InputValueType InvalidValue =
      std::numeric_limits<InputValueType>::min();
  using InputObject::InputObject;
};

}  // namespace smgm::input::v2
