#pragma once

#include "Windows.h"

#include <atomic>
#include <cstdint>

#include <fmt/core.h>
#include <libloaderapi.h>
#include <minwindef.h>

#include "game_data/data_types.h"
#include "game_data/game_data.h"

#define SMGM_HOOK_NAME(Src) Hook_##Src

#define SMGM_CALL_HOOK(Src, ...) SMGM_HOOK_NAME(Src)(__VA_ARGS__)

#define SMGM_DECLARE_FUNCTION_PTR(Name, Sig)                                   \
  using Fnc##Name = Sig;                                                       \
  inline Fnc##Name *Name = NULL

#define SMGM_GAME_FUNCTION(Offset, R, Name, ...)                               \
  using Fnc##Name = R(__VA_ARGS__);                                            \
  inline auto *Name = GetPtrToOffset<Fnc##Name>(Offset);                       \
  R SMGM_HOOK_NAME(Name)(__VA_ARGS__)

#define SMGM_DECLARE_PTR(Offset, T, Name)                                      \
  inline auto *Name = GetPtrToOffset<T>(Offset)

#define SMGM_DECLARE_ATOMIC_PTR(Offset, T, Name)                               \
  inline std::atomic<T *> Name = GetPtrToOffset<T>(Offset)

template <typename T> inline T *GetPtrToOffset(std::int64_t offset) {
  HMODULE base = GetModuleHandleA(NULL);

  return (T *)((BYTE *)base + offset);
}

namespace GameRelatedData {
inline const float PowerCoefLowGear = .45f;
inline const float PowerCoefLowPlusGear = 1.f;
inline const float PowerCoefLowMinusGear = .2f;

SMGM_DECLARE_PTR(0x2E96388, combine_TRUCK_CONTROL *, TruckControlPtr);
} // namespace GameRelatedData

SMGM_GAME_FUNCTION(0xD567B0, void, SwitchAWD, Vehicle *, bool);
SMGM_GAME_FUNCTION(0xD4F040, bool, ShiftGear, Vehicle *, std::int32_t);
SMGM_GAME_FUNCTION(0xD4EDD0, std::int32_t, GetMaxGear, const Vehicle *);
SMGM_GAME_FUNCTION(0xD4EE10, void, ShiftToAutoGear, Vehicle *);
SMGM_GAME_FUNCTION(0xD4E240, void, SetPowerCoef, Vehicle *, float);
SMGM_GAME_FUNCTION(0xAD3100, void, SetCurrentVehicle, combine_TRUCK_CONTROL *,
                   Vehicle *);
