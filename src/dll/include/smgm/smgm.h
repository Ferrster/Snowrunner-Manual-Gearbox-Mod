#pragma once

#include "smgm/game/data_types/combine/truck_control.h"
#include "smgm/game/data_types/vehicle.h"
#include "smgm/utils/utils.h"

SMGM_GAME_FUNCTION(0xD57C00, void, SwitchAWD, smgm::Vehicle *, bool);
SMGM_GAME_FUNCTION(0xD50460, bool, ShiftGear, smgm::Vehicle *, std::int32_t);
SMGM_GAME_FUNCTION(0xD501F0, std::int32_t, GetMaxGear, const smgm::Vehicle *);
SMGM_GAME_FUNCTION(0xD50230, void, ShiftToAutoGear, smgm::Vehicle *);
SMGM_GAME_FUNCTION(0xD4F660, void, SetPowerCoef, smgm::Vehicle *, float);
SMGM_GAME_FUNCTION(0xAD3A60, void, SetCurrentVehicle,
                   smgm::combine::TruckControl *, smgm::Vehicle *);

namespace smgm {

/**
 * @brief Singleton instance with mod's settings, constants, etc.
 *
 */
class SMGM {
public:
  static constexpr float PowerCoefLowGear = .45f;
  static constexpr float PowerCoefLowPlusGear = 1.f;
  static constexpr float PowerCoefLowMinusGear = .2f;

  inline static const auto TruckControlInstancePtr =
      GetPtrToOffset<combine::TruckControl *>(0x2EB7528);

  /// Returns singleton instance of this class
  static SMGM *GetInstance() {
    static auto *inst = new SMGM;

    return inst;
  }

  /// Returns currently operated vehicle
  static Vehicle *GetCurrentVehicle();

  inline bool IsClutchPressed() const noexcept { return m_bClutchPressed; }

  inline bool IsClutchIgnored() const noexcept { return m_bIgnoreClutch; }

  void ToggleClutch(bool state);

  const Vehicle::GearParams &GetTargetGear() const noexcept {
    return m_targetGear;
  }

  void SetTargetGear(const Vehicle::GearParams &targetGear);

private:
  bool m_bClutchPressed = false;
  bool m_bIgnoreClutch = false;

  ///  Gear to switch to after releasing clutch
  Vehicle::GearParams m_targetGear;
};
} // namespace smgm
