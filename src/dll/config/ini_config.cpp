#include "config/ini_config.hpp"

#include "utils/logging.h"

#include <boost/property_tree/ini_parser.hpp>

namespace smgm {
IniConfig::IniConfig() : m_path("smgm.ini") {}

IniConfig::IniConfig(const std::filesystem::path &path) : m_path(path) {
  ReadFrom(path);
}

void IniConfig::SetConfigPath(const std::filesystem::path &path) {
  m_path = path;
}

bool IniConfig::Read() { return ReadFrom(m_path); }

bool IniConfig::Write() { return WriteTo(m_path); }

bool IniConfig::ReadFrom(const std::filesystem::path &path) {
  try {
    read_ini(path.u8string(), m_config);
  } catch (const std::exception &e) {
    LOG_DEBUG(fmt::format("Failed to read config file: {}", e.what()));

    return false;
  }

  WriteDefaultValues();

  return true;
}

bool IniConfig::WriteTo(const std::filesystem::path &path) {
  try {
    write_ini(path.u8string(), m_config);
  } catch (const std::exception &e) {
    LOG_DEBUG(fmt::format("Failed to write config file: {}", e.what()));

    return false;
  }

  return true;
}

void IniConfig::WriteDefaultValues() {
  SetIfNotExists("SMGM.DisableGameShifting", true);
}

} // namespace smgm
