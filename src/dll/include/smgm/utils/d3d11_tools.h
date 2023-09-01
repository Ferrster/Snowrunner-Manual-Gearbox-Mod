#pragma once

#include <boost/signals2.hpp>

#include "Windows.h"
#include "d3d11.h"
#include "smgm/utils/detours_helpers.h"

namespace smgm::d3d11 {
using FncD3D11Present = HRESULT(__stdcall *)(IDXGISwapChain *pSwapChain,
                                             UINT SyncInterval, UINT Flags);
struct HookParams {
  FncD3D11Present o_present = nullptr;
  HWND window = nullptr;
  WNDPROC o_wnd_proc = nullptr;
  ID3D11Device *device = nullptr;
  ID3D11DeviceContext *ctx = nullptr;
  ID3D11RenderTargetView *rtv = nullptr;
};

extern HookParams hook_params;
extern boost::signals2::signal<void(const HookParams &)> sig_hook_initialized;
extern boost::signals2::signal<void(const HookParams &)>
    sig_before_hook_release;
extern boost::signals2::signal<void(const HookParams &)> sig_after_hook_release;
extern boost::signals2::signal<void(const HookParams &)> sig_tick;
extern boost::signals2::signal<void(const HWND, UINT, WPARAM, LPARAM, bool &)>
    sig_wnd_proc;

HRESULT __stdcall SMGM_HOOK_NAME(D3D11Present)(IDXGISwapChain *pSwapChain,
                                               UINT SyncInterval, UINT Flags);

bool Init(HINSTANCE hinst);

bool IsInitialized();

void RequestRelease();

}  // namespace smgm::d3d11
