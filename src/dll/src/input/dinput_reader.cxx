#include "smgm/input/dinput_reader.h"

#include <fstream>
#include <sstream>

#include "smgm/input/action_bindings.h"
#include "smgm/json/fields.h"
#include "smgm/json/json.h"
#include "smgm/json/structs.h"
#include "smgm/utils/enums.h"

namespace smgm::input {
bool DirectInputReader::Process(std::shared_ptr<dinput::Device> dev_info,
                                const dinput::DeviceState& state) {
  // switch (dev_info->GetType() & 0xFF) {
  //   case DI8DEVTYPE_KEYBOARD: {
  //     ProcessKeys(dev_info, state);

  //     break;
  //   }
  //   case DI8DEVTYPE_DRIVING:
  //   case DI8DEVTYPE_GAMEPAD:
  //   case DI8DEVTYPE_JOYSTICK:
  //   case DI8DEVTYPE_SUPPLEMENTAL: {
  //     ProcessJoystick(dev_info, state);

  //     break;
  //   }
  // }

  const dinput::DeviceState* last_state = GetLastState(dev_info);

  std::visit(
      [&](auto&& cur_state, auto&& last_state) {
        using T1 = std::decay_t<decltype(cur_state)>;
        using T2 = std::decay_t<decltype(last_state)>;

        if constexpr (!std::is_same_v<T2, std::monostate> &&
                      std::is_same_v<T1, T2>) {
          const auto& objs = dev_info->GetAvailableObjects();

          for (auto it = begin(objs); it != end(objs); ++it) {
            const dinput::AvailableObject& obj = it->second;
            const DWORD type = DIDFT_GETTYPE(obj.instance.dwType);

            if (type & DIDFT_BUTTON) {
              const input::KeyState cur_btn_state =
                                        dinput::ReadKeyState(cur_state, obj),
                                    last_btn_state =
                                        dinput::ReadKeyState(last_state, obj);

              if (cur_btn_state != last_btn_state) {
                LOG_DEBUG(fmt::format(
                    "Key \"{}\" (type={}, code_str={}) changed state to {}",
                    std::string{obj.instance.tszName},
                    StrGuidType(obj.instance.guidType), obj.ToCodeStr(),
                    (int)cur_btn_state));
              }
            } else if (type & DIDFT_AXIS) {
              const LONG cur_axis_value = dinput::ReadAxisValue(cur_state, obj),
                         last_axis_value =
                             dinput::ReadAxisValue(last_state, obj);

              if (cur_axis_value != last_axis_value) {
                LOG_DEBUG(fmt::format("Axis \"{}\" value changed to {}",
                                      std::string{obj.instance.tszName},
                                      cur_axis_value));
              }
            } else if (type & DIDFT_POV) {
              const input::POVState cur_pov_value =
                                        dinput::ReadPOVState(cur_state, obj),
                                    last_pov_value =
                                        dinput::ReadPOVState(last_state, obj);

              if (cur_pov_value != last_pov_value) {
                LOG_DEBUG(fmt::format("POV \"{}\" value changed to {}",
                                      std::string{obj.instance.tszName},
                                      (int)cur_pov_value));
              }
            }
          }
        }
      },
      state, last_state ? *last_state : dinput::DeviceState{});

  // if (check_keybindings && keybindings_.count(dev_info->GetGUID()) != 0) {
  //   for (auto& kb : keybindings_[dev_info->GetGUID()]) {
  //     const auto& keys = kb.GetKeys();
  //     bool activate = true;

  //     for (const Key kb_key : keys) {
  //       if (dinput::GetKeyState(state, kb_key) == input::KeyState::kReleased)
  //       {
  //         activate = false;
  //         break;
  //       }
  //     }

  //     if (activate) {
  //       kb();
  //     }
  //   }
  // }

  last_states_.insert_or_assign(dev_info->GetGUID(), state);

  return true;
}

dinput::DeviceState* DirectInputReader::GetLastState(
    std::shared_ptr<dinput::Device> dev_info) {
  return last_states_.count(dev_info->GetGUID()) != 0
             ? &last_states_[dev_info->GetGUID()]
             : nullptr;
}

void DirectInputReader::OnKeyStateChanged(
    std::shared_ptr<dinput::Device> dev_info,
    const dinput::DeviceState& var_dev_state,
    const dinput::AvailableObject& changed_obj, input::KeyState key_state) {
  LOG_DEBUG(fmt::format(
      "[ {} | {}] Key {} {}", dev_info->GetName(), dev_info->GetGUID(),
      std::string{changed_obj.instance.tszName},
      key_state == input::KeyState::kPressed ? "pressed" : "released"));

  sig_key_state_changed(dev_info, var_dev_state, changed_obj, key_state);
}

bool DirectInputReader::BindAction(const std::string& device_guid,
                                   const ActionKeybind& kb,
                                   FncKeybindPressed cb) {
  if (!kb.IsValid()) {
    LOG_WARNING(fmt::format("Invalid keybind {}", kb.ToString()));
    return false;
  }

  KeybindingsList& kbs = keybindings_[device_guid];
  kbs.emplace_back(kb, cb);

  LOG_INFO(fmt::format("Created keybind {} for action {}", kb.ToString(),
                       (int)kb.action));

  return true;
}

bool DirectInputReader::UnbindAction(const std::string& device_guid,
                                     InputAction action) {
  return true;
}

bool DirectInputReader::LoadFromConfig(const nlohmann::json& config) {
  return false;
  // try {
  //   for (const auto& el : config) {
  //     json::Keybind json_kb = el;

  //     LOG_DEBUG(FormatDataTable("Keybind", std::pair{"Action",
  //     json_kb.action},
  //                               std::pair{"Device", json_kb.device_guid},
  //                               std::pair{"Keybind", json_kb.keybind}));

  //     const auto action = EnumFromString<InputAction>(json_kb.action);
  //     auto action_cb = GetCallbackForAction(action);

  //     if (!action_cb) {
  //       continue;
  //     }

  //     Keybind kb(json_kb.keybind);

  //     if (!BindAction(json_kb.device_guid, ActionKeybind(std::move(kb),
  //     action),
  //                     action_cb)) {
  //       continue;
  //     }
  //   }
  // } catch (const std::exception& e) {
  //   LOG_ERROR(
  //       fmt::format("Failed to load keybindings from config: {}", e.what()));
  //   return false;
  // }

  // return true;
}

std::vector<input::Key> DirectInputReader::GetPressedKeys(
    std::shared_ptr<dinput::Device> dev) const {
  std::vector<input::Key> res;

  res.reserve(8);

  if (!last_states_.count(dev->GetGUID())) return {};

  const dinput::DeviceState& state = last_states_.at(dev->GetGUID());

  for (std::size_t i = 0; i < dinput::GetMaxKeys(state); ++i) {
    const input::KeyState key_state =
        dinput::GetKeyState(state, static_cast<input::Key>(i));

    if (key_state == input::KeyState::kPressed) {
      res.emplace_back(static_cast<input::Key>(i));
    }
  }

  return res;
}

bool DirectInputReader::SetKeybind(const std::string& device_guid,
                                   InputAction action,
                                   const std::string& str_new_kb) {
  if (keybindings_.count(device_guid) == 0) {
    return false;
  }
}
}  // namespace smgm::input
