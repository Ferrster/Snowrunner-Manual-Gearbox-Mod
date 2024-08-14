#include "Windows.h"
#include "smgm/smgm.h"

#ifdef SMGM_USE_DETOURS
#include "detours.h"
#endif

void OnDLLAttach(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
  DisableThreadLibraryCalls(hinst);

  smgm::Init(hinst);
}

void OnDLLDetach(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
  smgm::Destroy(hinst);
}

BOOL APIENTRY DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
#ifdef SMGM_USE_DETOURS
  if (DetourIsHelperProcess()) {
    return TRUE;
  }
#endif

  if (dwReason == DLL_PROCESS_ATTACH) {
    OnDLLAttach(hinst, dwReason, reserved);
  } else if (dwReason == DLL_PROCESS_DETACH) {
    OnDLLDetach(hinst, dwReason, reserved);
  }
  return TRUE;
}
