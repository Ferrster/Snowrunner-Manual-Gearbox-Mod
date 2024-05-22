#pragma once

#include <boost/signals2/signal.hpp>
#include <functional>

#include "smgm/input/v2/device.h"

namespace smgm::input::v2::dinput {
class DI_Device final : public Device {
 public:
  DI_Device(LPDIRECTINPUTDEVICE8 instance, const DIDEVICEINSTANCE &info,
            LPCDIDATAFORMAT data_format);
  ~DI_Device();

  inline LPDIRECTINPUTDEVICE8 GetInstance() const { return instance_; }

  inline const std::string &GetGUID() const { return guid_; }

  inline const std::string &GetName() const { return name_; }

  inline const std::string &GetProductName() const { return product_name_; }

  inline DWORD GetType() const noexcept { return type_; }

  inline LPCDIDATAFORMAT GetDataFormat() const noexcept { return data_format_; }

  inline bool IsValid() const noexcept { return instance_ != nullptr; }

  [[nodiscard]] std::shared_ptr<InputState> GetCurrentState() const override;

 protected:
  void Release();
  void FindInputObjects();

 public:
  boost::signals2::signal<void()> sig_device_released;

 protected:
  LPDIRECTINPUTDEVICE8 instance_ = nullptr;
  std::string guid_;
  std::string name_;
  std::string product_name_;
  DWORD type_;
  LPCDIDATAFORMAT data_format_ = nullptr;
  std::function<std::shared_ptr<InputState>()> state_obj_provider_;
};
}  // namespace smgm::input::v2::dinput
