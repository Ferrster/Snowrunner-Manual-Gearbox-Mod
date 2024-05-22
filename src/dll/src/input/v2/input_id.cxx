#include "smgm/input/v2/input_id.h"

namespace smgm::input::v2 {
std::string InputID::ToString() const {
  auto str_type = [&]() -> std::string {
    switch (type) {
      case InputType::kKey:
        return "key";
      case InputType::kButton:
        return "button";
      case InputType::kAxis:
        return "axis";
      case InputType::kPOV:
        return "pov";
      default:
        return "unk";
    }
  }();

  str_type.append(std::to_string(instance));

  return str_type;
}
}  // namespace smgm::input::v2
