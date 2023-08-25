#pragma once

#include <cstdint>

namespace smgm {

class String {
  union {
    char pad_0000[16]; // 0x0000
    char *m_strPtr;
  };
  std::uint64_t m_size;     // 0x0010
  std::uint64_t m_capacity; // 0x0018

public:
  inline bool Enlarged() const { return m_capacity >= 16; }
  inline std::uint64_t Size() const noexcept { return m_size; }
  const char *c_str() const {
    if (Enlarged()) {
      return m_strPtr;
    } else {
      return static_cast<const char *>(pad_0000);
    }
  }
}; // Size: 0x0020

} // namespace smgm
