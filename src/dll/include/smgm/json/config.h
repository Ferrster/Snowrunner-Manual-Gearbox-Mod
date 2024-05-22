#pragma once

#include <filesystem>

#include "nlohmann/json.hpp"
#include "smgm/json/fields.h"
#include "smgm/json/structs.h"

namespace smgm::json {
class Config final {
 public:
  static constexpr std::string_view kFieldKeybindings = "keybindings";
  static constexpr std::string_view kFieldDeviceNames = "device_names";

  Config() = default;
  Config(const std::filesystem::path& config_path);

  void SetConfigPath(const std::filesystem::path& path);
  const std::filesystem::path& GetConfigPath() const noexcept {
    return config_path_;
  }
  const nlohmann::json& GetConfig() const noexcept { return config_; }
  bool Load();
  bool Load(const std::filesystem::path& path);
  bool Save();
  bool Save(const std::filesystem::path& path);
  std::vector<json::Keybind> GetKeybindings() const;
  std::string GetDeviceName(const std::string& device_guid) const;

 private:
  std::filesystem::path config_path_;
  nlohmann::json config_;
};
}  // namespace smgm::json
