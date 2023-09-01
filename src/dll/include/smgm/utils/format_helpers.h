#pragma once

#include <iterator>
#include <type_traits>

#include "fmt/format.h"

template <typename... T>
std::string FormatDataTable(const std::string &name,
                            const std::pair<const char *, T> &...field) {
  std::string output = fmt::format("{}:\n", name);

  (
      [&] {
        static constexpr const char *format = "\t{:<32}: {}\n";

        if constexpr (std::is_pointer_v<decltype(field.second)> &&
                      !std::is_same_v<decltype(field.second), const char *>) {
          fmt::format_to(std::back_inserter(output), format, field.first,
                         FormatPointer(field.second));
        } else {
          fmt::format_to(std::back_inserter(output), format, field.first,
                         field.second);
        }
      }(),
      ...);

  return output;
}

inline std::string FormatPointer(void *ptr) {
  return fmt::format("{:0>16x}", (std::uint64_t)ptr);
}

template <typename T>
inline std::string FormatPointer(T *ptr) {
  return FormatPointer((void *)ptr);
}
