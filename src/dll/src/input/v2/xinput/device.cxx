#include "smgm/input/v2/xinput/device.h"

#include <fmt/format.h>

#include <array>

#include "Windows.h"
#include "Xinput.h"
#include "smgm/input/v2/xinput/input_object.h"
#include "smgm/input/v2/xinput/input_state.h"

namespace smgm::input::v2::xinput {
XI_Device::XI_Device(DWORD user_id)
    : Device(API::kXInput, fmt::format("xinput{}", user_id),
             fmt::format("xinput{}", user_id),
             fmt::format("XInput Controller {}", user_id)),
      user_id_(user_id) {
  static const auto kInputObjs = [] {
    std::vector<std::shared_ptr<InputObject>> result;
    constexpr int kNumButtons = 14, kNumTriggers = 2, kNumAxis = 4;

    result.reserve(kNumButtons + kNumTriggers + kNumAxis);

    const std::array<WORD, kNumButtons> kButtons{
        XINPUT_GAMEPAD_DPAD_UP,
        XINPUT_GAMEPAD_DPAD_DOWN,
        XINPUT_GAMEPAD_DPAD_LEFT,
        XINPUT_GAMEPAD_DPAD_RIGHT,
        XINPUT_GAMEPAD_START,
        XINPUT_GAMEPAD_BACK,
        XINPUT_GAMEPAD_LEFT_THUMB,
        XINPUT_GAMEPAD_RIGHT_THUMB,
        XINPUT_GAMEPAD_LEFT_SHOULDER,
        XINPUT_GAMEPAD_RIGHT_SHOULDER,
        XINPUT_GAMEPAD_A,
        XINPUT_GAMEPAD_B,
        XINPUT_GAMEPAD_X,
        XINPUT_GAMEPAD_Y,
    };

    for (unsigned short i = 0; i < kButtons.size(); ++i) {
      result.emplace_back(std::make_shared<XI_KeyInputObject>(
          InputID(v2::InputType::kButton, i), kButtons[i]));
    }

    for (unsigned short i = 0; i < kNumAxis; ++i) {
      result.emplace_back(std::make_shared<XI_AxisInputObject>(
          InputID(v2::InputType::kAxis, i), XI_InputType::kAxis));
    }

    result.emplace_back(std::make_shared<XI_AxisInputObject>(
        InputID(v2::InputType::kAxis, kNumAxis + 0), XI_InputType::kLTrigger));
    result.emplace_back(std::make_shared<XI_AxisInputObject>(
        InputID(v2::InputType::kAxis, kNumAxis + 1), XI_InputType::kRTrigger));

    return result;
  }();

  input_objs_ = kInputObjs;
}

std::shared_ptr<InputState> XI_Device::GetCurrentState() const {
  auto state = std::make_shared<XI_InputState>();

  if (!state->ReadFromDevice(*this)) {
    return nullptr;
  }

  return state;
}
}  // namespace smgm::input::v2::xinput
