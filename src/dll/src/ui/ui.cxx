#include "smgm/ui/ui.h"

#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

#include "imgui.h"
#include "smgm/ui/components/imhotkey.h"

#include <vector>

namespace smgm {
bool Ui::Init(const InitParams &params) {
  if (m_initialized) {
    return true;
  }

  ImGuiContext *ctx = ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();

  io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
  ImGui_ImplWin32_Init(params.window);
  ImGui_ImplDX11_Init(params.device, params.deviceCtx);

  m_imguiCtx = ctx;
  m_initParams = params;
  m_initialized = true;

  return true;
}

bool Ui::Destroy() {
  if (!m_initialized) {
    return true;
  }

  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext(m_imguiCtx);
  m_initialized = false;

  return true;
}

bool Ui::Draw() {
  if (!m_initialized) {
    return false;
  }

  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  {
    static std::vector<ImHotKey::HotKey> hotkeys = {
        {"Layout", "Reorder nodes in a simpler layout ", 0xFFFF261D},
        {"Save", "Save the current graph", 0xFFFF1F1D},
        {"Load", "Load an existing graph file", 0xFFFF181D},
        {"Play/Stop", "Play or stop the animation from the current graph",
         0xFFFFFF3F},
        {"SetKey",
         "Make a new animation key with the current parameters values at the "
         "current time ",
         0xFFFFFF1F}};

    if (ImGui::Button("Edit Hotkeys")) {
      ImGui::OpenPopup("HotKeys Editor");
    }
    ImHotKey::Edit(hotkeys.data(), hotkeys.size(), "HotKeys Editor");
  }
  ImGui::ShowDemoWindow();

  ImGui::Render();
  m_initParams.deviceCtx->OMSetRenderTargets(1, &m_initParams.renderTargetView,
                                             NULL);
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  return true;
}
} // namespace smgm
