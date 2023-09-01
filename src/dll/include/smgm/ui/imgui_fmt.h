#pragma once

#include <fmt/format.h>

#include <string>
#include <utility>

#include "imgui.h"

namespace ImGui {
template <typename T, typename... Args>
IMGUI_API void TextFmt(T&& fmt, const Args&... args) {
  std::string str = fmt::format(std::forward<T>(fmt), args...);
  ImGui::TextUnformatted(&*str.begin(), &*str.end());
}
}  // namespace ImGui