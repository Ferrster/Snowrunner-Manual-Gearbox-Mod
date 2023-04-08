#include "game_data/game_data.h"
#include "utils/format_helpers.h"
#include "utils/logging.h"

void DETOUR_HOOK_NAME(SwitchAWD)(Vehicle *veh, bool enabled) {
  SwitchAWD(veh, enabled);
}

bool DETOUR_HOOK_NAME(SwitchGear)(Vehicle *veh, std::uint32_t gear) {
  LOG_DEBUG(fmt::format("[ {} ] Switching gear: {} => {}", FormatPointer(veh),
                        veh->TruckAction->Gear_1, gear));
  return SwitchGear(veh, gear);
}

std::uint32_t DETOUR_HOOK_NAME(GetMaxGear)(Vehicle *veh) {
  return GetMaxGear(veh);
}

void DETOUR_HOOK_NAME(SwitchToAutoGear)(Vehicle *veh) {
  LOG_DEBUG(fmt::format("[ {} ] Switching to Auto gear", FormatPointer(veh)));
  SwitchToAutoGear(veh);
  veh->TruckAction->AutoGearSwitch = false;
  Hook_SwitchGear(veh, 1);
}
