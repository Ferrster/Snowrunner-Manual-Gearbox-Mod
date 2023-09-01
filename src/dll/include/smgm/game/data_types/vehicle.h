#pragma once

#include <cstdint>

#include "smgm/game/data_types/combine/truck_action.h"
#include "smgm/game/data_types/combine/truck_addon_model.h"
#include "smgm/game/data_types/combine/truck_post_simulation_listener.h"

namespace smgm {

class Vehicle {
 public:
  struct GearParams {
    std::int32_t gear = -2;
    float power_coef = 1.f;

    inline bool IsValid() const noexcept { return gear >= -2; }
  };

  enum StateFlags : std::uint32_t {
    kEngineIgniting = (1 << 0),
    kEngineRunning = (1 << 1)
  };

  static constexpr float kPowerCoefLowGear = .45f;
  static constexpr float kPowerCoefLowPlusGear = 1.f;
  static constexpr float kPowerCoefLowMinusGear = .2f;

  bool IsEngineRunning() const { return q_veh_state_flags & kEngineRunning; }

  void Stall();

  void SetPowerCoef(float coef);

  GearParams GetCurrentGear() const;

  std::int32_t GetMaxGear() const;

  /**
   * @brief Switches to @p targetGear gear, if possible.
   *
   * @param target_gear gear to switch to
   * @param power_coef power coef to set after switching. Low gears have
   * different coefs.
   * @return true - if gear was switched, false otherwise
   */
  bool ShiftToGear(std::int32_t target_gear, float power_coef = 1.f);

  /// @sa ShiftToGear(std::int32_t, float)
  bool ShiftToGear(const GearParams &params) {
    return ShiftToGear(params.gear, params.power_coef);
  }

  /**
   * @brief Switches to next gear, if possible.
   *
   * @param veh vehicle to switch the gear on
   * @return true - if gear was switched, false otherwise
   */
  bool ShiftToNextGear();

  /**
   * @brief Switches to previous gear, if possible.
   *
   * @param veh vehicle to switch the gear on
   * @return true - if gear was switched, false otherwise
   */
  bool ShiftToPrevGear();

  bool ShiftToHighGear();

  bool ShiftToReverseGear();

  bool ShiftToLowGear();

  bool ShiftToLowPlusGear();

  bool ShiftToLowMinusGear();

  bool ShiftToNeutral();

  char pad_0000[88];                                  // 0x0000
  class combine::TruckAddonModel *truck_addon_model;  // 0x0058
  class combine::TruckAction *truck_action;           // 0x0060
  class combine::TruckPostSimulationListener
      *truck_post_sim_listener;     // 0x0068
  char pad_0070[164];               // 0x0070
  float stall_counter;              // 0x0114
  char pad_0118[1496];              // 0x0118
  std::uint32_t ignition;           // 0x06F0
  char pad_06F4[84];                // 0x06F4
  std::uint32_t q_veh_state_flags;  // 0x0748
  char pad_074C[244];               // 0x074C
};                                  // Size: 0x0440

}  // namespace smgm
