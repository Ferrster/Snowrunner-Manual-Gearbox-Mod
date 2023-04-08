#include "custom_functions.h"
#include "game_data/game_data.h"

namespace smgm {

Vehicle *GetCurrentVehicle() {
  auto *truckCtrl = *GameRelatedData::TruckControlPtr;

  if (truckCtrl == nullptr) {
    return nullptr;
  }

  return truckCtrl->CurVehicle;
}

bool SwitchToNextGear(Vehicle *veh) {
  if (!veh) {
    return false;
  }

  int gear = (int)veh->TruckAction->Gear_1 + 1;

  // Skip Neutral
  if (gear == 0) {
    gear = 1;
  }

  return Hook_SwitchGear(veh, std::min(gear, (int)GetMaxGear(veh)));
}

bool SwitchToPrevGear(Vehicle *veh) {
  if (!veh) {
    return false;
  }

  int gear = (int)veh->TruckAction->Gear_1 - 1;

  // Skip Neutral
  if (gear == 0) {
    gear = -1;
  }
  return Hook_SwitchGear(veh, std::max(gear, -1));
}

} // namespace smgm
