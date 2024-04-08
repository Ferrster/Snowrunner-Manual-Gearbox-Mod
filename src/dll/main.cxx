#include "Windows.h"
#include "Xinput.h"
#include "detours.h"
#include <consoleapi.h>
#include <filesystem>
#include <libloaderapi.h>
#include <memory>
#include <minwindef.h>

#include "fmt/format.h"
#include "fmt/ranges.h"
#include <fmt/core.h>

#include <iterator>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <winerror.h>
#include <winuser.h>

#include "custom_functions.h"
#include "game_data/data_types.h"
#include "game_data/game_data.h"
#include "spdlog/spdlog.h"
#include "utils/format_helpers.h"
#include "utils/input_reader.h"
#include "utils/logging.h"

#define DETOUR_ATTACH(Src)                                                     \
  if (DetourAttach(&(PVOID &)Src, (PVOID)SMGM_HOOK_NAME(Src)) != NO_ERROR)     \
  LOG_DEBUG("Failed to hook function " #Src)
#define DETOUR_DETACH(Src)                                                     \
  DetourDetach(&(PVOID &)Src, (PVOID)SMGM_HOOK_NAME(Src))

std::atomic_bool g_Shutdown = false;
smgm::InputReader *g_InputReader = nullptr;

DWORD WINAPI MainThread(LPVOID param) {
  g_InputReader->WaitForThread();

  FreeLibraryAndExitThread((HMODULE)param, 0);

  LOG_DEBUG("Exiting...");

  return 0;
}

void Init(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
#ifndef SMGM_NO_CONSOLE
  if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
    AllocConsole();
  }
  freopen("CONIN$", "r", stdin);
  freopen("CONOUT$", "w", stdout);
  freopen("CONOUT$", "w", stderr);
#endif

  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

  LOG_DEBUG("SnowRunner Manual Gearbox v0.1.9");

  DetourRestoreAfterWith();
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());

  // DETOUR_ATTACH(SwitchAWD);
  DETOUR_ATTACH(ShiftGear);
  DETOUR_ATTACH(GetMaxGear);
  DETOUR_ATTACH(ShiftToAutoGear);
  DETOUR_ATTACH(SetCurrentVehicle);

  DetourTransactionCommit();

  g_InputReader = new smgm::InputReader;
  g_InputReader->BindKeyboard(VK_F1, [] { g_InputReader->Stop(); });
  g_InputReader->ReadInputConfig(std::filesystem::current_path() / "smgm.ini");
  g_InputReader->Start();

  if (Vehicle *veh = smgm::GetCurrentVehicle()) {
    veh->TruckAction->AutoGearSwitch = false;
  }

  HMODULE gameBase = GetModuleHandleA(NULL);
  LOG_DEBUG(FormatDataTable(
      "Mod initialized", std::make_pair("Game base", gameBase),
      std::make_pair("Current path",
                     std::filesystem::current_path().string())));

  CreateThread(0, 0, MainThread, hinst, 0, 0);
}

void Teardown(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
  LOG_DEBUG("DLL Detach");

  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());

  // DETOUR_DETACH(SwitchAWD);
  DETOUR_DETACH(ShiftGear);
  DETOUR_DETACH(GetMaxGear);
  DETOUR_DETACH(ShiftToAutoGear);
  DETOUR_DETACH(SetCurrentVehicle);

  DetourTransactionCommit();

  // free(g_InputReader);

#ifndef SMGM_NO_CONSOLE
  FreeConsole();
#endif
}

BOOL APIENTRY DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
  if (DetourIsHelperProcess()) {
    return TRUE;
  }

  if (dwReason == DLL_PROCESS_ATTACH) {
    Init(hinst, dwReason, reserved);
  } else if (dwReason == DLL_PROCESS_DETACH) {
    Teardown(hinst, dwReason, reserved);
  }
  return TRUE;
}