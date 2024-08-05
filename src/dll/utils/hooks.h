#pragma once

#ifdef SMGM_USE_DETOURS
#include "Windows.h"

#include "detours.h"
#endif

#include <cstdint>

#include "utils/memory.h"

#define SMGM_HOOK_NAME(Src) Hook_##Src
#define SMGM_CALL_HOOK(Src, ...) SMGM_HOOK_NAME(Src)(__VA_ARGS__)
#define SMGM_GAME_FUNCTION(Offset, R, Name, ...)                               \
  using Fnc##Name = R(__VA_ARGS__);                                            \
  inline auto *Name = GetPtrToOffset<Fnc##Name>(Offset);                       \
  R SMGM_HOOK_NAME(Name)(__VA_ARGS__)

#ifdef SMGM_USE_DETOURS
#define SMGM_ATTACH_HOOK(Name)                                                 \
  if (DetourAttach(&(PVOID &)Name, (PVOID)SMGM_HOOK_NAME(Name)) != NO_ERROR)   \
    MessageBox(nullptr,                                                        \
               "Failed to hook function \"" #Name                              \
               "\"! Try restarting the mod or the game.",                      \
               SMGM_TITLE, MB_OK | MB_ICONERROR);
#define SMGM_DETACH_HOOK(Name)                                                 \
  DetourDetach(&(PVOID &)Name, (PVOID)SMGM_HOOK_NAME(Name))
#define SMGM_CALL_ORIG_FN(Name, ...) Name(__VA_ARGS__)
#endif

namespace smgm {
void AttachHooks();
void DetachHooks();
} // namespace smgm
