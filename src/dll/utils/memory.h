#pragma once

#include "Windows.h"

#include <cstdint>

#define SMGM_DECLARE_PTR(Offset, T, Name)                                      \
  inline auto *Name = GetPtrToOffset<T>(Offset)

template <typename T> inline T *GetPtrToOffset(std::int64_t offset) {
  HMODULE base = GetModuleHandleA(NULL);

  return (T *)((BYTE *)base + offset);
}
