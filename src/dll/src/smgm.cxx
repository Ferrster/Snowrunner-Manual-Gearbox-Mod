#include "smgm/smgm.h"

#include <combaseapi.h>
#include <dinput.h>
#include <stringapiset.h>
#include <winnt.h>
#include <winuser.h>

#include <fstream>
#include <sstream>

#include "boost/uuid/string_generator.hpp"
#include "boost/uuid/uuid.hpp"
#include "detours.h"
#include "nlohmann/json.hpp"
#include "smgm/actions/action.h"
#include "smgm/game/data_types/combine/truck_control.h"
#include "smgm/input/dinput_reader.h"
#include "smgm/input/v2/input_binding.h"
#include "smgm/input/v2/input_bindings_manager.h"
#include "smgm/input/v2/input_reader.h"
#include "smgm/json/fields.h"
#include "smgm/utils/d3d11_tools.h"
#include "smgm/utils/detours_helpers.h"
#include "smgm/utils/dinput_tools.h"
#include "smgm/utils/format_helpers.h"
#include "smgm/utils/guid.h"
#include "smgm/utils/logging.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
                                              WPARAM wParam, LPARAM lParam);

// ===================== HOOKS =====================

bool SMGM_HOOK_NAME(ShiftGear)(smgm::Vehicle *veh, std::int32_t gear) {
  LOG_DEBUG(fmt::format("[ {} ] Switching gear: {} => {}", FormatPointer(veh),
                        veh->truck_action->gear_1, gear));

  return ShiftGear(veh, gear);
}

std::int32_t SMGM_HOOK_NAME(GetMaxGear)(const smgm::Vehicle *veh) {
  return GetMaxGear(veh);
}

void SMGM_HOOK_NAME(ShiftToAutoGear)(smgm::Vehicle *veh) {
  ShiftToAutoGear(veh);
  veh->truck_action->is_auto_enabled = false;

  SMGM_CALL_HOOK(ShiftGear, veh, 1);
}

void SMGM_HOOK_NAME(SetPowerCoef)(smgm::Vehicle *veh, float coef) {
  SetPowerCoef(veh, coef);
}

void SMGM_HOOK_NAME(SetCurrentVehicle)(smgm::combine::TruckControl *truck_ctrl,
                                       smgm::Vehicle *veh) {
  SetCurrentVehicle(truck_ctrl, veh);

  if (veh) {
    veh->truck_action->is_auto_enabled = false;
  }
}

namespace smgm {
std::atomic_bool is_exiting = false;
smgm::Ui ui;
InputReader input_reader;
input::DirectInputReader dinput_reader;
json::Config config;
input::v2::DeviceManager device_manager;
input::v2::InputReader new_input_reader;
input::v2::InputBindingsManager input_bindings_manager;
boost::signals2::signal<void()> sig_tick;

HWND game_window = nullptr;

DWORD WINAPI MainThread(LPVOID param) {
  auto hinst = (HMODULE)param;
  const auto Exit = [&]() {
    smgm::Exit();
    FreeLibraryAndExitThread(hinst, 0);
  };

  if (!d3d11::Init(hinst)) {
    MessageBox(nullptr, "Failed to set up D3D11 hook!", SMGM_TITLE,
               MB_OK | MB_ICONERROR);
    Exit();
  }

  if (!dinput::Init(hinst)) {
    MessageBox(nullptr, "Failed to initialize DirectInput8!", SMGM_TITLE,
               MB_OK | MB_ICONERROR);
    Exit();
  }

  smgm::ExitWhenReady();
  FreeLibraryAndExitThread(hinst, 0);

  return 0;
}

bool Init(HINSTANCE hinst) {
#ifndef SMGM_NO_CONSOLE
  if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
    AllocConsole();
  }
  freopen("CONIN$", "r", stdin);
  freopen("CONOUT$", "w", stdout);
  freopen("CONOUT$", "w", stderr);
#endif

  LOG_INFO("SnowRunner Manual Gearbox v0.2");

  using namespace std::placeholders;
  d3d11::sig_hook_initialized.connect([](const d3d11::HookParams &params) {
    game_window = params.window;

    // std::shared_ptr<dinput::Device> dev =
    //     dinput::CreateDevice(FromGUID(GUID_SysKeyboard));

    try {
      dinput_reader.LoadFromConfig(
          config.GetConfig().at(json::Config::kFieldKeybindings));
    } catch (const std::exception &e) {
      LOG_DEBUG(fmt::format("cringe {}", e.what()));
    }

    // if (dev) {
    //   dinput_reader.BindAction(dev->GetGUID(),
    //                            input::DirectInputReader::ActionKeybind(
    //                                "2 + 3",
    //                                input::InputAction::SHIFT_4_GEAR),
    //                            [] { LOG_DEBUG("cringe"); });
    // }

    ui.Init({params.window, params.device, params.ctx, params.rtv});
  });
  sig_tick.connect([] {
    // dinput::PollDeviceStates();
    const auto devices = device_manager.GetDevices();

    new_input_reader.ProcessInput(devices);

    for (const auto &device : devices) {
      const auto last_state = new_input_reader.GetLastInputState(device);

      if (!last_state) continue;

      input_bindings_manager.ProcessBindings(*device, *last_state);
    }

    ui.Draw();
  });
  d3d11::sig_tick.connect([](const d3d11::HookParams &params) { sig_tick(); });
  d3d11::sig_wnd_proc.connect([](const HWND hWnd, UINT uMsg, WPARAM wParam,
                                 LPARAM lParam, bool &skipOrigWndProc) {
    if (ui.is_open) {
      ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
      // skipOrigWndProc = true;
    }
  });
  // dinput::sig_device_state.connect(
  //     std::bind(&input::DirectInputReader::Process, &dinput_reader, _1, _2));

  DetourRestoreAfterWith();
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());

  DETOUR_ATTACH(ShiftGear);
  DETOUR_ATTACH(GetMaxGear);
  DETOUR_ATTACH(ShiftToAutoGear);
  DETOUR_ATTACH(SetCurrentVehicle);

  DetourTransactionCommit();

  {
    const std::filesystem::path config_path =
        std::filesystem::current_path() / "smgm.json";
    try {
      config = json::Config(config_path);
    } catch (const std::exception &e) {
      LOG_ERROR(fmt::format("Load config exception: {}", e.what()));
    }
  }

  input_reader.BindKeyboard(VK_F1, [] {
    is_exiting = true;
    input_reader.Stop();
  });
  input_reader.ReadInputConfig(std::filesystem::current_path() / "smgm.ini");
  input_reader.Start();

  if (smgm::Vehicle *veh = smgm::combine::TruckControl::GetCurrentVehicle()) {
    veh->truck_action->is_auto_enabled = false;
  }

  {
    HMODULE game_base = GetModuleHandleA(NULL);
    LOG_DEBUG(FormatDataTable(
        "Mod initialized", std::make_pair("Game base", game_base),
        std::make_pair("Current path",
                       std::filesystem::current_path().u8string())));
  }

  CreateThread(0, 0, MainThread, hinst, 0, 0);

  return true;
}

void Destroy(HINSTANCE hinst) {
  LOG_DEBUG("Destroying mod...");

  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());

  DETOUR_DETACH(ShiftGear);
  DETOUR_DETACH(GetMaxGear);
  DETOUR_DETACH(ShiftToAutoGear);
  DETOUR_DETACH(SetCurrentVehicle);

  DetourTransactionCommit();

#ifndef SMGM_NO_CONSOLE
  HWND hConsole = GetConsoleWindow();

  FreeConsole();
  PostMessage(hConsole, WM_CLOSE, 0, 0);
#endif
}

void Exit() {
  is_exiting = true;
  input_reader.Stop();
  ExitWhenReady();
}

void ExitWhenReady() {
  input_reader.WaitForThread();

  LOG_DEBUG("Exiting...");

  ui.Destroy();
  Sleep(300);
}

}  // namespace smgm
