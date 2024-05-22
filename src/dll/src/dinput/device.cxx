#include "smgm/dinput/device.h"

#include <unordered_set>

#include "fmt/format.h"
#include "smgm/utils/guid.h"
#include "smgm/utils/logging.h"

namespace smgm::dinput {
Device::Device(LPDIRECTINPUTDEVICE8 instance, const DIDEVICEINSTANCE &info,
               LPCDIDATAFORMAT data_format)
    : instance_(instance),
      guid_(FromGUID(info.guidInstance)),
      name_(std::string(info.tszInstanceName)),
      product_name_(std::string(info.tszProductName)),
      type_(info.dwDevType),
      data_format_(data_format) {
  FindAvailableObjects();
}

Device::~Device() { Release(); }

void Device::InitFrom(LPDIRECTINPUTDEVICE8 instance,
                      const DIDEVICEINSTANCE &info,
                      LPCDIDATAFORMAT data_format) {
  instance_ = instance;
  guid_ = FromGUID(info.guidInstance);
  name_ = std::string(info.tszInstanceName);
  product_name_ = std::string(info.tszProductName);
  type_ = info.dwDevType;
  data_format_ = data_format;

  FindAvailableObjects();
}

void Device::FindAvailableObjects() {
  if (!data_format_ || !GetInstance()) return;

  static const auto FromObjType = [](DWORD dwType) -> std::string {
    const DWORD type = DIDFT_GETTYPE(dwType);

    if (type & DIDFT_BUTTON) {
      return fmt::format("Button ({})",
                         type & DIDFT_PSHBUTTON ? "push" : "toggle");
    } else if (type & DIDFT_AXIS) {
      return fmt::format("Axis ({})", type & DIDFT_ABSAXIS ? "abs" : "rel");
    } else if (type == DIDFT_POV) {
      return "POV";
    } else {
      return "???";
    }
  };

  DIDEVICEOBJECTINSTANCE doi;

  doi.dwSize = sizeof(doi);

  std::unordered_set<DWORD> offsets;

  for (std::size_t i = 0; i < data_format_->dwNumObjs; ++i) {
    const DIOBJECTDATAFORMAT &diodf = data_format_->rgodf[i];

    if (auto hr =
            GetInstance()->GetObjectInfo(&doi, diodf.dwOfs, DIPH_BYOFFSET);
        hr == DI_OK) {
      const bool is_already_mapped = offsets.count(diodf.dwOfs) > 0;

      if (!is_already_mapped) {
        AvailableObject obj{diodf, std::move(doi)};
        std::string code_str = obj.ToCodeStr();
        const auto [it, _] =
            objs_.insert({obj.instance.dwType, std::move(obj)});

        offsets.insert(diodf.dwOfs);

        LOG_DEBUG(
            fmt::format("name={}, type_name={}, code_str={}, diodf.dwType={}, "
                        "doi.dwType={}, "
                        "diodf.dwOfs={}, doi.dwOfs={}, already_mapped={}",
                        std::string{doi.tszName}, FromObjType(doi.dwType),
                        code_str, diodf.dwType, doi.dwType, diodf.dwOfs,
                        doi.dwOfs, is_already_mapped));
      }
    }
  }

  // if (FAILED(
  //         GetInstance()->EnumObjects(&Device_EnumObjects, this, DIDFT_ALL)))
  //         {
  //   LOG_ERROR(fmt::format("Failed to enumerate objects for device {}",
  //   name_));

  //   return;
  // }

  LOG_DEBUG(
      fmt::format("Mapped {} object/s for device {}", objs_.size(), name_));
}

// input::POVState ReadPOVState(const DeviceState &state,
//                              const AvailableObject &obj) {
//   std::visit(
//       [&](auto &&state) -> input::POVState {

//       },
//       state);
// }

}  // namespace smgm::dinput
