#pragma once

#include "Windows.h"
#include "detours.h"

#define SMGM_HOOK_NAME(Src) Hook_##Src
#define SMGM_CALL_HOOK(Src, ...) SMGM_HOOK_NAME(Src)(__VA_ARGS__)
#define DETOUR_ATTACH(Src) \
  DetourAttach(&(PVOID &)Src, (PVOID)SMGM_HOOK_NAME(Src))
#define DETOUR_DETACH(Src) \
  DetourDetach(&(PVOID &)Src, (PVOID)SMGM_HOOK_NAME(Src))
