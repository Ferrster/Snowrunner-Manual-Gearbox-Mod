#pragma once

#include "Windows.h"
#include "smgm/utils/format_helpers.h"
#include "smgm/utils/logging.h"

#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"

namespace smgm {
using FncDInputGetDeviceState = HRESULT(__stdcall *)(IDirectInputDevice8 *,
                                                     DWORD, LPVOID);
using FncDInputGetDeviceData = HRESULT(__stdcall *)(IDirectInputDevice8 *,
                                                    DWORD, LPDIDEVICEOBJECTDATA,
                                                    LPDWORD, DWORD);

inline FncDInputGetDeviceState g_oDInputGetDeviceState = nullptr;
inline FncDInputGetDeviceData g_oDInputGetDeviceData = nullptr;
inline IDirectInput8 *g_DInput = nullptr;
inline LPDIRECTINPUTDEVICE8 g_DInputMouse = nullptr;

bool InstallDInputHooks(HINSTANCE hinst);
void RemoveDInputHooks(HINSTANCE hinst);

HRESULT __stdcall Hook_DInputGetDeviceState(IDirectInputDevice8 *, DWORD,
                                            LPVOID);

HRESULT __stdcall Hook_DInputGetDeviceData(IDirectInputDevice8 *, DWORD,
                                           LPDIDEVICEOBJECTDATA, LPDWORD,
                                           DWORD);

inline BOOL CALLBACK CbDInputEnumDevices(LPCDIDEVICEINSTANCE lpddi,
                                         LPVOID pvRef) {
  wchar_t wGUID[40] = {0};
  char cGUID[40] = {0};

  if (StringFromGUID2(lpddi->guidProduct, wGUID, ARRAYSIZE(wGUID))) {
    WideCharToMultiByte(CP_ACP, 0, wGUID, -1, cGUID, 40, NULL, NULL);
    LOG_DEBUG(FormatDataTable(
        "Device info",
        std::make_pair("Name", (const char *)lpddi->tszProductName),
        std::make_pair("GUID", (const char *)cGUID)));
  }
  return DIENUM_CONTINUE;
}

} // namespace smgm
