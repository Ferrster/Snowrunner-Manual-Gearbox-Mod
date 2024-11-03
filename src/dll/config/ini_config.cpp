#include "config/ini_config.hpp"

#include "utils/input_reader.h"
#include "utils/logging.h"

#include <boost/property_tree/ini_parser.hpp>

extern smgm::InputReader *g_InputReader;

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

bool IniConfig::WriteDefaultConfig() {
  if (std::filesystem::exists(m_path)) {
    return false;
  }

  LOG_DEBUG(
      fmt::format("No config file found at path {}. Generating default one...",
                  m_path.u8string()));

  g_InputReader->WriteDefaultConfig(*this);
  WriteDefaultValues();

  return Write();
}

void IniConfig::WriteDefaultValues() {
  SetIfNotExists("SMGM.DisableGameShifting", true);
}

} // namespace smgm
