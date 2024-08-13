#ifndef _FIELD_TRAITS_HPP_
#define _FIELD_TRAITS_HPP_


#include "sc_type_traits.hpp"
#include "field_size.hpp"
#include "field.hpp"


template <typename T>
struct is_comptime_sized_field;

template <fixed_string id,
          field_containable T, 
          comptime_size_like size, 
          auto constraint_on_value, 
          auto present_only_if, 
          auto type_deducer>
struct is_comptime_sized_field<field<id, T, size, constraint_on_value, present_only_if, type_deducer>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_comptime_sized_field {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_comptime_sized_field_v = is_comptime_sized_field<T>::res;

template <typename T>
struct is_runtime_sized_field;

// todo: todo var buffer like field constraint
template <fixed_string id,
          typename T, 
          runtime_size_like size, 
          auto constraint_on_value, 
          auto present_only_if, 
          auto type_deducer>
struct is_runtime_sized_field<field<id, T, size, constraint_on_value, present_only_if, type_deducer>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_runtime_sized_field {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_runtime_sized_field_v = is_runtime_sized_field<T>::res;

// todo: move to sc_type_traits
// todo: add constraints such that user defined optionals can also be used 
// todo: also add constraint to permit var length fields
// template <typename T>
// struct is_optional_like;
//
// template <typename T>
// struct is_optional_like {
//   static inline constexpr bool res = false;
// };
//
// template <field_containable T>
// struct is_optional_like<std::optional<T>> {
//   static inline constexpr bool res = true;
// };
//
// template <typename T>
// inline constexpr bool is_optional_like_v = is_optional_like<T>::res;
//
// template <typename T>
// concept optional_like = is_optional_like_v<T>;
//
template <typename T>
struct is_optional_field;

template <fixed_string id,
          optional_like T, 
          typename size, 
          auto constraint_on_value, 
          auto present_only_if, 
          auto type_deducer>
struct is_optional_field<field<id, T, size, constraint_on_value, present_only_if, type_deducer>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_optional_field {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_optional_field_v = is_optional_field<T>::res;

// todo: add constraints such that user defined optionals can also be used 
// todo: move to sc_type_traits
// todo: todo var buffer like field constraint
// template <typename T>
// struct is_variant_like;
//
// template <typename T>
// struct is_variant_like {
//   static constexpr bool res = false;
// };
//
// template <typename... ts>
// struct is_variant_like<std::variant<ts...>> {
//   static constexpr bool res = true;
// };
//
// template <typename T>
// inline constexpr bool is_variant_like_v = is_variant_like<T>::res;
//
// template <typename T>
// concept variant_like = is_variant_like_v<T>;

template <typename T>
struct is_union_field;

template <typename T>
struct is_union_field {
  static constexpr bool res = false;
};

template <fixed_string id,
          variant_like T, 
          typename size, 
          auto constraint_on_value, 
          auto present_only_if, 
          auto type_deducer>
struct is_union_field<field<id, T, size, constraint_on_value, present_only_if, type_deducer>> {
  static constexpr bool res = true;
};

template <typename T>
inline constexpr bool is_union_field_v = is_union_field<T>::res;

template <typename T>
concept field_like = is_comptime_sized_field_v<T> ||
                     is_runtime_sized_field_v<T>  ||
                     is_optional_field_v<T>       ||
                     is_union_field_v<T>;

template <typename... ts>
struct are_all_fields;

template <>
struct are_all_fields<field_list<>> { static constexpr bool all_same = true; };

template <typename T, typename... rest>
struct are_all_fields<field_list<T, rest...>> {
  static constexpr bool all_same = false;
};

template <field_like T, typename... rest>
struct are_all_fields<field_list<T, rest...>> {
  static constexpr bool all_same = true && are_all_fields<field_list<rest...>>::all_same;
};

template <typename T>
inline constexpr bool are_all_fields_v = are_all_fields<T>::all_same;

#endif /*_FIELD_TRAITS_HPP_*/