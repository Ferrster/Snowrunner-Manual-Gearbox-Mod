#include "Windows.h"

#include "TlHelp32.h"
#include <corecrt_wstring.h>
#include <handleapi.h>

#include <fmt/format.h>
#include <processthreadsapi.h>
#include <spdlog/spdlog.h>
#include <string.h>
#include <winnt.h>

int main(int argc, char **argv) {
  spdlog::set_level(spdlog::level::debug);
  spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
  spdlog::info("SnowRunner Manual Gearbox Mod Loader");

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
    spdlog::critical("Failed to find SnowRunner process!");

    return -1;
  }

  HANDLE gameHandle = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
                                      PROCESS_VM_WRITE,
                                  false, pid);
  if (!gameHandle) {
    spdlog::critical("Failed to open SnowRunner process!");

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
    spdlog::critical("Failed to create remote thread in the target process!");
  }
}
