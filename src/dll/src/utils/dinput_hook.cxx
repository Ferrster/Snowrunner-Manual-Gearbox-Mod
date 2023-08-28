#include "smgm/utils/dinput_hook.h"
#include "smgm/utils/format_helpers.h"
#include "smgm/utils/logging.h"
#include "smgm/utils/utils.h"

#include "detours.h"
#include <utility>

namespace smgm {
bool InstallDInputHooks(HINSTANCE hinst) {
  IDirectInput8 *dinput = nullptr;

  if (DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8,
                         (LPVOID *)&dinput, NULL) != DI_OK) {
    LOG_ERROR("Failed to create DirectInput8 instance!");

    return false;
  }

  LPDIRECTINPUTDEVICE8 lpDInputDevice;

  if (dinput->CreateDevice(GUID_SysKeyboard, &lpDInputDevice, NULL) != DI_OK) {
    dinput->Release();

    LOG_ERROR("Failed to create dinput device!");

    return false;
  }

  {
    void *vftable = *(void **)lpDInputDevice;

    g_oDInputGetDeviceState =
        (FncDInputGetDeviceState) *
        (uintptr_t *)((BYTE *)vftable + 9 * sizeof(void *));
    g_oDInputGetDeviceData =
        (FncDInputGetDeviceData) *
        (uintptr_t *)((BYTE *)vftable + 10 * sizeof(void *));
  }

  DetourAttach(&(PVOID &)g_oDInputGetDeviceState,
               (PVOID)Hook_DInputGetDeviceState);
  DetourAttach(&(PVOID &)g_oDInputGetDeviceData,
               (PVOID)Hook_DInputGetDeviceData);

  g_DInput = dinput;
  g_DInputMouse = lpDInputDevice;

  LOG_INFO("DirectInput8 hooks installed");

  return true;
}

void RemoveDInputHooks(HINSTANCE hinst) {
  DetourDetach(&(PVOID &)g_oDInputGetDeviceState,
               (PVOID)Hook_DInputGetDeviceState);
  DetourDetach(&(PVOID &)g_oDInputGetDeviceData,
               (PVOID)Hook_DInputGetDeviceData);

  g_DInput->Release();
  g_DInputMouse->Release();

  LOG_INFO("DirectInput8 hooks released");
}

HRESULT __stdcall Hook_DInputGetDeviceState(IDirectInputDevice8 *_this,
                                            DWORD cbData, LPVOID lpvData) {
  HRESULT res = g_oDInputGetDeviceState(_this, cbData, lpvData);

  return res;
}

HRESULT __stdcall Hook_DInputGetDeviceData(IDirectInputDevice8 *_this,
                                           DWORD cbObjectData,
                                           LPDIDEVICEOBJECTDATA rgdod,
                                           LPDWORD pdwInOut, DWORD dwFlags) {
  HRESULT res =
      g_oDInputGetDeviceData(_this, cbObjectData, rgdod, pdwInOut, dwFlags);

  return res;
}

} // namespace smgm
