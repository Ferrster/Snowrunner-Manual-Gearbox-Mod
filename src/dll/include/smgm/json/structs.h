#pragma once

#include "smgm/json/json.h"

namespace smgm::json {
struct Keybind {
  BOOST_HANA_DEFINE_STRUCT(Keybind, (std::string, keybind),
                           (std::string, device_guid), (std::string, action));
};
SMGM_JSON_ENABLE_SERIALIZATION(Keybind)
}  // namespace smgm::json
