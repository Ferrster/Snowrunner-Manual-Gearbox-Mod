#pragma once

#include <d3d11.h>

#include <atomic>

#include "Windows.h"
#include "imgui.h"
#include "smgm/input/keybinding.h"
#include "smgm/utils/dinput_tools.h"

namespace smgm {

class Ui {
 public:
  struct InitParams {
    HWND window = NULL;
    ID3D11Device *device = NULL;
    ID3D11DeviceContext *ctx = NULL;
    ID3D11RenderTargetView *rtv = NULL;
  };
  struct ListEntryDInputDevice {
    std::string name;
    std::string product_name;
    std::string guid;
    DWORD type = 0;
  };

  bool Init(const InitParams &params);
  bool Destroy();
  bool Draw();

  void AddDInputDevice(const ListEntryDInputDevice &info);
  void RemoveDInputDevice(LPCDIDEVICEINSTANCE device);

 private:
  void PopupSetKeybind(const char *popup_name, input::Keybind &keybind,
                       bool &is_set);
  void UpdateDIDevices();

 public:
  bool is_open = true;

 private:
  std::vector<ListEntryDInputDevice> dinput_devs_;
  std::atomic_bool initialized_ = false;
  ImGuiContext *imgui_ctx_ = nullptr;
  InitParams init_params_;
};

}  // namespace smgm
