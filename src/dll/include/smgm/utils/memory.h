#pragma once

#include <cstdint>

#include "Windows.h"

#define SMGM_DECLARE_PTR(Offset, T, Name) \
  inline auto *Name = smgm::GetPtrToOffset<T>(Offset)

namespace smgm {
template <typename T>
inline T *GetPtrToOffset(std::int64_t offset) {
  HMODULE base = GetModuleHandleA(NULL);

  return (T *)((BYTE *)base + offset);
}
}  // namespace smgm