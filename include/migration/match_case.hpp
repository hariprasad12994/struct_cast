#ifndef _MATCH_CASE_HPP_
#define _MATCH_CASE_HPP_

#include "type_tag.hpp"

// todo constrain to data types possible for fields
// todo constrain T?
template <auto v, typename T>
struct match_case {
  static constexpr auto value = v;
  using type_tag = T;
};
template <typename T>
struct is_match_case;

template <auto v, typename h>
struct is_match_case<match_case<v, h>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_match_case {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_match_case_v = is_match_case<T>::res;

template <typename T>
concept match_case_like = is_match_case_v<T>;




#endif // _MATCH_CASE_HPP_
