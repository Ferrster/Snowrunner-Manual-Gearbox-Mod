#include "utils/input_reader.h"
#include "custom_functions.h"
#include "utils/format_helpers.h"
#include "utils/logging.h"
#include <Xinput.h>
#include <boost/describe/enum_to_string.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <exception>
#include <filesystem>
#include <fmt/core.h>
#include <mutex>
#include <utility>
#include <winerror.h>
#include <winuser.h>

#include <boost/mp11.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace smgm {

inline const WORD FromHex(const std::string &value) {
  return static_cast<WORD>(std::stoi(value, nullptr, 16));
}

const std::unordered_map<InputDeviceType, std::unordered_map<InputAction, WORD>>
    DefaultKbs{{KEYBOARD,
                {
                    {SHIFT_PREV_AUTO_GEAR, VK_LCONTROL},
                    {SHIFT_NEXT_AUTO_GEAR, VK_LMENU},
                }},
               {JOYSTICK,
                {
                    {SHIFT_PREV_AUTO_GEAR, VK_PAD_DPAD_LEFT},
                    {SHIFT_NEXT_AUTO_GEAR, VK_PAD_DPAD_RIGHT},
                }}};

InputReader::InputReader() { LOG_DEBUG("Input reader created"); }

InputReader::~InputReader() {
  LOG_DEBUG("Input reader is being destroyed...");

  Stop();
  WaitForThread();

  LOG_DEBUG("Input reader destroyed");
}

void InputReader::Start() {
  if (!m_bStop) {
    return;
  }

  WaitForThread();

  m_bStop = false;

  m_readerThread = std::thread(&InputReader::ProcessKeys, this);
}

void InputReader::WaitForThread() {
  if (m_readerThread.joinable()) {
    m_readerThread.join();
  }
}

void InputReader::Stop() { m_bStop = true; }

void InputReader::BindJoystick(WORD key, FncOnPressed &&onPressed) {
  std::lock_guard lck(m_mtx);

  m_keysJoystick.insert({key, {onPressed}});
}

void InputReader::BindKeyboard(SHORT key, FncOnPressed &&onPressed) {
  std::lock_guard lck(m_mtx);

  m_keysKeyboard.insert({static_cast<WORD>(key), {onPressed}});
}

void InputReader::ProcessKeys() {
  while (!m_bStop) {
    std::shared_lock lck(m_mtx);

    // Handle keyboard input
    for (auto &[key, info] : m_keysKeyboard) {
      if (GetAsyncKeyState(static_cast<SHORT>(key)) & 0x8000) {
        if (!info.bPressed) {
          info.bPressed = true;

          if (info.onPressed) {
            info.onPressed();
          }
        }
      } else {
        info.bPressed = false;
      }
    }

    // Handle joystick input
    if (XINPUT_KEYSTROKE ks;
        XInputGetKeystroke(XUSER_INDEX_ANY, 0, &ks) == ERROR_SUCCESS) {
      if (m_keysJoystick.count(ks.VirtualKey) == 0) {
        continue;
      }
      KeyInfo &info = m_keysJoystick[ks.VirtualKey];

      if (ks.Flags & XINPUT_KEYSTROKE_KEYDOWN) {
        info.bPressed = true;

        if (info.onPressed) {
          info.onPressed();
        }
      } else if (ks.Flags & XINPUT_KEYSTROKE_KEYUP) {
        info.bPressed = false;
      }
    }
  }

  LOG_DEBUG("Finished processing");
}

bool InputReader::ReadInputConfig(const std::filesystem::path &configPath) {
  if (!std::filesystem::exists(configPath)) {
    LOG_DEBUG(fmt::format(
        "No config file found at path {}. Generating default one...",
        configPath.string()));

    WriteDefaultConfig(configPath);
  }

  std::unique_lock lck(m_mtx);

  boost::property_tree::ptree config;
  try {
    boost::property_tree::ini_parser::read_ini(configPath.string(), config);

    static const auto ReadKeybindings =
        [](const boost::property_tree::ptree &pt,
           const std::string &key) -> std::unordered_map<WORD, FncOnPressed> {
      std::unordered_map<WORD, FncOnPressed> result;

      boost::mp11::mp_for_each<
          boost::describe::describe_enumerators<InputAction>>([&](auto D) {
        const auto action = [&]() -> FncOnPressed {
          static const auto ShiftGearFnc = [](std::int32_t gear) {
            return [gear] {
              if (auto *veh = smgm::GetCurrentVehicle()) {
                veh->ShiftToGear(gear);
              }
            };
          };

          switch (D.value) {
          case SHIFT_1_GEAR:
            return ShiftGearFnc(1);
          case SHIFT_2_GEAR:
            return ShiftGearFnc(2);
          case SHIFT_3_GEAR:
            return ShiftGearFnc(3);
          case SHIFT_4_GEAR:
            return ShiftGearFnc(4);
          case SHIFT_5_GEAR:
            return ShiftGearFnc(5);
          case SHIFT_6_GEAR:
            return ShiftGearFnc(6);
          case SHIFT_7_GEAR:
            return ShiftGearFnc(7);
          case SHIFT_8_GEAR:
            return ShiftGearFnc(8);
          case SHIFT_HIGH_GEAR:
            return [] {
              if (auto *veh = smgm::GetCurrentVehicle()) {
                veh->ShiftToHighGear();
              }
            };
          case SHIFT_LOW_GEAR:
            return [] {
              if (auto *veh = smgm::GetCurrentVehicle()) {
                veh->ShiftToLowGear();
              }
            };
          case SHIFT_LOW_PLUS_GEAR:
            return [] {
              if (auto *veh = smgm::GetCurrentVehicle()) {
                veh->ShiftToLowPlusGear();
              }
            };
          case SHIFT_LOW_MINUS_GEAR:
            return [] {
              if (auto *veh = smgm::GetCurrentVehicle()) {
                veh->ShiftToLowMinusGear();
              }
            };
          case SHIFT_NEUTRAL:
            return ShiftGearFnc(0);
          case SHIFT_PREV_AUTO_GEAR:
            return [] {
              if (auto *veh = smgm::GetCurrentVehicle()) {
                veh->ShiftToPrevGear();
              }
            };
          case SHIFT_NEXT_AUTO_GEAR:
            return [] {
              if (auto *veh = smgm::GetCurrentVehicle()) {
                veh->ShiftToNextGear();
              }
            };
          }

          return [] {};
        }();

        const std::string iniKey = fmt::format("{}.{}", key, D.name);
        const auto v = pt.get_optional<std::string>(iniKey);

        LOG_DEBUG(fmt::format("{}: {}", iniKey,
                              v.has_value() ? v.value() : "<empty>"));

        if (v.has_value() && !v.value().empty()) {
          result.insert({FromHex(v.value()), std::move(action)});
        }
      });

      return result;
    };

    for (auto &&[key, fnc] : ReadKeybindings(
             config, boost::describe::enum_to_string(KEYBOARD, "Keyboard"))) {
      m_keysKeyboard.emplace(key, KeyInfo{std::move(fnc)});
    }

    for (auto &&[key, fnc] : ReadKeybindings(
             config, boost::describe::enum_to_string(JOYSTICK, "Joystick"))) {
      m_keysJoystick.emplace(key, KeyInfo{std::move(fnc)});
    }
  } catch (const std::exception &e) {
    LOG_DEBUG(fmt::format("Failed to read config file: {}", e.what()));

    return false;
  }

  return true;
}

void InputReader::WriteDefaultConfig(const std::filesystem::path &configPath) {
  boost::property_tree::ptree config;

  boost::mp11::mp_for_each<
      boost::describe::describe_enumerators<InputDeviceType>>(
      [&](auto deInputDeviceType) {
        boost::mp11::mp_for_each<
            boost::describe::describe_enumerators<InputAction>>(
            [&](auto deInputAction) {
              const auto defaultKey = [&]() -> std::string {
                if (DefaultKbs.count(deInputDeviceType.value) == 0) {
                  return {};
                }

                const auto &deviceDefaultKbs =
                    DefaultKbs.at(deInputDeviceType.value);

                if (deviceDefaultKbs.count(deInputAction.value) == 0) {
                  return {};
                }

                return fmt::format("{:#x}",
                                   deviceDefaultKbs.at(deInputAction.value));
              }();

              config.add(fmt::format("{}.{}", deInputDeviceType.name,
                                     deInputAction.name),
                         defaultKey);
            });
      });
  try {
    boost::property_tree::ini_parser::write_ini(configPath.string(), config);
  } catch (const std::exception &e) {
    LOG_DEBUG(
        fmt::format("Failed to create default config file: {}", e.what()));
  }
}

} // namespace smgm
