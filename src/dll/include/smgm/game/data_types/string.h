#pragma once

#include <cstdint>

namespace smgm {

class String {
  union {
    char pad_0000[16];  // 0x0000
    char *str_ptr;
  };
  std::uint64_t size_;      // 0x0010
  std::uint64_t capacity_;  // 0x0018

 public:
  inline bool Enlarged() const { return capacity_ >= 16; }
  inline std::uint64_t Size() const noexcept { return size_; }
  const char *c_str() const {
    if (Enlarged()) {
      return str_ptr;
    } else {
      return static_cast<const char *>(pad_0000);
    }
  }
};  // Size: 0x0020

}  // namespace smgm
