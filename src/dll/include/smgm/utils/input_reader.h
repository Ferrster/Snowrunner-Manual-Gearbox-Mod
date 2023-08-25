#pragma once

#include "Windows.h"
#include "Xinput.h"

#include <atomic>
#include <boost/property_tree/ptree_fwd.hpp>
#include <filesystem>
#include <functional>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

#include <boost/describe.hpp>

namespace smgm {

BOOST_DEFINE_ENUM(InputDeviceType, KEYBOARD, JOYSTICK)

BOOST_DEFINE_ENUM(InputAction, SHIFT_1_GEAR, SHIFT_2_GEAR, SHIFT_3_GEAR,
                  SHIFT_4_GEAR, SHIFT_5_GEAR, SHIFT_6_GEAR, SHIFT_7_GEAR,
                  SHIFT_8_GEAR, SHIFT_HIGH_GEAR, SHIFT_LOW_GEAR,
                  SHIFT_LOW_PLUS_GEAR, SHIFT_LOW_MINUS_GEAR, SHIFT_NEUTRAL,
                  SHIFT_PREV_AUTO_GEAR, SHIFT_NEXT_AUTO_GEAR,
                  SHIFT_REVERSE_GEAR, CLUTCH);

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
} // namespace smgm
