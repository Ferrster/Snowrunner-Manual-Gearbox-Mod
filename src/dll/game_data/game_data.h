#pragma once

#include "Windows.h"

#include <atomic>
#include <cstdint>

#include <fmt/core.h>
#include <libloaderapi.h>
#include <minwindef.h>

#include "game_data/data_types.h"

#define DETOUR_HOOK_NAME(Src) Hook_##Src

#define SNOWRUNNER_DECLARE_FUNCTION_PTR(Name, Sig)                             \
  using Fnc##Name = Sig;                                                       \
  inline Fnc##Name *Name = NULL

#define SNOWRUNNER_GAME_FUNCTION(Offset, R, Name, ...)                         \
  using Fnc##Name = R(__VA_ARGS__);                                            \
  inline auto *Name = GetPtrToOffset<Fnc##Name>(Offset);                       \
  R DETOUR_HOOK_NAME(Name)(__VA_ARGS__)

#define SNOWRUNNER_DECLARE_PTR(T, Name, Offset)                                \
  inline auto *Name = GetPtrToOffset<T>(Offset)

#define SNOWRUNNER_DECLARE_ATOMIC_PTR(Offset, T, Name)                         \
  inline std::atomic<T *> Name = GetPtrToOffset<T>(Offset)

template <typename T> inline T *GetPtrToOffset(std::int64_t offset) {
  HMODULE base = GetModuleHandleA(NULL);

  return (T *)((BYTE *)base + offset);
}

namespace GameRelatedData {
SNOWRUNNER_DECLARE_ATOMIC_PTR(0x2e7c508, combine_TRUCK_CONTROL *,
                              TruckControlPtr);
} // namespace GameRelatedData

SNOWRUNNER_GAME_FUNCTION(0xd54b90, void, SwitchAWD, Vehicle *, bool);
SNOWRUNNER_GAME_FUNCTION(0xd4d420, bool, SwitchGear, Vehicle *, std::uint32_t);
SNOWRUNNER_GAME_FUNCTION(0xd4d1b0, std::uint32_t, GetMaxGear, Vehicle *);
SNOWRUNNER_GAME_FUNCTION(0xd4d1f0, void, SwitchToAutoGear, Vehicle *);
