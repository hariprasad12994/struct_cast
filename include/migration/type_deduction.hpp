#ifndef _TYPE_DEDUCTION_HPP_
#define _TYPE_DEDUCTION_HPP_

#include <variant>
#include "../typelist.hpp"
#include "type_ladder.hpp"
#include "type_switch.hpp"


template <typename... clauses>
struct clauses_to_typelist {
  using tlist = typelist::typelist<typename clauses::type_tag...>;
  // todo aargh, variable length types might have to computed at cast function
};

struct no_type_deduction {};

template <typename T>
struct is_no_type_deduction;

template <typename T>
struct is_no_type_deduction {
  static constexpr bool res = false;
};

template <>
struct is_no_type_deduction<no_type_deduction> {
  static constexpr bool res = true;
};

template <typename T>
inline constexpr bool is_no_type_deduction_v = is_no_type_deduction<T>::res;

template <typename T>
concept no_type_deduction_like = is_no_type_deduction_v<T>;


template <typename... Args>
struct type;

template <no_type_deduction_like T>
struct type<T> {};

// todo constraints
template <typename eval_expression, typename tswitch>
struct type<eval_expression, tswitch> {
  using expression = eval_expression;
  using type_switch = tswitch;

  template <typename... fields>
  auto operator()(const struct_field_list<fields...>& sfl)
    -> std::expected<std::size_t, std::string> const {
    type_switch(eval_expression{}(), sfl); 
  }
};

// todo constraints
template <typename tladder>
struct type<tladder> {
  using type_ladder = tladder;

  template <typename... fields>
  auto operator()(const struct_field_list<fields...>& sfl)
    -> std::expected<std::size_t, std::string> const {
    return type_ladder(sfl);
  }
};


// todo typelist to variant?
template <typename T>
struct variant_from_tlist;

template <typename... type>
struct variant_from_tlist<typelist::typelist<type...>> {
  using variant = std::variant<type...>;
};

template <typename T>
using variant_from_tlist_v = typename variant_from_tlist<T>::variant;

static_assert(std::is_same_v<variant_from_tlist_v<typelist::typelist<int, float>>, std::variant<int, float>>);

#endif // _TYPE_DEDUCTION_HPP_
