#pragma once

#include <atomic>

#include "smgm/game/data_types/combine/truck_control.h"
#include "smgm/game/data_types/vehicle.h"
#include "smgm/input/dinput_reader.h"
#include "smgm/input/v2/device_manager.h"
#include "smgm/json/config.h"
#include "smgm/ui/ui.h"
#include "smgm/utils/hooks.h"
#include "smgm/utils/input_reader.h"

SMGM_GAME_FUNCTION(0xD50510, bool, ShiftGear, smgm::Vehicle *, std::int32_t);
SMGM_GAME_FUNCTION(0xD50270, std::int32_t, GetMaxGear, const smgm::Vehicle *);
SMGM_GAME_FUNCTION(0xD502C0, void, ShiftToAutoGear, smgm::Vehicle *);
SMGM_GAME_FUNCTION(0xD4F6B0, void, SetPowerCoef, smgm::Vehicle *, float);
SMGM_GAME_FUNCTION(0xAC9C50, void, SetCurrentVehicle,
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
extern input::DirectInputReader dinput_reader;
extern json::Config config;
extern input::v2::DeviceManager device_manager;
extern boost::signals2::signal<void()> sig_tick;
}  // namespace smgm
