#pragma once

#include <string>

#include "spdlog/spdlog.h"

inline void LOG_DEBUG() {}
inline void LOG_INFO() {}
inline void LOG_WARNING() {}
inline void LOG_ERROR() {}

template <typename T1, typename... T2> void LOG_DEBUG(T1 &&arg, T2 &&...args) {
  spdlog::debug(std::forward<T1>(arg));

  LOG_DEBUG(args...);
}

template <typename T1, typename... T2> void LOG_INFO(T1 &&arg, T2 &&...args) {
  spdlog::info(std::forward<T1>(arg));

  LOG_INFO(args...);
}

template <typename T1, typename... T2> void LOG_ERROR(T1 &&arg, T2 &&...args) {
  spdlog::error(std::forward<T1>(arg));

  LOG_ERROR(args...);
}

template <typename T1, typename... T2>
void LOG_WARNING(T1 &&arg, T2 &&...args) {
  spdlog::warn(std::forward<T1>(arg));

  LOG_WARNING(args...);
}
