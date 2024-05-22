#pragma once

#include <atomic>
#include <boost/describe.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <filesystem>
#include <functional>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

#include "Windows.h"

namespace smgm {

class InputReader {
 public:
  using FncKeyPressCb = std::function<void()>;

  struct KeyInfo {
    FncKeyPressCb onPressed;
    FncKeyPressCb onReleased;
    bool bPressed = false;
  };

 private:
  std::atomic_bool m_bStop = true;
  std::thread m_readerThread;
  std::shared_mutex m_mtx;
  std::unordered_map<WORD, KeyInfo> m_keysKeyboard, m_keysJoystick;

 private:
  void ProcessKeys();

  void WriteDefaultConfig(const std::filesystem::path &configPath);

 public:
  InputReader();
  ~InputReader();

  void Start();

  void Stop();

  void BindKeyboard(SHORT key, FncKeyPressCb &&onPressed,
                    FncKeyPressCb &&onReleased = {});

  void BindJoystick(WORD key, FncKeyPressCb &&onPressed,
                    FncKeyPressCb &&onReleased = {});

  void WaitForThread();

  bool ReadInputConfig(const std::filesystem::path &configPath);
};
}  // namespace smgm
