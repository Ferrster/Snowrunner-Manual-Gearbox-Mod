#pragma once

#include <cstdint>
#include <string>

class String {
public:
  union {
    char pad_0000[16]; // 0x0000
    char *Ptr;
  };
  uint64_t Size;     // 0x0010
  uint64_t Capacity; // 0x0018

  inline bool Enlarged() const { return Capacity >= 16; }
  const char *c_str() const {
    if (Enlarged()) {
      return Ptr;
    } else {
      return static_cast<const char *>(pad_0000);
    }
  }
}; // Size: 0x0020

class combine_combineTruckAction {
public:
  char pad_0000[48];          // 0x0000
  class Vehicle *Veh;         // 0x0030
  float N00000056;            // 0x0038
  bool AutoGearSwitch;        // 0x003C
  char pad_003D[3];           // 0x003D
  float WheelTurn;            // 0x0040
  float Accel;                // 0x0044
  bool Handbrake;             // 0x0048
  bool AWD;                   // 0x0049
  bool Diff;                  // 0x004A
  char pad_004B[21];          // 0x004B
  class Unknown_1 *N0000005B; // 0x0060
  char pad_0068[8];           // 0x 0068
  uint32_t Gear_1;            // 0x0070
  uint32_t Gear_2;            // 0x0074
  char pad_0078[56];          // 0x0078
  float N00000065;            // 0x00B0
  float N00000319;            // 0x00B4
  float N00000066;            // 0x00B8
  float N00000325;            // 0x00BC
  char pad_00C0[24];          // 0x00C0
  float N0000006A;            // 0x00D8
  float SwitchThreshold;      // 0x00DC
  uint32_t NextGear;          // 0x00E0
  char pad_00E4[860];         // 0x00E4
};                            // Size: 0x0440

class Vehicle {
public:
  char pad_0000[88];                                // 0x0000
  class combine_TRUCK_ADDON_MODEL *TruckAddonModel; // 0x0058
  class combine_combineTruckAction *TruckAction;    // 0x0060
  class combine_combineTruckPostSimulationListener
      *TruckPostSimulationListener; // 0x0068
  char pad_0070[976];               // 0x0070
};                                  // Size: 0x0440

class combine_TRUCK_ADDON_MODEL {
public:
  char pad_0000[88];             // 0x0000
  class combine_SCENE *Scene;    // 0x0058
  class combine_XMESH *XMesh;    // 0x0060
  char pad_0068[144];            // 0x0068
  class N0000059F *N00000462;    // 0x00F8
  char pad_0100[16];             // 0x0100
  class N00000579 *N00000465;    // 0x0110
  char pad_0118[200];            // 0x0118
  class hkpRigidBody *N0000047F; // 0x01E0
  char pad_01E8[544];            // 0x01E8
};                               // Size: 0x0408

class combine_XMESH {
public:
  char pad_0000[8]; // 0x0000
};                  // Size: 0x0008

class combine_SCENE {
public:
  char pad_0000[8]; // 0x0000
};                  // Size: 0x0008

class N00000579 {
public:
  class combine_TRUCK_WHEEL_MODEL *TruckWheelModel1; // 0x0000
  class N00000696 *N000005CA;                        // 0x0008
  void *N000005CB;                                   // 0x0010
  class N000006BB *N000005CC;                        // 0x0018
  char pad_0020[1000];                               // 0x0020
};                                                   // Size: 0x0408

class N0000059F {
public:
  char pad_0000[8]; // 0x0000
};                  // Size: 0x0008

class combine_combineTruckPostSimulationListener {
public:
  char pad_0000[8]; // 0x0000
};                  // Size: 0x0008

class combine_TRUCK_WHEEL_MODEL {
public:
  char pad_0000[8]; // 0x0000
};                  // Size: 0x0008

class N00000696 {
public:
  char pad_0000[8]; // 0x0000
};                  // Size: 0x0008

class N000006BB {
public:
  char pad_0000[8]; // 0x0000
};                  // Size: 0x0008

class N000006C6 {
public:
  class combine_combineDriveCameraAction *N000006C7; // 0x0000
  class combine_combineSubstanceWheel *N000006C8;    // 0x0008
  class combine_combineSubstanceWheel *N000006C9;    // 0x0010
  class combine_combineSubstanceWheel *N000006CA;    // 0x0018
  class combine_combineSubstanceWheel *N000006CB;    // 0x0020
  class combine_combineSubstanceWheel *N000006CC;    // 0x0028
  class combine_combineSubstanceWheel *N000006CD;    // 0x0030
  class combine_combineSubstanceWheel *N000006CE;    // 0x0038
  class combine_combineSubstanceWheel *N000006CF;    // 0x0040
  char pad_0048[1016];                               // 0x0048
};                                                   // Size: 0x0440

class combine_combineDriveCameraAction {
public:
  char pad_0000[8]; // 0x0000
};                  // Size: 0x0008

class combine_combineSubstanceWheel {
public:
  char pad_0000[8]; // 0x0000
};                  // Size: 0x0008

class combine_TRUCK_CONTROL {
public:
  char pad_0000[8];                      // 0x0000
  class Vehicle *CurVehicle;             // 0x0008
  char pad_0010[96];                     // 0x0010
  class combine_SOUND_OBJECT *N0000094C; // 0x0070
  class combine_SOUND_OBJECT *N0000094D; // 0x0078
  class combine_SOUND_OBJECT *N0000094E; // 0x0080
  char pad_0088[8];                      // 0x0088
  class String *N00000950;               // 0x0090
  char pad_0098[936];                    // 0x0098
};                                       // Size: 0x0440

class combine_SOUND_OBJECT {
public:
  char pad_0000[8]; // 0x0000
};                  // Size: 0x0008

class Unknown_1 {
public:
  char pad_0000[1032]; // 0x0000
};                     // Size: 0x0408

class hkpRigidBody {
public:
  char pad_0000[1032]; // 0x0000
};                     // Size: 0x0408
