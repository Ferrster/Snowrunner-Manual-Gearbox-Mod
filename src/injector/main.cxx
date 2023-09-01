// clang-format off
#include "Windows.h"
// clang-format on

#include <corecrt_wstring.h>
#include <fmt/format.h>
#include <handleapi.h>
#include <processthreadsapi.h>
#include <string.h>
#include <winnt.h>

#include "TlHelp32.h"

#define SMGM_LOADER_TITLE "SMGM Loader"

int main(int argc, char **argv) {
  HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hProcessSnap == INVALID_HANDLE_VALUE) {
    return -1;
  }

  PROCESSENTRY32 pe32;
  pe32.dwSize = sizeof(PROCESSENTRY32);

  if (!Process32First(hProcessSnap, &pe32)) {
    CloseHandle(hProcessSnap);
    return -1;
  }

  const char *exeName = "SnowRunner.exe";
  DWORD pid = 0;

  do {
    if (strstr(pe32.szExeFile, exeName) != NULL) {
      pid = pe32.th32ProcessID;

      break;
    }
  } while (Process32Next(hProcessSnap, &pe32));

  CloseHandle(hProcessSnap);

  if (pid == 0) {
    MessageBox(nullptr, "Failed to find SnowRunner process!", SMGM_LOADER_TITLE,
               MB_OK | MB_ICONERROR);

    return -1;
  }

  HANDLE gameHandle = OpenProcess(
      PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, false,
      pid);
  if (!gameHandle) {
    MessageBox(nullptr, "Failed to open SnowRunner process!", SMGM_LOADER_TITLE,
               MB_OK | MB_ICONERROR);

    return -1;
  }

  const WCHAR *dllName = L"smgm.dll";
  std::size_t memSize = (1 + lstrlenW(dllName)) * sizeof(WCHAR);
  LPWSTR bufDllName = LPWSTR(
      VirtualAllocEx(gameHandle, NULL, memSize, MEM_COMMIT, PAGE_READWRITE));

  WriteProcessMemory(gameHandle, bufDllName, dllName, memSize, NULL);

  auto loadLibraryFunction = (PTHREAD_START_ROUTINE)(GetProcAddress(
      GetModuleHandleW(L"Kernel32"), "LoadLibraryW"));

  HANDLE remoteThreadHandle = CreateRemoteThread(
      gameHandle, NULL, 0, loadLibraryFunction, bufDllName, 0, NULL);
  if (!remoteThreadHandle) {
    MessageBox(nullptr,
               "Failed to create loading thread in the target process!",
               SMGM_LOADER_TITLE, MB_OK | MB_ICONERROR);
  }
}
