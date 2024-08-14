#pragma once

#include <array>
#include <cstdint>

#include "Windows.h"
#include "smgm/input/key.h"
#include "smgm/input/v2/dinput/device.h"
#include "smgm/input/v2/dinput/input_object.h"
#include "smgm/input/v2/input_state.h"

namespace {
template <typename InputDataT, typename TargetInputT, typename InputT,
          typename StateDataT>
InputDataT ReadObjectValue(const StateDataT& data, const InputT& input_obj) {
  const auto& di_input = static_cast<const TargetInputT&>(input_obj);
  const auto val = *reinterpret_cast<const InputDataT*>(
      reinterpret_cast<const BYTE*>(&data) + di_input.GetFormat().dwOfs);

  return val;
}
}  // namespace

namespace smgm::input::v2::dinput {

template <typename T>
class DI_InputState : public InputState {
 public:
  inline const auto& GetData() const noexcept { return data_; }
  virtual bool ReadFromDevice(const Device& device) override {
    const auto& di_device = static_cast<const DI_Device&>(device);

    if (HRESULT hr; FAILED(hr = di_device.GetInstance()->GetDeviceState(
                               sizeof(data_), &data_))) {
      return false;
    }

    return true;
  }
  virtual KeyInputObject::InputValueType GetKeyState(
      const KeyInputObject& input_obj) const override {
    const auto val =
        ::ReadObjectValue<BYTE, DI_KeyInputObject>(GetData(), input_obj);

    return val > 0 ? KeyState::kPressed : KeyState::kReleased;
  }
  virtual AxisInputObject::InputValueType GetAxisState(
      const AxisInputObject& input_obj) const override {
    return ::ReadObjectValue<LONG, DI_AxisInputObject>(GetData(), input_obj);
  }
  virtual POVInputObject::InputValueType GetPOVState(
      const POVInputObject& input_obj) const override {
    return ::ReadObjectValue<DWORD, DI_POVInputObject>(GetData(), input_obj);
  }

 protected:
  T data_{};
};

using DI_KeyboardState = DI_InputState<std::array<BYTE, 256>>;
using DI_JoystickState = DI_InputState<DIJOYSTATE2>;

}  // namespace smgm::input::v2::dinput
