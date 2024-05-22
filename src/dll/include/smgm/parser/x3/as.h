#pragma once

#include <boost/spirit/home/x3/char/char_class.hpp>
#include <boost/spirit/home/x3/core/parser.hpp>
#include <boost/spirit/home/x3/nonterminal/rule.hpp>

namespace smgm::parser::x3 {
namespace x3 = boost::spirit::x3;

template <typename T>
struct as_type {
  template <typename Expr>
  auto operator[](Expr &&expr) const {
    return x3::rule<struct _, T>{"as"} =
               x3::as_parser(std::forward<Expr>(expr));
  }
};
template <typename T>
inline const as_type<T> as = {};

}  // namespace smgm::parser::x3
