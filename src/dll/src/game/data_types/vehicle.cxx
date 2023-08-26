#include "smgm/game/data_types/vehicle.h"
#include "smgm/smgm.h"
#include "smgm/utils/logging.h"

#include <algorithm>

namespace smgm {

void Vehicle::Stall() {
  LOG_DEBUG("Force-stalling the engine...");

  stallCounter = 5.f;
}

void Vehicle::SetPowerCoef(float coef) { truckAction->powerCoef = coef; }

std::int32_t Vehicle::GetMaxGear() const {
  return SMGM_CALL_HOOK(GetMaxGear, this);
}

Vehicle::GearParams Vehicle::GetCurrentGear() const {
  GearParams params;

  params.gear = truckAction->gear_2;
  params.powerCoef = truckAction->powerCoef;

  return params;
}

bool Vehicle::ShiftToGear(std::int32_t targetGear, float powerCoef) {
  std::int32_t gear = std::clamp(targetGear, -1, GetMaxGear() + 1);

  bool bSwitched = SMGM_CALL_HOOK(ShiftGear, this, gear);
  if (bSwitched) {
    SetPowerCoef(powerCoef);
  }

  return bSwitched;
}

bool Vehicle::ShiftToNextGear() {
  std::int32_t gear = truckAction->gear_1 + 1;

  // Skip Neutral
  if (gear == 0) {
    gear = 1;
  }

  return ShiftToGear(std::min(gear, GetMaxGear()));
}

bool Vehicle::ShiftToPrevGear() {
  auto gear = static_cast<std::int32_t>(truckAction->gear_1) - 1;

  // Skip Neutral
  if (gear == 0) {
    gear = -1;
  }

  return ShiftToGear(std::max(gear, -1));
}

bool Vehicle::ShiftToHighGear() { return ShiftToGear(GetMaxGear() + 1); }

bool Vehicle::ShiftToReverseGear() { return ShiftToGear(-1); }

bool Vehicle::ShiftToLowGear() {
  return ShiftToGear(1, SMGM::PowerCoefLowGear);
}

bool Vehicle::ShiftToLowPlusGear() {
  return ShiftToGear(1, SMGM::PowerCoefLowPlusGear);
}

bool Vehicle::ShiftToLowMinusGear() {
  return ShiftToGear(1, SMGM::PowerCoefLowMinusGear);
}

bool Vehicle::ShiftToNeutral() { return ShiftToGear(0); }

} // namespace smgm
