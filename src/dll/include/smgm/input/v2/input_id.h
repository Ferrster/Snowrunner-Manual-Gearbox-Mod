#pragma once

#include <boost/functional/hash.hpp>
#include <string>

namespace smgm::input::v2 {
enum class InputType { kUnknown = -1, kKey, kButton, kAxis, kPOV };

class InputID {
 public:
  InputID(InputType type, unsigned short instance)
      : type(type), instance(instance) {}
  InputID(const std::string& str_id) {}
  std::string ToString() const;
  inline bool operator==(const InputID& rhs) const noexcept {
    return type == rhs.type && instance == rhs.instance;
  }

  InputType type = InputType::kUnknown;
  unsigned short instance = 0;
};
}  // namespace smgm::input::v2

template <>
struct std::hash<smgm::input::v2::InputID> {
  std::size_t operator()(const smgm::input::v2::InputID& id) const {
    std::size_t seed = std::hash<smgm::input::v2::InputType>{}(id.type);

    boost::hash_combine(seed, id.instance);

    return seed;
  }
};
