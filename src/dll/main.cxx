#include "Windows.h"
#include "Xinput.h"
#include "detours.h"
#include <consoleapi.h>
#include <libloaderapi.h>
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
#include "utils/logging.h"

#define DETOUR_ATTACH(Src)                                                     \
  DetourAttach(&(PVOID &)Src, (PVOID)DETOUR_HOOK_NAME(Src))
#define DETOUR_DETACH(Src)                                                     \
  DetourDetach(&(PVOID &)Src, (PVOID)DETOUR_HOOK_NAME(Src))

DWORD WINAPI MainThread(LPVOID param) {
  while (true) {
    if (GetAsyncKeyState(VK_F1)) {
      break;
    }

    Vehicle *curVehicle = smgm::GetCurrentVehicle();

    if (curVehicle == nullptr) {
      continue;
    }

    curVehicle->TruckAction->AutoGearSwitch = false;

    if (XINPUT_KEYSTROKE ks;
        XInputGetKeystroke(XUSER_INDEX_ANY, 0, &ks) == ERROR_SUCCESS) {
      if (ks.Flags & XINPUT_KEYSTROKE_KEYDOWN) {
        switch (ks.VirtualKey) {
        case VK_PAD_DPAD_LEFT: {
          smgm::SwitchToPrevGear(curVehicle);
          break;
        }
        case VK_PAD_DPAD_RIGHT: {
          smgm::SwitchToNextGear(curVehicle);
          break;
        }
        }
      }
    }
  }

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

  LOG_DEBUG("SnowRunner Manual Gearbox v0.1");

  DetourRestoreAfterWith();
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());

  DETOUR_ATTACH(SwitchAWD);
  DETOUR_ATTACH(SwitchGear);
  DETOUR_ATTACH(GetMaxGear);
  DETOUR_ATTACH(SwitchToAutoGear);

  DetourTransactionCommit();

  CreateThread(0, 0, MainThread, hinst, 0, 0);

  HMODULE gameBase = GetModuleHandleA(NULL);
  LOG_DEBUG(FormatDataTable(
      "Mod initialized", std::make_pair("Game base", gameBase),
      std::make_pair("Truck Control Ptr", *GameRelatedData::TruckControlPtr)));
}

void Teardown(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
  LOG_DEBUG("DLL Detach");

  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());

  DETOUR_DETACH(SwitchAWD);
  DETOUR_DETACH(SwitchGear);
  DETOUR_DETACH(GetMaxGear);
  DETOUR_DETACH(SwitchToAutoGear);

  DetourTransactionCommit();

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