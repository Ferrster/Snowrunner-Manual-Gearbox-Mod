#include "smgm/game/data_types/vehicle.h"

#include <algorithm>

#include "smgm/smgm.h"
#include "smgm/utils/logging.h"

namespace smgm {

void Vehicle::Stall() {
  LOG_DEBUG("Force-stalling the engine...");

  stall_counter = 5.f;
}

void Vehicle::SetPowerCoef(float coef) { truck_action->power_coef = coef; }

std::int32_t Vehicle::GetMaxGear() const {
  return SMGM_CALL_HOOK(GetMaxGear, this);
}

Vehicle::GearParams Vehicle::GetCurrentGear() const {
  GearParams params;

  params.gear = truck_action->gear_2;
  params.power_coef = truck_action->power_coef;

  return params;
}

bool Vehicle::ShiftToGear(std::int32_t target_gear, float power_coef) {
  std::int32_t gear = std::clamp(target_gear, -1, GetMaxGear() + 1);

  bool is_switched = SMGM_CALL_HOOK(ShiftGear, this, gear);
  if (is_switched) {
    SetPowerCoef(power_coef);
  }

  return is_switched;
}

bool Vehicle::ShiftToNextGear() {
  std::int32_t gear = truck_action->gear_1 + 1;

  // Skip Neutral
  if (gear == 0) {
    gear = 1;
  }

  return ShiftToGear(std::min(gear, GetMaxGear()));
}

bool Vehicle::ShiftToPrevGear() {
  auto gear = static_cast<std::int32_t>(truck_action->gear_1) - 1;

  // Skip Neutral
  if (gear == 0) {
    gear = -1;
  }

  return ShiftToGear(std::max(gear, -1));
}

bool Vehicle::ShiftToHighGear() { return ShiftToGear(GetMaxGear() + 1); }

bool Vehicle::ShiftToReverseGear() { return ShiftToGear(-1); }

bool Vehicle::ShiftToLowGear() { return ShiftToGear(1, kPowerCoefLowGear); }

bool Vehicle::ShiftToLowPlusGear() {
  return ShiftToGear(1, kPowerCoefLowPlusGear);
}

bool Vehicle::ShiftToLowMinusGear() {
  return ShiftToGear(1, kPowerCoefLowMinusGear);
}

bool Vehicle::ShiftToNeutral() { return ShiftToGear(0); }

}  // namespace smgm
