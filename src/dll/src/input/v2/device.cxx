#include "smgm/input/v2/device.h"

namespace smgm::input::v2 {
std::shared_ptr<InputObject> Device::FindInputObject(
    const InputID& input_id) const {
  auto it = std::find_if(begin(GetInputObjects()), end(GetInputObjects()),
                         [&input_id](const std::shared_ptr<InputObject>& item) {
                           return item->GetInputID() == input_id;
                         });

  return it != end(GetInputObjects()) ? *it : nullptr;
}
}  // namespace smgm::input::v2
