#pragma once

#include <map>
#include <string>
#include <variant>

#include "Windows.h"
#include "boost/signals2/signal.hpp"
#include "dinput.h"
#include "fmt/format.h"
#include "smgm/input/key.h"

namespace smgm::dinput {

using KeyboardState = std::array<input::Key, 256>;
using JoystickState = DIJOYSTATE2;
using DeviceState = std::variant<std::monostate, KeyboardState, JoystickState>;

/// Available device object, such as button, axis, etc.
struct AvailableObject {
  const DIOBJECTDATAFORMAT &format;
  DIDEVICEOBJECTINSTANCE instance;

  std::string ToCodeStr() const {
    return fmt::format("{}{}", input::StrGuidType(instance.guidType),
                       DIDFT_GETINSTANCE(instance.dwType));
  }
};

template <typename T, typename StateT>
T ReadObjectValue(const StateT &state, const AvailableObject &obj) {
  return *reinterpret_cast<const T *>(reinterpret_cast<const BYTE *>(&state) +
                                      obj.format.dwOfs);
}

template <typename T>
input::KeyState ReadKeyState(const T &state, const AvailableObject &obj) {
  if constexpr (std::is_same_v<T, std::monostate>) {
    return input::KeyState::kReleased;
  }

  const auto value = ReadObjectValue<BYTE>(state, obj);

  return value > 0 ? input::KeyState::kPressed : input::KeyState::kReleased;
}

template <typename T>
input::POVState ReadPOVState(const T &state, const AvailableObject &obj) {
  if constexpr (std::is_same_v<T, std::monostate>) {
    return input::POVState::kReleased;
  }

  const auto value = ReadObjectValue<DWORD>(state, obj);

  switch (value) {
    case 0:
      return input::POVState::kUp;
    case 9000:
      return input::POVState::kRight;
    case 18000:
      return input::POVState::kDown;
    case 27000:
      return input::POVState::kLeft;
    case 4294967295:
      return input::POVState::kReleased;
    default:
      return input::POVState::kReleased;
  }
}

template <typename T>
LONG ReadAxisValue(const T &state, const AvailableObject &obj) {
  if constexpr (std::is_same_v<T, std::monostate>) {
    return std::numeric_limits<LONG>::max();
  }

  const auto value = ReadObjectValue<LONG>(state, obj);

  return value;
}

/// IDirectInputDevice8 instance wrapper
class Device {
 public:
  Device() = default;
  Device(const Device &) = delete;
  Device(Device &&) = default;
  Device &operator=(const Device &) = delete;
  Device &operator=(Device &&) = default;
  Device(LPDIRECTINPUTDEVICE8 instance, const DIDEVICEINSTANCE &info,
         LPCDIDATAFORMAT data_format);
  ~Device();

  void InitFrom(LPDIRECTINPUTDEVICE8 instance, const DIDEVICEINSTANCE &info,
                LPCDIDATAFORMAT data_format);

  inline bool IsValid() const noexcept { return instance_ != nullptr; }

  void Release() {
    if (instance_) {
      instance_->Release();
      sig_device_released();
      instance_ = nullptr;
    }
  }

  inline LPDIRECTINPUTDEVICE8 GetInstance() const { return instance_; }

  inline const std::string &GetGUID() const { return guid_; }

  inline const std::string &GetName() const { return name_; }

  inline const std::string &GetProductName() const { return product_name_; }

  inline DWORD GetType() const noexcept { return type_; }

  inline LPCDIDATAFORMAT GetDataFormat() const noexcept { return data_format_; }

  inline const auto &GetAvailableObjects() const noexcept { return objs_; }

 public:
  boost::signals2::signal<void()> sig_device_released;

 private:
  void FindAvailableObjects();

  /// DirectInput device instance
  LPDIRECTINPUTDEVICE8 instance_ = nullptr;

  std::string guid_;
  std::string name_;
  std::string product_name_;
  DWORD type_;
  LPCDIDATAFORMAT data_format_ = nullptr;
  std::unordered_map<DWORD, AvailableObject> objs_;
};
}  // namespace smgm::dinput
