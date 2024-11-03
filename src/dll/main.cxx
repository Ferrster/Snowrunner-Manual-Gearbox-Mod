#include "Windows.h"
#include "Xinput.h"
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

#include "config/ini_config.hpp"
#include "custom_functions.h"
#include "game_data/data_types.h"
#include "game_data/game_data.h"
#include "spdlog/spdlog.h"
#include "utils/format_helpers.h"
#include "utils/hooks.h"
#include "utils/input_reader.h"
#include "utils/logging.h"

std::atomic_bool g_Shutdown = false;
smgm::InputReader *g_InputReader = nullptr;
smgm::IniConfig g_IniConfig;

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

  LOG_DEBUG("SnowRunner Manual Gearbox v0.1.14");

  if (!g_IniConfig.WriteDefaultConfig()) {
    g_IniConfig.Read();
  }

#ifdef SMGM_USE_DETOURS
  DetourRestoreAfterWith();
#endif
  smgm::AttachHooks();

  g_InputReader = new smgm::InputReader;
  g_InputReader->BindKeyboard(VK_F1, [] { g_InputReader->Stop(); });
  g_InputReader->ReadInputConfig(g_IniConfig);
  g_InputReader->Start();

  if (Vehicle *veh = smgm::GetCurrentVehicle()) {
    veh->TruckAction->IsInAutoMode = false;
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

  smgm::DetachHooks();
#ifndef SMGM_NO_CONSOLE
  FreeConsole();
#endif
}

BOOL APIENTRY DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    Init(hinst, dwReason, reserved);
  } else if (dwReason == DLL_PROCESS_DETACH) {
    Teardown(hinst, dwReason, reserved);
  }
  return TRUE;
}