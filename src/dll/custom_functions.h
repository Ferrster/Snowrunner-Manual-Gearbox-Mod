#pragma once

#include "game_data/data_types.h"
#include "game_data/game_data.h"

namespace smgm {

/// Returns currently operated vehicle if there is one, nullptr otherwise
Vehicle *GetCurrentVehicle();

/**
 * @brief Switches to next gear, if possible.
 *
 * @param veh vehicle to switch the gear on
 * @return true - if gear was switches, false otherwise
 */
bool SwitchToNextGear(Vehicle *veh);

/**
 * @brief Switches to previous gear, if possible.
 *
 * @param veh vehicle to switch the gear on
 * @return true - if gear was switched, false otherwise
 */
bool SwitchToPrevGear(Vehicle *veh);

} // namespace smgm
