#include "smgm/input/action_bindings.h"

#include "smgm/game/data_types/combine/truck_control.h"
#include "smgm/game/data_types/vehicle.h"

namespace smgm::input {
std::function<void()> GetCallbackForAction(InputAction action) {
  static const auto ShiftGearFnc = [](std::int32_t gear) {
    return [gear] {
      if (auto *veh = combine::TruckControl::GetCurrentVehicle()) {
        veh->ShiftToGear(gear);
      }
    };
  };

  switch (action) {
    case SHIFT_NEUTRAL:
      return ShiftGearFnc(0);
    case SHIFT_1_GEAR:
      return ShiftGearFnc(1);
    case SHIFT_2_GEAR:
      return ShiftGearFnc(2);
    case SHIFT_3_GEAR:
      return ShiftGearFnc(3);
    case SHIFT_4_GEAR:
      return ShiftGearFnc(4);
    case SHIFT_5_GEAR:
      return ShiftGearFnc(5);
    case SHIFT_6_GEAR:
      return ShiftGearFnc(6);
    case SHIFT_7_GEAR:
      return ShiftGearFnc(7);
    case SHIFT_8_GEAR:
      return ShiftGearFnc(8);
    case SHIFT_HIGH_GEAR:
      return []() {
        if (auto *veh = combine::TruckControl::GetCurrentVehicle()) {
          veh->ShiftToHighGear();
        }
      };
    case SHIFT_LOW_GEAR:
      return [] {
        if (auto *veh = combine::TruckControl::GetCurrentVehicle()) {
          veh->ShiftToLowGear();
        }
      };
    case SHIFT_LOW_PLUS_GEAR:
      return [] {
        if (auto *veh = combine::TruckControl::GetCurrentVehicle()) {
          veh->ShiftToLowPlusGear();
        }
      };
    case SHIFT_LOW_MINUS_GEAR:
      return [] {
        if (auto *veh = combine::TruckControl::GetCurrentVehicle()) {
          veh->ShiftToLowMinusGear();
        }
      };
    case SHIFT_PREV_AUTO_GEAR:
      return [] {
        if (auto *veh = combine::TruckControl::GetCurrentVehicle()) {
          veh->ShiftToPrevGear();
        }
      };
    case SHIFT_NEXT_AUTO_GEAR:
      return [] {
        if (auto *veh = combine::TruckControl::GetCurrentVehicle()) {
          veh->ShiftToNextGear();
        }
      };
    case SHIFT_REVERSE_GEAR:
      return [] {
        if (auto *veh = combine::TruckControl::GetCurrentVehicle()) {
          veh->ShiftToReverseGear();
        }
      };
    default:
      return {};
  }
}
}  // namespace smgm::input
