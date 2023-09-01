#include "Windows.h"
#include "detours.h"
#include "smgm/smgm.h"

void OnDLLAttach(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
  DisableThreadLibraryCalls(hinst);

  smgm::Init(hinst);
}

void OnDLLDetach(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
  smgm::Destroy(hinst);
}

BOOL APIENTRY DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
  if (DetourIsHelperProcess()) {
    return TRUE;
  }

  if (dwReason == DLL_PROCESS_ATTACH) {
    OnDLLAttach(hinst, dwReason, reserved);
  } else if (dwReason == DLL_PROCESS_DETACH) {
    OnDLLDetach(hinst, dwReason, reserved);
  }
  return TRUE;
}
