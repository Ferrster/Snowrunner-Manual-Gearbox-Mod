#pragma once

#include <vector>

#include "boost/algorithm/string/join.hpp"
#include "boost/range/adaptor/transformed.hpp"
#include "smgm/input/key.h"

namespace smgm::input {

class Keybind {
 public:
  Keybind() = default;
  Keybind(const std::string& str_keybind);
  Keybind(const std::vector<Key>& keys) { SetKeys(keys); }

  void SetKeys(const std::vector<Key>& keys) {
    keys_ = keys;
    Sort(keys_);
  }
  bool AddKey(Key key);
  void RemoveKey(Key key);
  inline bool IsValid() const noexcept { return !keys_.empty(); }
  inline void Clear() { keys_.clear(); }
  std::string ToString(
      KeyNameFormat format = KeyNameFormat::kPreferNames) const {
    return ToString(keys_, format);
  }
  inline const std::vector<Key>& GetKeys() const noexcept { return keys_; }
  static std::string ToString(
      const std::vector<Key>& keys,
      KeyNameFormat format = KeyNameFormat::kPreferNames) {
    return boost::algorithm::join(
        keys | boost::adaptors::transformed(
                   [format](input::Key key) -> std::string {
                     return input::GetKeyName(key, format);
                   }),
        " + ");
  }
  static void Sort(std::vector<Key>& keys);

 private:
  std::vector<Key> keys_;
};

}  // namespace smgm::input
