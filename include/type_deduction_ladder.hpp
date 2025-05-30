#ifndef _TYPE_DEDUCTION_LADDER_HPP_
#define _TYPE_DEDUCTION_LADDER_HPP_

#include "cast_error.hpp"
#include "type_deduction_helper.hpp"
#include <expected>


namespace s2s {
// todo return type tag constructed from clause
template <typename... clauses>
struct type_ladder;

template <std::size_t idx, typename... clauses>
struct type_ladder_impl;

template <std::size_t idx>
struct type_ladder_impl<idx> {
  constexpr auto operator()(const auto&) const -> 
    std::expected<std::size_t, error_reason> 
  {
    return std::unexpected(error_reason::type_deduction_failure);
  }
};

// todo constrain clause head and clause_rest
template <std::size_t idx, typename clause_head, typename... clause_rest>
struct type_ladder_impl<idx, clause_head, clause_rest...> {
  template <typename... fields>
  constexpr auto operator()(const struct_field_list<fields...>& field_list) const -> 
    std::expected<std::size_t, error_reason> 
  {
    bool eval_result = clause_head::e(field_list);
    if(eval_result) return idx;
    else return type_ladder_impl<idx + 1, clause_rest...>{}(field_list);
  }
};

template <clause_like clause_head, clause_like... clause_rest>
struct type_ladder<clause_head, clause_rest...> {
  // ? is this ok
  using variant = variant_from_type_conditions_v<clause_head, clause_rest...>;
  using sizes = size_choices_from_type_conditions_v<clause_head, clause_rest...>;

  template <typename... fields>
  constexpr auto operator()(const struct_field_list<fields...>& field_list) const -> 
    std::expected<std::size_t, error_reason> 
  {
    return type_ladder_impl<0, clause_head, clause_rest...>{}(field_list);
  }
};

template <typename T>
struct is_type_ladder;

template <typename T>
struct is_type_ladder {
  static constexpr bool res = false;
};

template <clause_like clause_head, clause_like... clause_tail>
struct is_type_ladder<type_ladder<clause_head, clause_tail...>> {
  static constexpr bool res = true;
};

template <typename T>
static constexpr bool is_type_ladder_v = is_type_ladder<T>::res;

template <typename T>
concept type_ladder_like = is_type_ladder_v<T>;
} /* namespace s2s */

#endif // _TYPE_LADDER_HPP_
