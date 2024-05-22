#include "smgm/input/v2/dinput/device.h"

#include <fmt/format.h>

#include <unordered_set>

#include "smgm/input/v2/dinput/input_object.h"
#include "smgm/input/v2/dinput/input_state.h"
#include "smgm/utils/guid.h"
#include "smgm/utils/logging.h"

namespace smgm::input::v2::dinput {
DI_Device::DI_Device(LPDIRECTINPUTDEVICE8 instance,
                     const DIDEVICEINSTANCE &info, LPCDIDATAFORMAT data_format)
    : Device(API::kDirectInput, FromGUID(info.guidInstance),
             FromGUID(info.guidInstance), std::string(info.tszInstanceName)),
      instance_(instance),
      guid_(id_),
      product_name_(std::string(info.tszProductName)),
      type_(info.dwDevType),
      data_format_(data_format) {
  FindInputObjects();

  if (data_format_ == &c_dfDIKeyboard) {
    state_obj_provider_ = []() -> std::shared_ptr<InputState> {
      return std::make_shared<DI_KeyboardState>();
    };
  } else if (data_format_ == &c_dfDIJoystick2) {
    state_obj_provider_ = []() -> std::shared_ptr<InputState> {
      return std::make_shared<DI_JoystickState>();
    };
  }
}

DI_Device::~DI_Device() { Release(); }

void DI_Device::Release() {
  if (instance_) {
    instance_->Release();
    instance_ = nullptr;
    sig_device_released();
  }
}

void DI_Device::FindInputObjects() {
  if (!GetDataFormat() || !GetInstance()) return;

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
  std::unordered_set<DWORD> offsets;

  doi.dwSize = sizeof(doi);

  for (std::size_t i = 0; i < GetDataFormat()->dwNumObjs; ++i) {
    const DIOBJECTDATAFORMAT &diodf = GetDataFormat()->rgodf[i];

    if (FAILED(
            GetInstance()->GetObjectInfo(&doi, diodf.dwOfs, DIPH_BYOFFSET))) {
      continue;
    }

    const bool is_already_mapped = offsets.count(diodf.dwOfs) > 0;

    if (!is_already_mapped) {
      auto input_obj = [&]() -> std::shared_ptr<InputObject> {
        const auto input_type = DIDFT_GETTYPE(doi.dwType);

        if (input_type & DIDFT_AXIS) {
          return std::make_shared<DI_AxisInputObject>(
              InputID(InputType::kAxis, DIDFT_GETINSTANCE(doi.dwType)), diodf,
              doi);
        } else if (input_type & DIDFT_BUTTON) {
          return std::make_shared<DI_KeyInputObject>(
              InputID(InputType::kButton, DIDFT_GETINSTANCE(doi.dwType)), diodf,
              doi);
        } else if (input_type == DIDFT_POV) {
          return std::make_shared<DI_POVInputObject>(
              InputID(InputType::kPOV, DIDFT_GETINSTANCE(doi.dwType)), diodf,
              doi);
        }

        return nullptr;
      }();

      if (!input_obj) {
        continue;
      }

      input_objs_.emplace_back(input_obj);
      offsets.insert(diodf.dwOfs);

      LOG_DEBUG(
          fmt::format("name={}, type_name={}, code_str={}, diodf.dwType={}, "
                      "doi.dwType={}, "
                      "diodf.dwOfs={}, doi.dwOfs={}, already_mapped={}",
                      std::string{doi.tszName}, FromObjType(doi.dwType),
                      input_obj->GetInputID().ToString(), diodf.dwType,
                      doi.dwType, diodf.dwOfs, doi.dwOfs, is_already_mapped));
    }
  }

  LOG_DEBUG(fmt::format("Mapped {} object/s for device {}", input_objs_.size(),
                        GetDisplayName()));
}

std::shared_ptr<InputState> DI_Device::GetCurrentState() const {
  LPDIRECTINPUTDEVICE8 dev = GetInstance();
  HRESULT hr = dev->Poll();

  if (FAILED(hr)) {
    hr = dev->Acquire();

    while (hr == DIERR_INPUTLOST) hr = dev->Acquire();

    return nullptr;
  }

  auto state = state_obj_provider_();

  if (!state->ReadFromDevice(*this)) {
    LOG_WARNING(fmt::format("Failed to read DI_KeyboardState from device {}",
                            GetDisplayName()));

    return nullptr;
  }

  return state;
}
}  // namespace smgm::input::v2::dinput
