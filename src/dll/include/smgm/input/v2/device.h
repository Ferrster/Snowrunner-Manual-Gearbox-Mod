#pragma once

#include <boost/signals2/signal.hpp>
#include <memory>
#include <string>
#include <vector>

#include "smgm/input/v2/input_object.h"
#include "smgm/input/v2/input_state.h"

namespace smgm::input::v2 {
class Device {
 public:
  enum class API { kUnknown = -1, kDirectInput, kXInput };

 public:
  Device(API api, const std::string& id, const std::string& sys_id,
         const std::string& display_name)
      : api_(api), id_(id), sys_id_(sys_id), display_name_(display_name) {}
  Device(const Device&) = delete;
  Device(Device&&) = default;
  Device& operator=(const Device&) = delete;
  Device& operator=(Device&&) = default;
  inline API GetAPI() const noexcept { return api_; }
  const auto& GetInputObjects() const noexcept { return input_objs_; }
  const std::string& GetID() const noexcept { return id_; }
  const std::string& GetSystemID() const noexcept { return sys_id_; }
  const std::string& GetDisplayName() const noexcept { return display_name_; }
  [[nodiscard]] virtual std::shared_ptr<InputState> GetCurrentState() const = 0;

 protected:
  virtual ~Device() { sig_destroyed(); }

 public:
  boost::signals2::signal<void()> sig_destroyed;
  boost::signals2::signal<void()> sig_disconnected;

 protected:
  API api_ = API::kUnknown;
  std::string id_;
  std::string sys_id_;
  std::string display_name_;
  std::vector<std::shared_ptr<InputObject>> input_objs_;
};
}  // namespace smgm::input::v2
