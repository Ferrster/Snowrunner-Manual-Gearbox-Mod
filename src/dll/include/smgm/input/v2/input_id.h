#pragma once

#include <string>

namespace smgm::input::v2 {
enum class InputType { kUnknown = -1, kKey, kButton, kAxis, kPOV };

class InputID {
 public:
  InputID(InputType type, unsigned short instance)
      : type(type), instance(instance) {}
  InputID(const std::string& str_id) {}
  std::string ToString() const;

  InputType type = InputType::kUnknown;
  unsigned short instance = 0;
};
}  // namespace smgm::input::v2
