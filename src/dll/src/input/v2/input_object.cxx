#include "smgm/input/v2/input_object.h"

namespace smgm::input::v2 {
InputObject::InputObject(const InputID& id) : id_(id) {}
// BaseInputObject::Type BaseInputObject::GetType() const noexcept {
//   const auto type = GetTypeNum();

//   if (type & DIDFT_BUTTON) {
//     return Type::kButton;
//   } else if (type & DIDFT_AXIS) {
//     return Type::kAxis;
//   }

//   switch (type) {
//     case DIDFT_POV:
//       return Type::kPOV;
//     default:
//       return Type::kUnknown;
//   }
// }
}  // namespace smgm::input::v2
