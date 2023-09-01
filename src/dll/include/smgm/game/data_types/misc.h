#pragma once

#include "smgm/game/data_types/combine/drive_camera_action.h"
#include "smgm/game/data_types/combine/substance_wheel.h"
#include "smgm/game/data_types/combine/truck_wheel_model.h"

namespace smgm {
class N00000579 {
 public:
  class combine::TruckWheelModel *truck_wheel_model_1;  // 0x0000
  class N00000696 *N000005CA;                           // 0x0008
  void *N000005CB;                                      // 0x0010
  class N000006BB *N000005CC;                           // 0x0018
  char pad_0020[1000];                                  // 0x0020
};                                                      // Size: 0x0408

class N0000059F {
 public:
  char pad_0000[8];  // 0x0000
};                   // Size: 0x0008

class N00000696 {
 public:
  char pad_0000[8];  // 0x0000
};                   // Size: 0x0008

class N000006BB {
 public:
  char pad_0000[8];  // 0x0000
};                   // Size: 0x0008

class N000006C6 {
 public:
  class combine::DriveCameraAction *N000006C7;  // 0x0000
  class combine::SubstanceWheel *N000006C8;     // 0x0008
  class combine::SubstanceWheel *N000006C9;     // 0x0010
  class combine::SubstanceWheel *N000006CA;     // 0x0018
  class combine::SubstanceWheel *N000006CB;     // 0x0020
  class combine::SubstanceWheel *N000006CC;     // 0x0028
  class combine::SubstanceWheel *N000006CD;     // 0x0030
  class combine::SubstanceWheel *N000006CE;     // 0x0038
  class combine::SubstanceWheel *N000006CF;     // 0x0040
  char pad_0048[1016];                          // 0x0048
};                                              // Size: 0x0440

class Unknown_1 {
 public:
  char pad_0000[1032];  // 0x0000
};                      // Size: 0x0408
}  // namespace smgm
