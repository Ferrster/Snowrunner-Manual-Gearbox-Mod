#include "smgm/input/v2/xinput/input_state.h"

#include <fmt/format.h>

#include "smgm/input/v2/xinput/device.h"
#include "smgm/input/v2/xinput/input_object.h"
#include "smgm/utils/logging.h"

namespace smgm::input::v2::xinput {
bool XI_InputState::ReadFromDevice(const Device& device) {
  const auto& xi_device = static_cast<const XI_Device&>(device);

  std::memset(&data_, 0, sizeof(data_));

  if (DWORD res = XInputGetState(xi_device.GetUserID(), &data_);
      res == ERROR_SUCCESS) {
    return true;
  } else if (res == ERROR_DEVICE_NOT_CONNECTED) {
    LOG_WARNING(fmt::format(
        "[XInput] Failed to read state of device {}: not connected!",
        xi_device.GetDisplayName()));
  } else {
    LOG_WARNING(fmt::format(
        "[XInput] Failed to read state of device {}: error code {}!",
        xi_device.GetDisplayName(), res));
  }

  return false;
}

KeyInputObject::InputValueType XI_InputState::GetKeyState(
    const KeyInputObject& input_obj) const {
  const auto& xi_input = static_cast<const XI_KeyInputObject&>(input_obj);
  const auto val = data_.Gamepad.wButtons & xi_input.GetButton();

  return val > 0 ? KeyState::kPressed : KeyState::kReleased;
}

AxisInputObject::InputValueType XI_InputState::GetAxisState(
    const AxisInputObject& input_obj) const {
  const auto& xi_input = static_cast<const XI_AxisInputObject&>(input_obj);

  switch (xi_input.GetInputType()) {
    case XI_InputType::kLTrigger:
      return data_.Gamepad.bLeftTrigger;
    case XI_InputType::kRTrigger:
      return data_.Gamepad.bRightTrigger;
    case XI_InputType::kAxis: {
      const auto& input_id = xi_input.GetInputID();

      switch (static_cast<AxisInstance>(input_id.instance)) {
        case AxisInstance::kLX:
          return data_.Gamepad.sThumbLX;
        case AxisInstance::kLY:
          return data_.Gamepad.sThumbLY;
        case AxisInstance::kRX:
          return data_.Gamepad.sThumbRX;
        case AxisInstance::kRY:
          return data_.Gamepad.sThumbRY;
      }
    }
    default:
      return AxisInputObject::InvalidValue;
  }
}

}  // namespace smgm::input::v2::xinput
