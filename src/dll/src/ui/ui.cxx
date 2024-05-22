#include "smgm/ui/ui.h"

#include <winuser.h>

#include <set>
#include <vector>

#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"
#include "boost/algorithm/string/join.hpp"
#include "boost/range/adaptor/transformed.hpp"
#include "fmt/xchar.h"
#include "imgui.h"
#include "smgm/input/actions.h"
#include "smgm/input/dinput_reader.h"
#include "smgm/input/key.h"
#include "smgm/input/keybinding.h"
#include "smgm/smgm.h"
#include "smgm/ui/imgui_fmt.h"
#include "smgm/utils/dinput_tools.h"
#include "smgm/utils/enums.h"
#include "smgm/utils/guid.h"
#include "smgm/utils/logging.h"

namespace smgm {

BOOL CALLBACK CbUiEnumDevices(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef) {
  const BYTE dev_type = lpddi->dwDevType & 0xFF;

  if (dev_type != DI8DEVTYPE_DRIVING && dev_type != DI8DEVTYPE_GAMEPAD &&
      dev_type != DI8DEVTYPE_KEYBOARD && dev_type != DI8DEVTYPE_JOYSTICK &&
      dev_type != DI8DEVTYPE_SUPPLEMENTAL) {
    return DIENUM_CONTINUE;
  }

  auto *ui = static_cast<Ui *>(pvRef);

  if (auto guid = FromGUID(lpddi->guidProduct); !guid.empty()) {
    LOG_DEBUG(FormatDataTable(
        "Device info",
        std::make_pair("Name", (const char *)lpddi->tszProductName),
        std::make_pair("GUID", guid)));

    Ui::ListEntryDInputDevice info;

    info.name = std::string{lpddi->tszInstanceName};
    info.product_name = std::string{lpddi->tszProductName};
    info.guid = std::move(guid);
    info.type = lpddi->dwDevType;

    ui->AddDInputDevice(info);
  }
  return DIENUM_CONTINUE;
}

void Ui::UpdateDIDevices() {
  dinput_devs_.clear();

  if (dinput::GetInstance()->EnumDevices(DI8DEVCLASS_ALL, &CbUiEnumDevices,
                                         this, DIEDFL_ATTACHEDONLY) != DI_OK) {
    LOG_ERROR("EnumDevices failed!");
  }
}

bool Ui::Init(const InitParams &params) {
  if (initialized_) {
    return true;
  }

  ImGuiContext *ctx = ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();

  io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
  ImGui_ImplWin32_Init(params.window);
  ImGui_ImplDX11_Init(params.device, params.ctx);

  imgui_ctx_ = ctx;
  init_params_ = params;
  initialized_ = true;

  return true;
}

bool Ui::Destroy() {
  if (!initialized_) {
    return true;
  }

  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext(imgui_ctx_);
  initialized_ = false;

  return true;
}

bool Ui::Draw() {
  if (!initialized_ || !is_open) {
    return false;
  }

  ImGui_ImplDX11_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  if (ImGui::Begin("SMGM Settings", &is_open)) {
    if (ImGui::CollapsingHeader("DirectInput")) {
      if (ImGui::Button("Search for DirectInput devices",
                        {ImGui::GetContentRegionAvail().x, 0})) {
        UpdateDIDevices();
      }
      static std::size_t selected_device_idx = 0;

      ImGui::TextFmt("Found DirectInput devices ({})", dinput_devs_.size());
      ImGui::BeginGroup();
      {
        if (ImGui::BeginListBox("##dinput devices",
                                {ImGui::GetContentRegionAvail().x * .5f,
                                 15 * ImGui::GetTextLineHeightWithSpacing()})) {
          for (std::size_t i = 0; i < dinput_devs_.size(); ++i) {
            const ListEntryDInputDevice &dev_info = dinput_devs_[i];
            const bool is_selected = selected_device_idx == i;

            ImGui::PushID(&dev_info);
            if (ImGui::Selectable(dev_info.name.c_str(), is_selected)) {
              selected_device_idx = i;
            }
            ImGui::PopID();

            if (is_selected) {
              ImGui::SetItemDefaultFocus();
            }
          }

          ImGui::EndListBox();
        }
        ImGui::EndGroup();
      }
      ImGui::SameLine();
      ImGui::BeginGroup();
      {
        const ListEntryDInputDevice &selected_device =
            dinput_devs_.empty() ? ListEntryDInputDevice{}
                                 : dinput_devs_[selected_device_idx];
        ImGui::BeginDisabled(dinput_devs_.empty());
        ImGui::TextFmt("Name: {}", selected_device.name);
        ImGui::TextFmt("Product name: {}", selected_device.product_name);
        ImGui::TextFmt("GUID: {}", selected_device.guid);
        ImGui::TextFmt("Device type: {}", selected_device.type);

        // ImGui::BeginDisabled(dinput::IsDeviceCreated(selected_device.guid));
        if (ImGui::Button("Create device",
                          {ImGui::GetContentRegionAvail().x * .5f, 0})) {
          smgm::device_manager.CreateDirectInputDevice(selected_device.guid);
        }
        // ImGui::EndDisabled();
        // ImGui::BeginDisabled(!dinput::IsDeviceCreated(selected_device.guid));
        ImGui::SameLine();
        if (ImGui::Button("Release device",
                          {ImGui::GetContentRegionAvail().x, 0})) {
          dinput::ReleaseDevice(selected_device.guid);
        }
        // ImGui::EndDisabled();
        ImGui::EndDisabled();
        ImGui::EndGroup();
      }
    }
    if (ImGui::CollapsingHeader("XInput")) {
      if (ImGui::Button("Detect connected XInput controllers",
                        {ImGui::GetContentRegionAvail().x, 0})) {
        smgm::device_manager.RefreshXInputDevices();
      }
    }
  }
  ImGui::ShowDemoWindow();

  ImGui::Render();
  init_params_.ctx->OMSetRenderTargets(1, &init_params_.rtv, NULL);
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  return true;
}

void Ui::PopupSetKeybind(const char *popup_name, input::Keybind &keybind,
                         bool &is_set) {
  static boost::signals2::connection key_listener;
  static input::Keybind pressed_keys;
  static std::string str_pressed_keys;
  static const auto apply = [&](input::Keybind &kb) {
    kb = pressed_keys;
    is_set = true;
    key_listener.disconnect();
  };
  static const auto cancel = [&] { key_listener.disconnect(); };

  is_set = false;
  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, {.5f, .5f});
  if (ImGui::BeginPopupModal(popup_name, nullptr,
                             ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::TextFmt("Mapping keybinding for {}", "test");
    ImGui::BeginDisabled();
    ImGui::InputTextWithHint("", "your mapping", str_pressed_keys.data(),
                             str_pressed_keys.size());
    ImGui::EndDisabled();
    if (ImGui::Button("Accept")) {
      apply(keybind);
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
      cancel();
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
}

void Ui::AddDInputDevice(const ListEntryDInputDevice &info) {
  dinput_devs_.emplace_back(info);
}

}  // namespace smgm
