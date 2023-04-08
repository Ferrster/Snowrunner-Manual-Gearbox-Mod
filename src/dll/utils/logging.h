#pragma once

#include <string>

#include "spdlog/spdlog.h"

inline void LOG_DEBUG() {}
template <typename T1, typename... T2> void LOG_DEBUG(T1 &&arg, T2 &&...args) {
  spdlog::debug(std::forward<T1>(arg));

  LOG_DEBUG(args...);
}
