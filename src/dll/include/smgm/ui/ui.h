#pragma once

#include "Windows.h"

#include <d3d11.h>

#include "imgui.h"

namespace smgm {

class Ui {
public:
  struct InitParams {
    HWND window = NULL;
    ID3D11Device *device = NULL;
    ID3D11DeviceContext *deviceCtx = NULL;
    ID3D11RenderTargetView *renderTargetView = NULL;
  };

  bool Init(const InitParams &params);
  bool Destroy();
  bool Draw();

private:
  ImGuiContext *m_imguiCtx = nullptr;
  InitParams m_initParams;
};

} // namespace smgm
