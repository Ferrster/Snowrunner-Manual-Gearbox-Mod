#pragma once

#include "smgm/game/data_types/combine/sound_object.h"
#include "smgm/game/data_types/string.h"

namespace smgm {
class Vehicle;

namespace combine {
class TruckControl {
public:
  char pad_0000[8];             // 0x0000
  class Vehicle *curVehicle;    // 0x0008
  char pad_0010[96];            // 0x0010
  class SoundObject *N0000094C; // 0x0070
  class SoundObject *N0000094D; // 0x0078
  class SoundObject *N0000094E; // 0x0080
  char pad_0088[8];             // 0x0088
  class String *N00000950;      // 0x0090
  char pad_0098[936];           // 0x0098
};                              // Size: 0x0440
} // namespace combine
} // namespace smgm
