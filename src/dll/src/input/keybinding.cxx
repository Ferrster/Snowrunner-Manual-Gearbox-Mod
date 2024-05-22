#include "smgm/input/keybinding.h"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/spirit/home/x3/char/char.hpp>
#include <boost/spirit/home/x3/char/char_class.hpp>
#include <boost/spirit/home/x3/core/action.hpp>
#include <boost/spirit/home/x3/core/call.hpp>
#include <boost/spirit/home/x3/core/parse.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/directive/lexeme.hpp>
#include <boost/spirit/home/x3/directive/no_case.hpp>
#include <boost/spirit/home/x3/directive/omit.hpp>
#include <boost/spirit/home/x3/directive/skip.hpp>
#include <boost/spirit/home/x3/numeric/int.hpp>
#include <boost/spirit/home/x3/operator/alternative.hpp>
#include <boost/spirit/home/x3/operator/difference.hpp>
#include <boost/spirit/home/x3/operator/kleene.hpp>
#include <boost/spirit/home/x3/operator/list.hpp>
#include <boost/spirit/home/x3/operator/not_predicate.hpp>
#include <boost/spirit/home/x3/operator/plus.hpp>
#include <boost/spirit/home/x3/operator/sequence.hpp>
#include <boost/spirit/home/x3/string/literal_string.hpp>

#include "smgm/parser/x3/as.h"
#include "smgm/utils/logging.h"
#include "smgm/utils/type_traits.h"

namespace {
template <
    typename KeyType,
    std::enable_if_t<std::numeric_limits<KeyType>::is_integer, bool> = true>
bool ParseKeybind(const std::string& str_keybind,
                  std::vector<KeyType>& out_keys) {
  // using namespace smgm::input;
  // using namespace smgm::parser::x3;
  // namespace x3 = boost::spirit::x3;

  // using x3::_attr;
  // using x3::blank;
  // using x3::char_;
  // using x3::int_;
  // using x3::omit;
  // using x3::skip;
  // using x3::string;

  // const auto add_key = [&out_keys](auto& ctx) {
  //   using Attribute = std::decay_t<decltype(_attr(ctx))>;

  //   if constexpr (std::is_same_v<Attribute, std::string>) {
  //     std::string& key_name = _attr(ctx);

  //     boost::algorithm::to_upper(key_name);

  //     const auto it = std::find(cbegin(kKeyNames), cend(kKeyNames),
  //                               boost::to_upper_copy(key_name));

  //     if (it == cend(kKeyNames)) {
  //       LOG_ERROR(fmt::format("key {} is not in the list of keys",
  //       key_name)); _pass(ctx) = true;

  //       return;
  //     }

  //     out_keys.emplace_back(
  //         static_cast<KeyType>(std::distance(cbegin(kKeyNames), it)));
  //   } else if constexpr (std::numeric_limits<Attribute>::is_integer) {
  //     const auto key = static_cast<KeyType>(_attr(ctx));

  //     if (key >= kKeyNames.size()) {
  //       LOG_ERROR(fmt::format("key {} is not in the list of keys", key));
  //       _pass(ctx) = true;

  //       return;
  //     }

  //     out_keys.emplace_back(key);
  //   } else {
  //     static_assert(smgm::always_false_v<Attribute>,
  //                   "unsupported attribute type");
  //   }
  // };
  // const auto add_nameless_key = [&out_keys](auto& ctx) {
  //   std::string& key_code = _attr(ctx);

  //   LOG_DEBUG(fmt::format("Keycode {}", key_code));
  // };
  // const auto kRuleKey = as<std::string>[+(char_ - '+')][add_key];
  // const auto kRuleNamelessKey =
  //     as<std::string>[omit[string("[[")] >> +char_ >> +int_ >>
  //                     omit[string("]]")]][add_nameless_key];
  // const auto kRuleKeybind = (kRuleNamelessKey | kRuleKey) % char_('+');

  // if (!x3::parse(begin(str_keybind), end(str_keybind),
  //                skip(blank)[kRuleKeybind])) {
  //   return false;
  // }

  // return true;

  using namespace smgm::input;
  using namespace smgm::parser::x3;
  namespace x3 = boost::spirit::x3;

  const auto add_nameless_key = [&](auto& ctx) {
    std::string& key = _attr(ctx);

    fmt::print("[add_nameless_key] key {}\n", key);
  };
  const auto add_named_key = [&](auto& ctx) {
    std::string& key = _attr(ctx);

    fmt::print("[add_named_key] key {}\n", key);
  };

  const auto kRuleNamelessKey =
      as<std::string>[x3::lexeme["[[" >> +x3::no_case[x3::char_('a', 'z')] >>
                                 +x3::digit >> "]]"]][add_nameless_key];
  const auto kRuleNamedKey =
      as<std::string>[+(x3::no_case[x3::char_('a', 'z')] | x3::digit)]
                     [add_named_key];
  const auto kRuleKeybinding =
      ((kRuleNamelessKey | kRuleNamedKey) >> !(x3::char_ - '+')) %
      x3::char_('+');

  return x3::phrase_parse(begin(str_keybind), end(str_keybind), kRuleKeybinding,
                          x3::blank);
}
}  // namespace

namespace smgm::input {
Keybind::Keybind(const std::string& str_kb) {
  if (!ParseKeybind(str_kb, keys_)) {
    throw std::runtime_error(
        fmt::format("failed to parse keybind ( {} )", str_kb));
  }
}

bool Keybind::AddKey(Key key) {
  if (std::find(begin(keys_), end(keys_), key) != end(keys_)) return false;

  keys_.emplace_back(key);
  Sort(keys_);

  return true;
}

void Keybind::RemoveKey(Key key) {
  if (auto it = std::find(begin(keys_), end(keys_), key); it != end(keys_)) {
    keys_.erase(it);
    Sort(keys_);
  }
}

void Keybind::Sort(std::vector<Key>& keys) {
  std::sort(begin(keys), end(keys), [](input::Key key_1, input::Key key_2) {
    const auto prio_key_1 = input::GetKeyPriority(key_1);
    const auto prio_key_2 = input::GetKeyPriority(key_2);

    return prio_key_1 == prio_key_2 ? key_1 > key_2 : prio_key_1 > prio_key_2;
  });
}
}  // namespace smgm::input
