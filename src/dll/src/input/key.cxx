#include "smgm/input/key.h"

#include "boost/algorithm/string/case_conv.hpp"
#include "dinput.h"

namespace smgm::input {

std::string GetKeyName(Key key, KeyNameFormat format) {
  static constexpr std::string_view kNamelessKeyFormat = "[[{:02}]]";

  switch (format) {
    default:
    case KeyNameFormat::kPreferNames: {
      const std::string_view key_name = kKeyNames[key];

      return key_name.empty() ? fmt::format(kNamelessKeyFormat, key)
                              : std::string{key_name};
    }
    case KeyNameFormat::kNameless:
      return fmt::format(kNamelessKeyFormat, key);
  }
}

std::optional<Key> GetKeyFromName(const std::string& key_name) {
  if (key_name.empty()) {
    return {};
  }

  const std::string upper_key_name = boost::algorithm::to_upper_copy(key_name);

  for (std::size_t i = 0; i < kKeyNames.size(); ++i) {
    const std::string_view& cur_key_name = kKeyNames[i];

    if (cur_key_name == upper_key_name) {
      return static_cast<Key>(i);
    }
  }

  return {};
}

int GetKeyPriority(input::Key key) {
  switch (key) {
    default:
      return 0;
    case DIK_LWIN:
    case DIK_RWIN:
      return 1;
    case DIK_LCONTROL:
    case DIK_RCONTROL:
    case DIK_LSHIFT:
    case DIK_RSHIFT:
    case DIK_LALT:
    case DIK_RALT:
      return 2;
  }
}

std::string_view StrGuidType(const GUID& guid_type) {
  if (guid_type == GUID_XAxis) {
    return "xaxis";
  } else if (guid_type == GUID_YAxis) {
    return "yaxis";
  } else if (guid_type == GUID_ZAxis) {
    return "zaxis";
  } else if (guid_type == GUID_RxAxis) {
    return "rxaxis";
  } else if (guid_type == GUID_RyAxis) {
    return "ryaxis";
  } else if (guid_type == GUID_RzAxis) {
    return "rzaxis";
  } else if (guid_type == GUID_Slider) {
    return "slider";
  } else if (guid_type == GUID_Button) {
    return "button";
  } else if (guid_type == GUID_Key) {
    return "key";
  } else if (guid_type == GUID_POV) {
    return "pov";
  } else {
    return "unk";
  }
}

}  // namespace smgm::input
