#pragma once

#include "smgm/game/data_types/combine/scene.h"
#include "smgm/game/data_types/combine/xmesh.h"

namespace smgm {
class N0000059F;
class N00000579;

namespace combine {
class TruckAddonModel {
 public:
  char pad_0000[88];              // 0x0000
  class Scene *scene;             // 0x0058
  class XMesh *xmesh;             // 0x0060
  char pad_0068[144];             // 0x0068
  class N0000059F *N00000462;     // 0x00F8
  char pad_0100[16];              // 0x0100
  class N00000579 *N00000465;     // 0x0110
  char pad_0118[200];             // 0x0118
  class hkpRigidBody *N0000047F;  // 0x01E0
  char pad_01E8[544];             // 0x01E8
};                                // Size: 0x0408
}  // namespace combine
}  // namespace smgm
