#pragma once

#include <codecvt>
#include <optional>

#include "Windows.h"
#include "dxgi.h"

namespace smgm {
inline std::optional<GUID> ToGUID(const std::string &str_guid) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> wc_conv;
  std::wstring wstr_guid = wc_conv.from_bytes(str_guid);
  GUID res;

  if (IIDFromString(wstr_guid.c_str(), &res) != S_OK) {
    return {};
  }

  return res;
}

inline std::string FromGUID(const GUID &guid) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> wc_conv;
  WCHAR *arr;

  if (StringFromIID(guid, &arr) != S_OK) {
    return {};
  }

  return wc_conv.to_bytes(std::wstring(arr));
}
}  // namespace smgm
