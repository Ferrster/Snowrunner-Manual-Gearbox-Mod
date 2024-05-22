
#pragma once

#include <boost/describe.hpp>
#include <functional>
#include <string>

namespace smgm {
namespace input {

BOOST_DEFINE_ENUM(InputDeviceType, KEYBOARD, JOYSTICK)
BOOST_DEFINE_ENUM(InputAction, SHIFT_1_GEAR, SHIFT_2_GEAR, SHIFT_3_GEAR,
                  SHIFT_4_GEAR, SHIFT_5_GEAR, SHIFT_6_GEAR, SHIFT_7_GEAR,
                  SHIFT_8_GEAR, SHIFT_HIGH_GEAR, SHIFT_LOW_GEAR,
                  SHIFT_LOW_PLUS_GEAR, SHIFT_LOW_MINUS_GEAR, SHIFT_NEUTRAL,
                  SHIFT_PREV_AUTO_GEAR, SHIFT_NEXT_AUTO_GEAR,
                  SHIFT_REVERSE_GEAR);

}  // namespace input

std::string ToString(input::InputAction e);
constexpr std::string_view ToStringView(input::InputAction e) {
  using namespace smgm::input;

  switch (e) {
    case SHIFT_1_GEAR:
      return "Gear 1";
    case SHIFT_2_GEAR:
      return "Gear 2";
    case SHIFT_3_GEAR:
      return "Gear 3";
    case SHIFT_4_GEAR:
      return "Gear 4";
    case SHIFT_5_GEAR:
      return "Gear 5";
    case SHIFT_6_GEAR:
      return "Gear 6";
    case SHIFT_7_GEAR:
      return "Gear 7";
    case SHIFT_8_GEAR:
      return "Gear 8";
    case SHIFT_HIGH_GEAR:
      return "High gear";
    case SHIFT_LOW_GEAR:
      return "Low gear";
    case SHIFT_LOW_PLUS_GEAR:
      return "Low+ gear";
    case SHIFT_LOW_MINUS_GEAR:
      return "Low- gear";
    case SHIFT_NEUTRAL:
      return "Neutral gear";
    case SHIFT_PREV_AUTO_GEAR:
      return "Previous gear";
    case SHIFT_NEXT_AUTO_GEAR:
      return "Next gear";
    case SHIFT_REVERSE_GEAR:
      return "Reverse gear";
    default:
      return "?";
  }
}

}  // namespace smgm