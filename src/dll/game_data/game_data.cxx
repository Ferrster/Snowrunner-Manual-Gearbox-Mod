#include "game_data/game_data.h"
#include "game_data/data_types.h"
#include "utils/format_helpers.h"
#include "utils/hooks.h"
#include "utils/logging.h"

// void SMGM_HOOK_NAME(SwitchAWD)(Vehicle *veh, bool enabled) {
//   SwitchAWD(veh, enabled);
// }

bool SMGM_HOOK_NAME(ShiftGear)(Vehicle *veh, std::int32_t gear) {
  LOG_DEBUG(fmt::format("[ {} ] Switching gear: {} => {}", FormatPointer(veh),
                        veh->TruckAction->Gear_1, gear));

  return SMGM_CALL_ORIG_FN(ShiftGear, veh, gear);
}

std::int32_t SMGM_HOOK_NAME(GetMaxGear)(const Vehicle *veh) {
  return SMGM_CALL_ORIG_FN(GetMaxGear, veh);
}

void SMGM_HOOK_NAME(ShiftToAutoGear)(Vehicle *veh) {
  LOG_DEBUG(fmt::format("[ {} ] Switching to Auto gear", FormatPointer(veh)));

  SMGM_CALL_ORIG_FN(::ShiftToAutoGear, veh);
  veh->TruckAction->AutoGearSwitch = false;
  SMGM_CALL_HOOK(ShiftGear, veh, 1);
}

void SMGM_HOOK_NAME(SetPowerCoef)(Vehicle *veh, float coef) {
  SMGM_CALL_ORIG_FN(SetPowerCoef, veh, coef);
}

void SMGM_HOOK_NAME(SetCurrentVehicle)(combine_TRUCK_CONTROL *truckCtrl,
                                       Vehicle *veh) {
  SMGM_CALL_ORIG_FN(SetCurrentVehicle, truckCtrl, veh);

  LOG_DEBUG(fmt::format("Current vehicle changed to {}", FormatPointer(veh)));

  if (veh) {
    veh->TruckAction->AutoGearSwitch = false;
  }
}
