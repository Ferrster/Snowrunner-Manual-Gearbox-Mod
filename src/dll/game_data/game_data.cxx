#include "game_data/game_data.h"

#include "config/ini_config.hpp"
#include "game_data/data_types.h"
#include "utils/format_helpers.h"
#include "utils/hooks.h"
#include "utils/logging.h"

#include <algorithm>

// void SMGM_HOOK_NAME(SwitchAWD)(Vehicle *veh, bool enabled) {
//   SwitchAWD(veh, enabled);
// }

extern smgm::IniConfig g_IniConfig;

bool SMGM_HOOK_NAME(ShiftGear)(Vehicle *veh, std::int32_t gear) {
  LOG_DEBUG(fmt::format("[ {} ] Switching gear: {} => {}", FormatPointer(veh),
                        veh->TruckAction->Gear_1, gear));

  return SMGM_CALL_ORIG_FN(ShiftGear, veh, gear);
}

std::int32_t SMGM_HOOK_NAME(GetMaxGear)(const Vehicle *veh) {
  return SMGM_CALL_ORIG_FN(GetMaxGear, veh);
}

void SMGM_HOOK_NAME(ShiftToAutoGear)(Vehicle *veh) {
  if (g_IniConfig.Get<bool>("SMGM.DisableGameShifting")) {
    return;
  }

  SMGM_CALL_ORIG_FN(ShiftToAutoGear, veh);
  veh->TruckAction->IsInAutoMode = false;
}

bool SMGM_HOOK_NAME(ShiftToReverse)(Vehicle *veh) {
  if (g_IniConfig.Get<bool>("SMGM.DisableGameShifting")) {
    return false;
  }

  return SMGM_CALL_ORIG_FN(ShiftToReverse, veh);
}

bool SMGM_HOOK_NAME(ShiftToNeutral)(Vehicle *veh) {
  if (g_IniConfig.Get<bool>("SMGM.DisableGameShifting")) {
    return false;
  }

  return SMGM_CALL_ORIG_FN(::ShiftToNeutral, veh);
}

bool SMGM_HOOK_NAME(ShiftToHigh)(Vehicle *veh) {
  if (g_IniConfig.Get<bool>("SMGM.DisableGameShifting")) {
    return false;
  }

  return SMGM_CALL_ORIG_FN(::ShiftToHigh, veh);
}

bool SMGM_HOOK_NAME(DisableAutoAndShift)(Vehicle *veh, std::int32_t gear) {
  if (g_IniConfig.Get<bool>("SMGM.DisableGameShifting")) {
    return false;
  }

  return SMGM_CALL_ORIG_FN(DisableAutoAndShift, veh, gear);
}

void SMGM_HOOK_NAME(SetPowerCoef)(Vehicle *veh, float coef) {
  if (g_IniConfig.Get<bool>("SMGM.DisableGameShifting")) {
    SMGM_CALL_ORIG_FN(SetPowerCoef, veh, GameRelatedData::PowerCoefLowPlusGear);
    return;
  }

  SMGM_CALL_ORIG_FN(SetPowerCoef, veh, coef);
}

void SMGM_HOOK_NAME(SetCurrentVehicle)(combine_TRUCK_CONTROL *truckCtrl,
                                       Vehicle *veh) {
  SMGM_CALL_ORIG_FN(SetCurrentVehicle, truckCtrl, veh);

  LOG_DEBUG(fmt::format("Current vehicle changed to {}", FormatPointer(veh)));

  if (veh) {
    veh->TruckAction->IsInAutoMode = false;
  }
}
