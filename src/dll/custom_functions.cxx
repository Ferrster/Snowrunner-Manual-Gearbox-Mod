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

} // namespace smgm
