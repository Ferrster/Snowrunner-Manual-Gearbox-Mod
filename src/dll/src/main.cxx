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
#include <processthreadsapi.h>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <winerror.h>
#include <winuser.h>

#include "smgm/game/data_types/vehicle.h"
#include "smgm/smgm.h"
#include "smgm/ui/ui.h"
#include "smgm/utils/dinput_hook.h"
#include "smgm/utils/format_helpers.h"
#include "smgm/utils/input_reader.h"
#include "smgm/utils/logging.h"

#define DETOUR_ATTACH(Src)                                                     \
  DetourAttach(&(PVOID &)Src, (PVOID)SMGM_HOOK_NAME(Src))
#define DETOUR_DETACH(Src)                                                     \
  DetourDetach(&(PVOID &)Src, (PVOID)SMGM_HOOK_NAME(Src))

// ===================================================================
#include "d3d11.h"
#include "dxgi.h"

#include "kiero.h"

#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

smgm::SMGM *g_Mod = nullptr;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
                                              WPARAM wParam, LPARAM lParam);

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam,
                          LPARAM lParam) {

  if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
    return true;

  LOG_DEBUG(fmt::format("hWnd: {} | uMsg: {} | wParam: {} | lParam: {}",
                        (void *)hWnd, uMsg, wParam, lParam));

  return CallWindowProc(smgm::SMGM::GetInstance()->oWndProc, hWnd, uMsg, wParam,
                        lParam);
}

HRESULT __stdcall hkPresent(IDXGISwapChain *pSwapChain, UINT SyncInterval,
                            UINT Flags) {
  if (!g_Mod->isD3D11PresetHookInit) {
    if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device),
                                        (void **)&g_Mod->pDevice))) {
      g_Mod->pDevice->GetImmediateContext(&g_Mod->pContext);

      DXGI_SWAP_CHAIN_DESC sd;

      pSwapChain->GetDesc(&sd);
      g_Mod->window = sd.OutputWindow;

      ID3D11Texture2D *pBackBuffer;

      pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                            (LPVOID *)&pBackBuffer);
      g_Mod->pDevice->CreateRenderTargetView(pBackBuffer, NULL,
                                             &g_Mod->mainRenderTargetView);
      pBackBuffer->Release();
      g_Mod->oWndProc = (WNDPROC)SetWindowLongPtr(g_Mod->window, GWLP_WNDPROC,
                                                  (LONG_PTR)WndProc);
      g_Mod->ui.Init({g_Mod->window, g_Mod->pDevice, g_Mod->pContext,
                      g_Mod->mainRenderTargetView});
      g_Mod->isD3D11PresetHookInit = true;
    } else {
      return g_Mod->oPresent(pSwapChain, SyncInterval, Flags);
    }
  }

  if (g_Mod->isExiting) {
    kiero::shutdown();

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID &)g_Mod->oPresent, (PVOID)hkPresent);
    DetourTransactionCommit();

    g_Mod->pDevice->Release();
    g_Mod->pContext->Release();
    pSwapChain->Release();
    g_Mod->oWndProc = (WNDPROC)SetWindowLongPtr(g_Mod->window, GWLP_WNDPROC,
                                                (LONG_PTR)(g_Mod->oWndProc));
    g_Mod->oPresent(pSwapChain, SyncInterval, Flags);
    return 0;
  }

  g_Mod->ui.Draw();

  return g_Mod->oPresent(pSwapChain, SyncInterval, Flags);
}

// ===================================================================

DWORD WINAPI MainThread(LPVOID param) {
  bool init_hook = false;
  do {
    if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
      DetourTransactionBegin();
      DetourUpdateThread(GetCurrentThread());
      g_Mod->oPresent = (smgm::FncD3D11Present)kiero::getMethodsTable()[8];
      DetourAttach(&(PVOID &)g_Mod->oPresent, (PVOID)hkPresent);
      DetourTransactionCommit();

      init_hook = true;
    }
  } while (!init_hook);

  g_Mod->inputReader.WaitForThread();
  LOG_DEBUG("Exiting...");

  g_Mod->ui.Destroy();

  Sleep(1000);
  FreeLibraryAndExitThread((HMODULE)param, 0);

  return 0;
}

void Init(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
  DisableThreadLibraryCalls(hinst);

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

  LOG_INFO("SnowRunner Manual Gearbox v0.1");

  g_Mod = smgm::SMGM::GetInstance();

  DetourRestoreAfterWith();
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());

  DETOUR_ATTACH(SwitchAWD);
  DETOUR_ATTACH(ShiftGear);
  DETOUR_ATTACH(GetMaxGear);
  DETOUR_ATTACH(ShiftToAutoGear);
  DETOUR_ATTACH(SetCurrentVehicle);

  smgm::InstallDInputHooks(hinst);

  DetourTransactionCommit();

  g_Mod->inputReader.BindKeyboard(VK_F1, [] {
    g_Mod->isExiting = true;
    g_Mod->inputReader.Stop();
  });
  g_Mod->inputReader.BindKeyboard(VK_F2, [] {
    using namespace smgm;
    if (g_DInput->EnumDevices(DI8DEVCLASS_ALL, CbDInputEnumDevices, NULL,
                              DIEDFL_ALLDEVICES) != DI_OK) {
      LOG_ERROR("EnumDevices failed!");
    }
  });
  g_Mod->inputReader.ReadInputConfig(std::filesystem::current_path() /
                                     "smgm.ini");
  g_Mod->inputReader.Start();

  if (smgm::Vehicle *veh = smgm::SMGM::GetCurrentVehicle()) {
    veh->truckAction->isAutoEnabled = false;
  }

  HMODULE gameBase = GetModuleHandleA(NULL);
  LOG_DEBUG(FormatDataTable(
      "Mod initialized", std::make_pair("Game base", gameBase),
      std::make_pair("Current path",
                     std::filesystem::current_path().u8string())));

  CreateThread(0, 0, MainThread, hinst, 0, 0);
}

void Teardown(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
  LOG_DEBUG("DLL Detach");

  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());

  DETOUR_DETACH(SwitchAWD);
  DETOUR_DETACH(ShiftGear);
  DETOUR_DETACH(GetMaxGear);
  DETOUR_DETACH(ShiftToAutoGear);
  DETOUR_DETACH(SetCurrentVehicle);

  smgm::RemoveDInputHooks(hinst);

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