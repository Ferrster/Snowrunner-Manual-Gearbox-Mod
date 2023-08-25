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

void SMGM::ToggleClutch(bool state) {
  if (m_bClutchPressed == state)
    return;

  Vehicle *veh = GetCurrentVehicle();

  if (!veh)
    return;

  m_bClutchPressed = state;
  if (state) {
    m_targetGear = veh->GetCurrentGear();
    m_bIgnoreClutch = true;
    veh->ShiftToNeutral();
    m_bIgnoreClutch = false;
  } else {
    m_bIgnoreClutch = true;
    veh->ShiftToGear(GetTargetGear());
    m_bIgnoreClutch = false;
  }

  LOG_DEBUG(
      fmt::format("Clutch was {}", m_bClutchPressed ? "pressed" : "released"));
}

void SMGM::SetTargetGear(const Vehicle::GearParams &targetGear) {
  m_targetGear = targetGear;
}
} // namespace smgm
