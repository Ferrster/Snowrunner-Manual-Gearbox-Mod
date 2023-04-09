#pragma once

#include "game_data/data_types.h"
#include "game_data/game_data.h"

namespace smgm {

/// Returns currently operated vehicle if there is one, nullptr otherwise
Vehicle *GetCurrentVehicle();

} // namespace smgm
