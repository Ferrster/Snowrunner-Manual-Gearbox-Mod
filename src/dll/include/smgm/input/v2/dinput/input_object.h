#pragma once

#include "Windows.h"
#include "dinput.h"
#include "smgm/input/v2/input_object.h"

namespace smgm::input::v2::dinput {

class DI_InputObject : virtual public InputObject {
 public:
  DI_InputObject(const InputID& id, const DIOBJECTDATAFORMAT& format,
                 const DIDEVICEOBJECTINSTANCE& doi)
      : InputObject(id), format_(format), doi_(doi) {}

  inline const DIOBJECTDATAFORMAT& GetFormat() const noexcept {
    return format_;
  }

  inline const DIDEVICEOBJECTINSTANCE& GetInstance() const noexcept {
    return doi_;
  }

 private:
  const DIOBJECTDATAFORMAT& format_;
  DIDEVICEOBJECTINSTANCE doi_;
};

class DI_KeyInputObject : public DI_InputObject, public KeyInputObject {
 public:
  DI_KeyInputObject(const InputID& id, const DIOBJECTDATAFORMAT& format,
                    const DIDEVICEOBJECTINSTANCE& doi)
      : InputObject(id), DI_InputObject(id, format, doi), KeyInputObject(id) {}
};
class DI_AxisInputObject : public DI_InputObject, public AxisInputObject {
 public:
  DI_AxisInputObject(const InputID& id, const DIOBJECTDATAFORMAT& format,
                     const DIDEVICEOBJECTINSTANCE& doi)
      : InputObject(id), DI_InputObject(id, format, doi), AxisInputObject(id) {}
};
class DI_POVInputObject : public DI_InputObject, public POVInputObject {
 public:
  DI_POVInputObject(const InputID& id, const DIOBJECTDATAFORMAT& format,
                    const DIDEVICEOBJECTINSTANCE& doi)
      : InputObject(id), DI_InputObject(id, format, doi), POVInputObject(id) {}
};
}  // namespace smgm::input::v2::dinput
