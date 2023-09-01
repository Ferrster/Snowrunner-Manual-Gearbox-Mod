#include "smgm/utils/d3d11_tools.h"

#include <boost/signals2.hpp>

#include "detours.h"
#include "kiero.h"
#include "smgm/smgm.h"

namespace smgm::d3d11 {

HookParams hook_params = {};
bool is_initialized = false;
std::atomic_bool release_requested = false;
boost::signals2::signal<void(const HookParams &)> sig_hook_initialized;
boost::signals2::signal<void(const HookParams &)> sig_before_hook_release;
boost::signals2::signal<void(const HookParams &)> sig_after_hook_release;
boost::signals2::signal<void(const HookParams &)> sig_tick;
boost::signals2::signal<void(const HWND, UINT, WPARAM, LPARAM, bool &)>
    sig_wnd_proc;

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam,
                          LPARAM lParam) {
  bool skip_orig_wnd_proc = false;

  sig_wnd_proc(hWnd, uMsg, wParam, lParam, skip_orig_wnd_proc);

  if (skip_orig_wnd_proc) {
    return true;
  }

  return CallWindowProc(hook_params.o_wnd_proc, hWnd, uMsg, wParam, lParam);
}

void Release(IDXGISwapChain *pSwapChain) {
  kiero::shutdown();

  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  DetourDetach(&(PVOID &)hook_params.o_present,
               (PVOID)&SMGM_HOOK_NAME(D3D11Present));
  DetourTransactionCommit();

  hook_params.device->Release();
  hook_params.ctx->Release();
  pSwapChain->Release();
  hook_params.o_wnd_proc = (WNDPROC)SetWindowLongPtr(
      hook_params.window, GWLP_WNDPROC, (LONG_PTR)(hook_params.o_wnd_proc));

  release_requested = false;
  is_initialized = false;
}

HRESULT __stdcall SMGM_HOOK_NAME(D3D11Present)(IDXGISwapChain *pSwapChain,
                                               UINT SyncInterval, UINT Flags) {
  if (!is_initialized) {
    if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device),
                                        (void **)&hook_params.device))) {
      hook_params.device->GetImmediateContext(&hook_params.ctx);

      DXGI_SWAP_CHAIN_DESC sd;

      pSwapChain->GetDesc(&sd);
      hook_params.window = sd.OutputWindow;

      ID3D11Texture2D *pBackBuffer;

      pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                            (LPVOID *)&pBackBuffer);
      hook_params.device->CreateRenderTargetView(pBackBuffer, NULL,
                                                 &hook_params.rtv);
      pBackBuffer->Release();
      hook_params.o_wnd_proc = (WNDPROC)SetWindowLongPtr(
          hook_params.window, GWLP_WNDPROC, (LONG_PTR)WndProc);
      is_initialized = true;
      sig_hook_initialized(hook_params);
    } else {
      return hook_params.o_present(pSwapChain, SyncInterval, Flags);
    }
  }

  if (smgm::is_exiting || release_requested) {
    sig_before_hook_release(hook_params);
    Release(pSwapChain);
    sig_after_hook_release(hook_params);
    hook_params.o_present(pSwapChain, SyncInterval, Flags);

    return 0;
  }

  sig_tick(hook_params);

  return hook_params.o_present(pSwapChain, SyncInterval, Flags);
}

bool Init([[maybe_unused]] HINSTANCE hinst) {
  if (is_initialized) {
    return true;
  }

  bool is_kiero_hook_inited = false;
  do {
    if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
      DetourTransactionBegin();
      DetourUpdateThread(GetCurrentThread());
      smgm::d3d11::hook_params.o_present =
          (smgm::d3d11::FncD3D11Present)kiero::getMethodsTable()[8];
      DetourAttach(&(PVOID &)smgm::d3d11::hook_params.o_present,
                   (PVOID)&SMGM_HOOK_NAME(D3D11Present));
      DetourTransactionCommit();

      is_kiero_hook_inited = true;
    }
  } while (!is_kiero_hook_inited);

  return true;
}

bool IsInitialized() { return is_initialized; }

void RequestRelease() { release_requested = true; }

}  // namespace smgm::d3d11
