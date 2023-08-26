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

#include "imgui.h"
#include "smgm/game/data_types/vehicle.h"
#include "smgm/smgm.h"
#include "smgm/utils/format_helpers.h"
#include "smgm/utils/input_reader.h"
#include "smgm/utils/logging.h"

#define DETOUR_ATTACH(Src)                                                     \
  DetourAttach(&(PVOID &)Src, (PVOID)SMGM_HOOK_NAME(Src))
#define DETOUR_DETACH(Src)                                                     \
  DetourDetach(&(PVOID &)Src, (PVOID)SMGM_HOOK_NAME(Src))

std::atomic_bool g_Shutdown = false;
smgm::InputReader *g_InputReader = nullptr;

// ===================================================================
#include "d3d11.h"
#include "dxgi.h"

#include "kiero.h"

#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

using FncPresent = HRESULT(__stdcall *)(IDXGISwapChain *pSwapChain,
                                        UINT SyncInterval, UINT Flags);

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
                                              WPARAM wParam, LPARAM lParam);

FncPresent oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device *pDevice = NULL;
ID3D11DeviceContext *pContext = NULL;
ID3D11RenderTargetView *mainRenderTargetView;

void InitImGui() {
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
  ImGui_ImplWin32_Init(window);
  ImGui_ImplDX11_Init(pDevice, pContext);
}

void DestroyImGui() {
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam,
                          LPARAM lParam) {

  if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
    return true;

  return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain *pSwapChain, UINT SyncInterval,
                            UINT Flags) {
  if (!init) {
    if (SUCCEEDED(
            pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **)&pDevice))) {
      pDevice->GetImmediateContext(&pContext);
      DXGI_SWAP_CHAIN_DESC sd;
      pSwapChain->GetDesc(&sd);
      window = sd.OutputWindow;
      ID3D11Texture2D *pBackBuffer;
      pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                            (LPVOID *)&pBackBuffer);
      pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
      pBackBuffer->Release();
      oWndProc =
          (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
      InitImGui();
      init = true;
    }

    else
      return oPresent(pSwapChain, SyncInterval, Flags);
  }

  if (g_Shutdown) {
    kiero::shutdown();

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID &)oPresent, (PVOID)hkPresent);
    DetourTransactionCommit();

    pDevice->Release();
    pContext->Release();
    pSwapChain->Release();
    oWndProc =
        (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)(oWndProc));
    oPresent(pSwapChain, SyncInterval, Flags);
    return 0;
  }

  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  ImGui::ShowDemoWindow();

  ImGui::Render();

  pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  return oPresent(pSwapChain, SyncInterval, Flags);
}

// ===================================================================

DWORD WINAPI MainThread(LPVOID param) {
  bool init_hook = false;
  do {
    if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
      DetourTransactionBegin();
      DetourUpdateThread(GetCurrentThread());
      oPresent = (FncPresent)kiero::getMethodsTable()[8];
      DetourAttach(&(PVOID &)oPresent, (PVOID)hkPresent);
      DetourTransactionCommit();
      init_hook = true;
    }
  } while (!init_hook);

  g_InputReader->WaitForThread();
  LOG_DEBUG("Exiting...");
  DestroyImGui();
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

  LOG_DEBUG("SnowRunner Manual Gearbox v0.1");

  DetourRestoreAfterWith();
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());

  DETOUR_ATTACH(SwitchAWD);
  DETOUR_ATTACH(ShiftGear);
  DETOUR_ATTACH(GetMaxGear);
  DETOUR_ATTACH(ShiftToAutoGear);
  DETOUR_ATTACH(SetCurrentVehicle);

  DetourTransactionCommit();

  g_InputReader = new smgm::InputReader;
  g_InputReader->BindKeyboard(VK_F1, [] {
    g_Shutdown = true;
    g_InputReader->Stop();
  });
  g_InputReader->ReadInputConfig(std::filesystem::current_path() / "smgm.ini");
  g_InputReader->Start();

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