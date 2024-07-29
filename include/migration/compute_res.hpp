#ifndef _COMPUTE_RES_
#define _COMPUTE_RES_

#include <type_traits>
#include "../typelist.hpp"
#include "../struct_field_list_base.hpp"
#include <functional>
#include <optional>
#include "../field_accessor.hpp"
#include "catch2/catch.hpp"
#include "field_reader.hpp"
#include "field_list.hpp"


template <fixed_string... fs>
struct fixed_string_list {};

template <fixed_string... fs>
using with_fields = fixed_string_list<fs...>;

// todo add constriants
template <auto callable, typename T, typename fstr_list>
struct compute;


// todo: expression evaluation requested by user shall not be empty but default to empty by library
// todo bring invocable compatibility at type level for strong type guarantee
// todo simplified concept or requires clause
// todo should cv qualification be removed
template <auto callable, typename field_list, fixed_string... req_fields>
    requires (std::invocable<decltype(callable), 
                             decltype(field_list{}[field_accessor<req_fields>{}])...>)
struct compute<callable, field_list, fixed_string_list<req_fields...>> {
  using flist_type = field_list;

  template <typename... fields>
  constexpr auto operator()(struct_field_list<fields...>& flist) {
    return std::invoke(callable, flist[field_accessor<req_fields>{}]...);
  }
};


template <typename T>
struct is_compute_like;

template <auto callable, typename field_list, fixed_string... req_fields>
struct is_compute_like<compute<callable, field_list, fixed_string_list<req_fields...>>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_compute_like {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_size_compute_like_v = is_compute_like<T>::res;

// todo move to another[separate] file
// Metafunction to check if a type is a struct_field_list
template <typename T>
struct is_struct_field_list : std::false_type {};

template <typename... Entries>
struct is_struct_field_list<struct_field_list<Entries...>> : std::true_type {};

template <typename T>
constexpr bool is_struct_field_list_v = is_struct_field_list<T>::value;


template <typename T>
concept size_compute_like = 
  is_size_compute_like_v<T> &&
  requires (T& compute_callable, typename T::flist_type& field_list) {
    { compute_callable(field_list) } -> std::convertible_to<std::size_t>;
  };

template <typename T>
concept bool_compute_like = 
  is_size_compute_like_v<T> &&
  requires (T& compute_callable, typename T::flist_type& field_list) {
    { compute_callable(field_list) } -> std::convertible_to<bool>;
  };

using u32 = unsigned int;
using u8 = unsigned char;
auto size_c = [](u32 a, u32 b) { return static_cast<std::size_t>(a * b); };
auto size_str = [](u32 a, u32 b) { (void)a; (void)b; return std::string("hello"); };
using str_flist = struct_field_list<field<"a", u32, field_size<4>, 0, 0, 0>, field<"b", u32, field_size<4>, 0, 0, 0>>;
using co = compute<size_c, str_flist, fixed_string_list<"a", "b">>;
static_assert(size_compute_like<compute<size_c, str_flist, fixed_string_list<"a", "b">>>);
static_assert(!size_compute_like<compute<size_str, str_flist, fixed_string_list<"a", "b">>>);
static_assert(std::invocable<
  decltype(size_c), 
  decltype(str_flist{}[field_accessor<"a">{}]),
  decltype(str_flist{}[field_accessor<"b">{}])>
);

// todo is this required
template <typename T, typename size>
struct type_tag {
  using type = T;
  using field_size = size;
};

template <typename T>
struct is_type_tag;

template <typename T, typename size>
struct is_type_tag<type_tag<T, size>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_type_tag {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_type_tag_v = is_type_tag<T>::res;

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


template <std::size_t idx, typename... cases>
struct type_switch_impl;

template <std::size_t idx>
struct type_switch_impl<idx> {
  constexpr auto operator()(auto) const -> 
    std::expected<std::size_t, std::string> 
  {
    return std::unexpected("no matches found");
  }
};

// todo check if case and eval result match in terms of types
template <std::size_t idx, match_case_like match_case_head, match_case_like... match_case_rest>
struct type_switch_impl<idx, match_case_head, match_case_rest...> {
  constexpr auto operator()(auto v) const -> 
    std::expected<type_tag, std::string> 
  {
    if(v == match_case_head::value) return idx;
    else return type_switch_impl<idx + 1, match_case_rest...>{}(v);
  }
};

// atleast one type has to match? but anyways if nothing is matches we get 
// std::unexpected
template <typename eval, match_case_like case_head, match_case_like... case_rest>
struct type_switch;

// todo constrain eval to compute type, cases to match cases
// todo constrain eval return type matches all match case values
// todo return tag constructed with match
template <typename eval, match_case_like case_head, match_case_like... case_rest>
struct type_switch {
  template <typename... fields>
  constexpr auto operator()(struct_field_list<fields...>& field_list) const -> 
    std::expected<std::size_t, std::string> 
  {
    auto expression = eval{};
    auto v = expression(field_list);
    return type_switch_impl<0, case_head, case_rest...>{}(v);
  } 
};

// todo constrain v to function like object returning bool
template <bool_compute_like eval, typename T>
struct clause {
  static constexpr auto e = eval;
  using type_tag = T;
};

// todo return type tag constructed from clause
template <typename... clauses>
struct type_ladder;

template <std::size_t idx, typename... clauses>
struct type_ladder_impl;

template <std::size_t idx>
struct type_ladder_impl<idx> {
  constexpr auto operator()(auto) const -> 
    std::expected<std::size_t, std::string> 
  {
    return std::unexpected("no matches found");
  }
};

// todo constrain clause head and clause_rest
template <std::size_t idx, typename clause_head, typename... clause_rest>
struct type_ladder_impl<idx, clause_head, clause_rest...> {
  template <typename... fields>
  constexpr auto operator()(struct_field_list<fields...>& field_list) const -> 
    std::expected<std::size_t, std::string> 
  {
    bool eval_result = clause_head{}(field_list);
    if(eval_result) return idx;
    else return type_ladder_impl<idx + 1, clause_rest...>{}(field_list);
  }
};

template <typename clause_head, typename... clause_rest>
struct type_ladder<clause_head, clause_rest...> {
  template <typename... fields>
  constexpr auto operator()(struct_field_list<fields...>& field_list) const -> 
    std::expected<std::size_t, std::string> 
  {
    return type_ladder_impl<0, clause_head, clause_rest...>{}(field_list);
  }
};

template <typename... clauses>
struct clauses_to_typelist;

template <typename... clauses>
struct clauses_to_typelist {
  using tlist = typelist::typelist<typename clauses::type_tag...>;
  // todo aargh, variable length types might have to computed at cast function
};

#endif // _COMPUTE_RES_
