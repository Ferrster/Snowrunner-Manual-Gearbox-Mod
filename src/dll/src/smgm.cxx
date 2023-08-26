#include "smgm/smgm.h"
#include "smgm/game/data_types/combine/truck_control.h"
#include "smgm/utils/format_helpers.h"
#include "smgm/utils/logging.h"

// ===================== HOOKS =====================

void SMGM_HOOK_NAME(SwitchAWD)(smgm::Vehicle *veh, bool enabled) {
  SwitchAWD(veh, enabled);
}

bool SMGM_HOOK_NAME(ShiftGear)(smgm::Vehicle *veh, std::int32_t gear) {
  LOG_DEBUG(fmt::format("[ {} ] Switching gear: {} => {}", FormatPointer(veh),
                        veh->truckAction->gear_1, gear));

  return ShiftGear(veh, gear);
}

std::int32_t SMGM_HOOK_NAME(GetMaxGear)(const smgm::Vehicle *veh) {
  return GetMaxGear(veh);
}

void SMGM_HOOK_NAME(ShiftToAutoGear)(smgm::Vehicle *veh) {
  ShiftToAutoGear(veh);
  veh->truckAction->isAutoEnabled = false;

  SMGM_CALL_HOOK(ShiftGear, veh, 1);
}

void SMGM_HOOK_NAME(SetPowerCoef)(smgm::Vehicle *veh, float coef) {
  SetPowerCoef(veh, coef);
}

void SMGM_HOOK_NAME(SetCurrentVehicle)(smgm::combine::TruckControl *truckCtrl,
                                       smgm::Vehicle *veh) {
  SetCurrentVehicle(truckCtrl, veh);

  if (veh) {
    veh->truckAction->isAutoEnabled = false;
  }
}

namespace smgm {
Vehicle *SMGM::GetCurrentVehicle() {
  auto *truckCtrl = *TruckControlInstancePtr;

  if (truckCtrl == nullptr) {
    return nullptr;
  }

  return truckCtrl->curVehicle;
}

} // namespace smgm
