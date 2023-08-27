#pragma once

#include "d3d11.h"
#include "dxgi.h"

#include "smgm/game/data_types/combine/truck_control.h"
#include "smgm/game/data_types/vehicle.h"
#include "smgm/ui/ui.h"
#include "smgm/utils/input_reader.h"
#include "smgm/utils/utils.h"

#include <atomic>

SMGM_GAME_FUNCTION(0xD57C00, void, SwitchAWD, smgm::Vehicle *, bool);
SMGM_GAME_FUNCTION(0xD50460, bool, ShiftGear, smgm::Vehicle *, std::int32_t);
SMGM_GAME_FUNCTION(0xD501F0, std::int32_t, GetMaxGear, const smgm::Vehicle *);
SMGM_GAME_FUNCTION(0xD50230, void, ShiftToAutoGear, smgm::Vehicle *);
SMGM_GAME_FUNCTION(0xD4F660, void, SetPowerCoef, smgm::Vehicle *, float);
SMGM_GAME_FUNCTION(0xAD3A60, void, SetCurrentVehicle,
                   smgm::combine::TruckControl *, smgm::Vehicle *);

namespace smgm {

using FncD3D11Present = HRESULT(__stdcall *)(IDXGISwapChain *pSwapChain,
                                             UINT SyncInterval, UINT Flags);

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

  std::atomic_bool isExiting = false;
  smgm::Ui ui;
  InputReader inputReader;

  bool isD3D11PresetHookInit = false;
  FncD3D11Present oPresent;
  HWND window = NULL;
  WNDPROC oWndProc;
  ID3D11Device *pDevice = NULL;
  ID3D11DeviceContext *pContext = NULL;
  ID3D11RenderTargetView *mainRenderTargetView;
};
} // namespace smgm
