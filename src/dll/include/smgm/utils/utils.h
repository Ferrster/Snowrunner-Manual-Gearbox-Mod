#pragma once

#include "Windows.h"

#include <cstdint>

#define SMGM_HOOK_NAME(Src) Hook_##Src

#define SMGM_CALL_HOOK(Src, ...) SMGM_HOOK_NAME(Src)(__VA_ARGS__)

#define SMGM_GAME_FUNCTION(Offset, R, Name, ...)                               \
  using Fnc##Name = R(__VA_ARGS__);                                            \
  inline auto *Name = smgm::GetPtrToOffset<Fnc##Name>(Offset);                 \
  R SMGM_HOOK_NAME(Name)(__VA_ARGS__)

#define SMGM_DECLARE_PTR(Offset, T, Name)                                      \
  inline auto *Name = smgm::GetPtrToOffset<T>(Offset)

namespace smgm {
template <typename T> inline T *GetPtrToOffset(std::int64_t offset) {
  HMODULE base = GetModuleHandleA(NULL);

  return (T *)((BYTE *)base + offset);
}
} // namespace smgm
