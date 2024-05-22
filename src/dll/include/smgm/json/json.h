#pragma once

#include <string>

#include "boost/hana/at_key.hpp"
#include "boost/hana/define_struct.hpp"
#include "boost/hana/for_each.hpp"
#include "fmt/format.h"
#include "nlohmann/json.hpp"
#include "smgm/utils/logging.h"

#define SMGM_JSON_ENABLE_SERIALIZATION(T)                                   \
  inline void from_json(const nlohmann::json& j, T& o) {                    \
    boost::hana::for_each(o, [&](const auto& i) {                           \
      auto key = boost::hana::first(i);                                     \
      auto& value = boost::hana::at_key(o, key);                            \
                                                                            \
      using value_type = decltype(value);                                   \
                                                                            \
      const auto& json_value = j.at(boost::hana::to<const char*>(key));     \
                                                                            \
      if (json_value.is_null()) {                                           \
        LOG_DEBUG(                                                          \
            fmt::format("{} is null", boost::hana::to<const char*>(key)));  \
        if constexpr (std::is_arithmetic_v<value_type>) {                   \
          value = 0;                                                        \
        } else if constexpr (std::is_default_constructible_v<value_type>) { \
          value = value_type{};                                             \
        }                                                                   \
      } else {                                                              \
        json_value.get_to(value);                                           \
      }                                                                     \
    });                                                                     \
  }                                                                         \
  inline void to_json(nlohmann::json& j, const T& o) {                      \
    boost::hana::for_each(o, [&](const auto& i) {                           \
      auto key = boost::hana::first(i);                                     \
      const auto& member = boost::hana::at_key(o, key);                     \
                                                                            \
      j[boost::hana::to<const char*>(key)] = member;                        \
    });                                                                     \
  }
