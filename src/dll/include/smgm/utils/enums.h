#pragma once

#include <boost/describe.hpp>
#include <exception>
#include <string>
#include <string_view>

#include "fmt/format.h"

namespace smgm {
template <typename T>
std::string_view EnumToString(T e, const char* def_name = "?") {
  return boost::describe::enum_to_string(e, def_name);
}

template <typename T>
T EnumFromString(const std::string& str_enum) {
  T res;

  if (!boost::describe::enum_from_string(str_enum.c_str(), res)) {
    throw std::runtime_error(
        fmt::format("no value for enum string {}", str_enum));
  }

  return res;
}
}  // namespace smgm
