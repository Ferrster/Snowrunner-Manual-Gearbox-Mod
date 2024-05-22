#include "smgm/input/actions.h"

namespace smgm {
std::string ToString(input::InputAction e) {
  return std::string{ToStringView(e)};
}
}  // namespace smgm
