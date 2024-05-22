#pragma once

#include <cstdint>

#include "smgm/input/v2/input_object.h"

namespace smgm::input::v2 {
class Device;

class InputState {
 public:
  virtual KeyInputObject::InputValueType GetKeyState(
      const KeyInputObject&) const {
    return KeyInputObject::InvalidValue;
  }
  virtual AxisInputObject::InputValueType GetAxisState(
      const AxisInputObject&) const {
    return AxisInputObject::InvalidValue;
  }
  virtual POVInputObject::InputValueType GetPOVState(
      const POVInputObject&) const {
    return POVInputObject::InvalidValue;
  }
  virtual bool ReadFromDevice(const Device&) { return false; };

 protected:
  virtual ~InputState() = default;
};
}  // namespace smgm::input::v2
