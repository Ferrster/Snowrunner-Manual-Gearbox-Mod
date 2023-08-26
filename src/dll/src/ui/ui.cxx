#include "smgm/ui/ui.h"

#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

namespace smgm {
bool Ui::Init(const InitParams &params) {
  ImGuiContext *ctx = ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();

  io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
  ImGui_ImplWin32_Init(params.window);
  ImGui_ImplDX11_Init(params.device, params.deviceCtx);

  m_imguiCtx = ctx;
  m_initParams = params;

  return true;
}

bool Ui::Destroy() {
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext(m_imguiCtx);

  return true;
}

bool Ui::Draw() {
  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  ImGui::ShowDemoWindow();

  ImGui::Render();
  m_initParams.deviceCtx->OMSetRenderTargets(1, &m_initParams.renderTargetView,
                                             NULL);
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  return true;
}
} // namespace smgm
