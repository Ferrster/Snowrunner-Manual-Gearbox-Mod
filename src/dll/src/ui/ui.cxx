#include "smgm/ui/ui.h"

#include <winuser.h>

#include <vector>

#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"
#include "fmt/xchar.h"
#include "imgui.h"
#include "smgm/smgm.h"
#include "smgm/ui/imgui_fmt.h"
#include "smgm/utils/dinput_tools.h"
#include "smgm/utils/guid.h"
#include "smgm/utils/logging.h"

namespace smgm {
const std::unordered_map<unsigned int, std::string> kDInputKeyNames{
    {DIK_ESCAPE, "ESCAPE"},
    {DIK_1, "1"},
    {DIK_2, "2"},
    {DIK_3, "3"},
    {DIK_4, "4"},
    {DIK_5, "5"},
    {DIK_6, "6"},
    {DIK_7, "7"},
    {DIK_8, "8"},
    {DIK_9, "9"},
    {DIK_0, "0"},
    {DIK_MINUS, "-"}, /* - on main keyboard */
    {DIK_EQUALS, "="},
    {DIK_BACK, "BACK"}, /* backspace */
    {DIK_TAB, "TAB"},
    {DIK_Q, "Q"},
    {DIK_W, "W"},
    {DIK_E, "E"},
    {DIK_R, "R"},
    {DIK_T, "T"},
    {DIK_Y, "Y"},
    {DIK_U, "U"},
    {DIK_I, "I"},
    {DIK_O, "O"},
    {DIK_P, "P"},
    {DIK_LBRACKET, "["},
    {DIK_RBRACKET, "]"},
    {DIK_RETURN, "RETURN"}, /* Enter on main keyboard */
    {DIK_LCONTROL, "LCTRL"},
    {DIK_A, "A"},
    {DIK_S, "S"},
    {DIK_D, "D"},
    {DIK_F, "F"},
    {DIK_G, "G"},
    {DIK_H, "H"},
    {DIK_J, "J"},
    {DIK_K, "K"},
    {DIK_L, "L"},
    {DIK_SEMICOLON, ";"},
    {DIK_APOSTROPHE, "'"},
    {DIK_GRAVE, "~"}, /* accent grave */
    {DIK_LSHIFT, "LSHIFT"},
    {DIK_BACKSLASH, "\\"},
    {DIK_Z, "Z"},
    {DIK_X, "X"},
    {DIK_C, "C"},
    {DIK_V, "V"},
    {DIK_B, "B"},
    {DIK_N, "N"},
    {DIK_M, "M"},
    {DIK_COMMA, ","},
    {DIK_PERIOD, "."}, /* . on main keyboard */
    {DIK_SLASH, "/"},  /* / on main keyboard */
    {DIK_RSHIFT, "RSHIFT"},
    {DIK_MULTIPLY, "NUM *"}, /* * on numeric keypad */
    {DIK_LMENU, "LALT"},     /* left Alt */
    {DIK_SPACE, "SPACE"},
    {DIK_CAPITAL, "CAPS"},
    {DIK_F1, "F1"},
    {DIK_F2, "F2"},
    {DIK_F3, "F3"},
    {DIK_F4, "F4"},
    {DIK_F5, "F5"},
    {DIK_F6, "F6"},
    {DIK_F7, "F7"},
    {DIK_F8, "F8"},
    {DIK_F9, "F9"},
    {DIK_F10, "F10"},
    {DIK_NUMLOCK, "NUMLOCK"},
    {DIK_SCROLL, "SCROLL"}, /* Scroll Lock */
    {DIK_NUMPAD7, "NUM 7"},
    {DIK_NUMPAD8, "NUM 8"},
    {DIK_NUMPAD9, "NUM 9"},
    {DIK_SUBTRACT, "NUM -"}, /* - on numeric keypad */
    {DIK_NUMPAD4, "NUM 4"},
    {DIK_NUMPAD5, "NUM 5"},
    {DIK_NUMPAD6, "NUM 6"},
    {DIK_ADD, "NUM +"}, /* + on numeric keypad */
    {DIK_NUMPAD1, "NUM 1"},
    {DIK_NUMPAD2, "NUM 2"},
    {DIK_NUMPAD3, "NUM 3"},
    {DIK_NUMPAD0, "NUM 0"},
    {DIK_DECIMAL, "NUM ."},   /* . on numeric keypad */
    {DIK_OEM_102, "OEM_102"}, /* <> or \| on RT 102-key keyboard (Non-U.S.) */
    {DIK_F11, "F11"},
    {DIK_F12, "F12"},
    {DIK_F13, "F13"},             /*                     (NEC PC98) */
    {DIK_F14, "F14"},             /*                     (NEC PC98) */
    {DIK_F15, "F15"},             /*                     (NEC PC98) */
    {DIK_KANA, "KANA"},           /* (Japanese keyboard)            */
    {DIK_ABNT_C1, "ABNT_C1"},     /* /? on Brazilian keyboard */
    {DIK_CONVERT, "CONVERT"},     /* (Japanese keyboard)            */
    {DIK_NOCONVERT, "NOCONVERT"}, /* (Japanese keyboard)            */
    {DIK_YEN, "YEN"},             /* (Japanese keyboard)            */
    {DIK_ABNT_C2, "ABNT_C2"},     /* Numpad . on Brazilian keyboard */
    {DIK_NUMPADEQUALS, "NUM ="},  /* = on numeric keypad (NEC PC98) */
    {DIK_PREVTRACK,
     "PREVTRACK"},  /* Previous Track (DIK_CIRCUMFLEX on Japanese keyboard) */
    {DIK_AT, "AT"}, /*                     (NEC PC98) */
    {DIK_COLON, "COLON"},           /*                     (NEC PC98) */
    {DIK_UNDERLINE, "UNDERLINE"},   /*                     (NEC PC98) */
    {DIK_KANJI, "KANJI"},           /* (Japanese keyboard)            */
    {DIK_STOP, "STOP"},             /*                     (NEC PC98) */
    {DIK_AX, "AX"},                 /*                     (Japan AX) */
    {DIK_UNLABELED, "UNLABELED"},   /*                        (J3100) */
    {DIK_NEXTTRACK, "NEXTTRACK"},   /* Next Track */
    {DIK_NUMPADENTER, "NUM ENTER"}, /* Enter on numeric keypad */
    {DIK_RCONTROL, "RCONTROL"},
    {DIK_MUTE, "MUTE"},               /* Mute */
    {DIK_CALCULATOR, "CALCULATOR"},   /* Calculator */
    {DIK_PLAYPAUSE, "PLAYPAUSE"},     /* Play / Pause */
    {DIK_MEDIASTOP, "MEDIASTOP"},     /* Media Stop */
    {DIK_VOLUMEDOWN, "VOLUMEDOWN"},   /* Volume - */
    {DIK_VOLUMEUP, "VOLUMEUP"},       /* Volume + */
    {DIK_WEBHOME, "WEBHOME"},         /* Web home */
    {DIK_NUMPADCOMMA, "NUMPADCOMMA"}, /* , on numeric keypad (NEC PC98) */
    {DIK_DIVIDE, "NUM /"},            /* / on numeric keypad */
    {DIK_SYSRQ, "PRTSC"},
    {DIK_RMENU, "RALT"},                /* right Alt */
    {DIK_PAUSE, "PAUSE"},               /* Pause */
    {DIK_HOME, "HOME"},                 /* Home on arrow keypad */
    {DIK_UP, "UP"},                     /* UpArrow on arrow keypad */
    {DIK_PRIOR, "PGUP"},                /* PgUp on arrow keypad */
    {DIK_LEFT, "LEFT"},                 /* LeftArrow on arrow keypad */
    {DIK_RIGHT, "RIGHT"},               /* RightArrow on arrow keypad */
    {DIK_END, "END"},                   /* End on arrow keypad */
    {DIK_DOWN, "DOWN"},                 /* DownArrow on arrow keypad */
    {DIK_NEXT, "PGDN"},                 /* PgDn on arrow keypad */
    {DIK_INSERT, "INSERT"},             /* Insert on arrow keypad */
    {DIK_DELETE, "DELETE"},             /* Delete on arrow keypad */
    {DIK_LWIN, "LWIN"},                 /* Left Windows key */
    {DIK_RWIN, "RWIN"},                 /* Right Windows key */
    {DIK_APPS, "APPS"},                 /* AppMenu key */
    {DIK_POWER, "POWER"},               /* System Power */
    {DIK_SLEEP, "SLEEP"},               /* System Sleep */
    {DIK_WAKE, "WAKE"},                 /* System Wake */
    {DIK_WEBSEARCH, "WEBSEARCH"},       /* Web Search */
    {DIK_WEBFAVORITES, "WEBFAVORITES"}, /* Web Favorites */
    {DIK_WEBREFRESH, "WEBREFRESH"},     /* Web Refresh */
    {DIK_WEBSTOP, "WEBSTOP"},           /* Web Stop */
    {DIK_WEBFORWARD, "WEBFORWARD"},     /* Web Forward */
    {DIK_WEBBACK, "WEBBACK"},           /* Web Back */
    {DIK_MYCOMPUTER, "MYCOMPUTER"},     /* My Computer */
    {DIK_MAIL, "MAIL"},                 /* Mail */
    {DIK_MEDIASELECT, "MEDIASELECT"},   /* Media Select */
};

BOOL CALLBACK CbUiEnumDevices(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef) {
  const BYTE dev_type = lpddi->dwDevType & 0xFF;

  if (dev_type != DI8DEVTYPE_DRIVING && dev_type != DI8DEVTYPE_GAMEPAD &&
      dev_type != DI8DEVTYPE_KEYBOARD && dev_type != DI8DEVTYPE_JOYSTICK &&
      dev_type != DI8DEVTYPE_SUPPLEMENTAL) {
    return DIENUM_CONTINUE;
  }

  auto *ui = static_cast<Ui *>(pvRef);

  if (auto guid = FromGUID(lpddi->guidInstance); !guid.empty()) {
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
        using namespace std::placeholders;
        dinput_devs_.clear();

        if (dinput::dinput_inst->EnumDevices(DI8DEVCLASS_ALL, &CbUiEnumDevices,
                                             this,
                                             DIEDFL_ATTACHEDONLY) != DI_OK) {
          LOG_ERROR("EnumDevices failed!");
        }
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

        ImGui::BeginDisabled(dinput::IsDeviceCreated(selected_device.guid));
        if (ImGui::Button("Create device",
                          {ImGui::GetContentRegionAvail().x * .5f, 0})) {
          dinput::CreateDevice(selected_device.guid);
        }
        ImGui::EndDisabled();
        ImGui::BeginDisabled(!dinput::IsDeviceCreated(selected_device.guid));
        ImGui::SameLine();
        if (ImGui::Button("Release device",
                          {ImGui::GetContentRegionAvail().x, 0})) {
          dinput::ReleaseDevice(selected_device.guid);
        }
        ImGui::EndDisabled();
        ImGui::EndDisabled();
        ImGui::EndGroup();
      }
      ImGui::End();
    }
  }
  ImGui::ShowDemoWindow();

  ImGui::Render();
  init_params_.ctx->OMSetRenderTargets(1, &init_params_.rtv, NULL);
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  return true;
}

void Ui::AddDInputDevice(const ListEntryDInputDevice &info) {
  dinput_devs_.emplace_back(info);
}

}  // namespace smgm
