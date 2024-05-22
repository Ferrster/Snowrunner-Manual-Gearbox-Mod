#pragma once

#include <boost/signals2.hpp>
#include <filesystem>
#include <unordered_map>

#include "nlohmann/json_fwd.hpp"
#include "smgm/input/actions.h"
#include "smgm/input/key.h"
#include "smgm/input/keybinding.h"
#include "smgm/utils/dinput_tools.h"

namespace smgm::input {

class DirectInputReader {
 public:
  class ActionKeybind : public Keybind {
   public:
    ActionKeybind(const std::string& str_kb, InputAction action)
        : Keybind(str_kb), action(action) {}
    ActionKeybind(const Keybind& kb, InputAction action)
        : Keybind(kb), action(action) {}
    InputAction action;
  };

  using FncKeybindPressed = std::function<void()>;

  template <typename R, typename... Args>
  struct CallableActionKeybind : public ActionKeybind {
    using function_type = std::function<R(Args...)>;

    CallableActionKeybind(const ActionKeybind& kb, function_type cb)
        : ActionKeybind(kb), cb(cb){};

    R operator()(Args&&... args) { return cb(std::forward<Args>(args)...); }

    function_type cb;
  };
  using KeybindType = CallableActionKeybind<void>;
  using KeybindingsList = std::vector<KeybindType>;

  bool Process(std::shared_ptr<dinput::Device> dev_info,
               const dinput::DeviceState& state);

  bool BindAction(const std::string& device_guid, const ActionKeybind& kb,
                  FncKeybindPressed cb);
  bool UnbindAction(const std::string& device_guid, InputAction action);
  bool LoadFromConfig(const nlohmann::json& config);
  inline const auto& GetKeybindingsList() const noexcept {
    return keybindings_;
  }
  inline bool SetKeybind(const std::string& device_guid, InputAction action,
                         const std::string& str_new_kb);
  std::vector<input::Key> GetPressedKeys(
      std::shared_ptr<dinput::Device> dev) const;

 private:
  bool ProcessJoystick(std::shared_ptr<dinput::Device> dev_info,
                       const dinput::DeviceState& state);
  void ProcessKeys(std::shared_ptr<dinput::Device> dev_info,
                   const dinput::DeviceState& state);
  void OnKeyStateChanged(std::shared_ptr<dinput::Device> dev_info,
                         const dinput::DeviceState& dev_state,
                         const dinput::AvailableObject& changed_obj,
                         input::KeyState key_state);
  dinput::DeviceState* GetLastState(std::shared_ptr<dinput::Device> dev_info);

  std::unordered_map<std::string, dinput::DeviceState> last_states_;
  std::map<std::string, KeybindingsList> keybindings_;

 public:
  boost::signals2::signal<void(
      std::shared_ptr<dinput::Device>, const dinput::DeviceState&,
      const dinput::AvailableObject& changed_obj, input::KeyState)>
      sig_key_state_changed;
  boost::signals2::signal<void(std::shared_ptr<dinput::Device>,
                               const dinput::DeviceState&,
                               const ActionKeybind&)>
      sig_keybind_triggered;
};

}  // namespace smgm::input
