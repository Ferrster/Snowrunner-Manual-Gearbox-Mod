#pragma once

#include <cstdint>

namespace smgm {
class Vehicle;
class Unknown_1;

namespace combine {
class TruckAction {
 public:
  char pad_0000[48];          // 0x0000
  Vehicle *veh;               // 0x0030
  float power_coef;           // 0x0038
  bool is_auto_enabled;       // 0x003C
  char pad_003D[3];           // 0x003D
  float wheel_turn;           // 0x0040
  float accel;                // 0x0044
  bool is_handbrake_engaged;  // 0x0048
  bool is_awd_engaged;        // 0x0049
  bool is_diff_engaged;       // 0x004A
  char pad_004B[21];          // 0x004B
  Unknown_1 *N0000005B;       // 0x0060
  char pad_0068[8];           // 0x 0068
  std::int32_t gear_1;        // 0x0070
  std::int32_t gear_2;        // 0x0074
  char pad_0078[56];          // 0x0078
  float N00000065;            // 0x00B0
  float N00000319;            // 0x00B4
  float N00000066;            // 0x00B8
  float N00000325;            // 0x00BC
  char pad_00C0[24];          // 0x00C0
  float N0000006A;            // 0x00D8
  float switch_threshold;     // 0x00DC
  std::int32_t next_gear;     // 0x00E0
  char pad_00E4[860];         // 0x00E4
};                            // Size: 0x0440
}  // namespace combine
}  // namespace smgm
