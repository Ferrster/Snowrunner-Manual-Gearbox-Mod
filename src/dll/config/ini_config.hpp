#pragma once

#include <filesystem>

#include <boost/property_tree/ptree.hpp>

namespace smgm {
class IniConfig {
public:
  IniConfig();
  explicit IniConfig(const std::filesystem::path &path);

  [[nodiscard]] const std::filesystem::path &GetConfigPath() const {
    return m_path;
  }

  void SetConfigPath(const std::filesystem::path &path);

  bool Read();

  bool Write();

  bool ReadFrom(const std::filesystem::path &path);

  bool WriteTo(const std::filesystem::path &path);

  [[nodiscard]] const auto &GetConfig() const { return m_config; }

  [[nodiscard]] auto &GetConfig() { return m_config; }

  template <typename T, typename PathT>
  IniConfig &Set(const PathT &key, const T &value) {
    GetConfig().put(key, value);

    return *this;
  }

  template <typename T, typename PathT>
  IniConfig &SetIfNotExists(const PathT &key, const T &value) {
    auto &config = GetConfig();

    if (config.find(key) == config.not_found()) {
      config.add(key, value);
    }

    return *this;
  }

  template <typename T, typename PathT>
  boost::optional<T> GetOptional(const PathT &key) {
    return GetConfig().get_optional<T>(key);
  }

  template <typename T, typename PathT> T Get(const PathT &key) {
    return GetConfig().get<T>(key);
  }

protected:
  void WriteDefaultValues();

  std::filesystem::path m_path;
  boost::property_tree::ptree m_config;
};
} // namespace smgm
