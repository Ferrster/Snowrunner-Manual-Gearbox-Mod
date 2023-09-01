#pragma once

#include <atomic>
#include <vector>

#include "boost/signals2.hpp"
#include "d3d11.h"
#include "dxgi.h"
#include "smgm/game/data_types/combine/truck_control.h"
#include "smgm/game/data_types/vehicle.h"
#include "smgm/ui/ui.h"
#include "smgm/utils/input_reader.h"
#include "smgm/utils/utils.h"

SMGM_GAME_FUNCTION(0xD57C00, void, SwitchAWD, smgm::Vehicle *, bool);
SMGM_GAME_FUNCTION(0xD50460, bool, ShiftGear, smgm::Vehicle *, std::int32_t);
SMGM_GAME_FUNCTION(0xD501F0, std::int32_t, GetMaxGear, const smgm::Vehicle *);
SMGM_GAME_FUNCTION(0xD50230, void, ShiftToAutoGear, smgm::Vehicle *);
SMGM_GAME_FUNCTION(0xD4F660, void, SetPowerCoef, smgm::Vehicle *, float);
SMGM_GAME_FUNCTION(0xAD3A60, void, SetCurrentVehicle,
                   smgm::combine::TruckControl *, smgm::Vehicle *);

#ifndef SMGM_TITLE
#define SMGM_TITLE "SMGM"
#endif

namespace smgm {
bool Init(HINSTANCE hinst);
void Destroy(HINSTANCE hinst);
void Exit();
void ExitWhenReady();

inline static const std::string kModTitle = SMGM_TITLE;
extern HWND game_window;
extern std::atomic_bool is_exiting;
extern Ui ui;
extern InputReader input_reader;
}  // namespace smgm
