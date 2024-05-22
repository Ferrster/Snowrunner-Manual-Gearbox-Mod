#include "smgm/json/config.h"

#include <fstream>
#include <sstream>

namespace smgm::json {

Config::Config(const std::filesystem::path& path) : config_path_(path) {
  Load();
}

void Config::SetConfigPath(const std::filesystem::path& path) {
  config_path_ = path;
}

bool Config::Load() { return Load(config_path_); }

bool Config::Load(const std::filesystem::path& path) {
  static constexpr std::string_view kErrorFormatStr =
      "Failed to load config at path {}: {}";

  if (!std::filesystem::exists(path)) {
    LOG_ERROR(
        fmt::format(kErrorFormatStr, path.u8string(), "file does not exist"));
    return false;
  }

  std::ifstream config_file(path);

  if (!config_file.is_open()) {
    LOG_ERROR(
        fmt::format(kErrorFormatStr, path.u8string(), "can't open the file"));
    return false;
  }

  config_ =
      nlohmann::json::parse((std::stringstream() << config_file.rdbuf()).str());

  LOG_DEBUG(fmt::format("Loaded config:\n{}", config_.dump(2)));

  return true;
}

bool Config::Save() { return Save(config_path_); }

bool Config::Save(const std::filesystem::path& path) {
  static constexpr std::string_view kErrorFormatStr =
      "Failed to save config at path {}: {}";

  std::ofstream config_file(path);

  if (!config_file.is_open()) {
    LOG_ERROR(
        fmt::format(kErrorFormatStr, path.u8string(), "can't open the file"));
    return false;
  }

  config_file << config_.dump(2);

  return true;
}

std::vector<json::Keybind> Config::GetKeybindings() const {
  static constexpr std::string_view kErrorFormatStr =
      "Failed to get keybindings from config: {}";

  try {
    const nlohmann::json& json_kbs = config_.at(kFieldKeybindings);
    std::vector<json::Keybind> res;

    res.reserve(json_kbs.size());
    for (json::Keybind json_kb : json_kbs) {
      res.emplace_back(std::move(json_kb));
    }

    return res;
  } catch (const std::exception& e) {
    LOG_ERROR(fmt::format(kErrorFormatStr, e.what()));

    return {};
  }
}

std::string Config::GetDeviceName(const std::string& device_guid) const {
  static constexpr std::string_view kErrorFormatStr =
      "Failed to read device name for guid {}: {}";

  try {
    const nlohmann::json& json_dev_names = config_.at(kFieldDeviceNames);

    return json_dev_names.at(device_guid).get<std::string>();
  } catch (const std::exception& e) {
    LOG_ERROR(fmt::format(kErrorFormatStr, device_guid, e.what()));
    return {};
  }
}

}  // namespace smgm::json
