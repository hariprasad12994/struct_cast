#include <cstdio>
#include <ranges>
#include <variant>
#include <type_traits>
#include <expected>
#include <cstdint>
#include <utility>
#include <vector>
#include <optional>
#include <cstddef>
#include <concepts>
#include <functional>
#include <string>
#include <string_view>
#include <iostream>
#include <algorithm>
#include <bit>
#include <cstring>
#include <array>
#include <cassert>

// Begin /home/hari/repos/s2s/include/lib/containers/static_vector.hpp
#ifndef _STATIC_VECTOR_HPP_
#define _STATIC_VECTOR_HPP_
 
namespace s2s {
template <typename T, std::size_t N>
class static_vector {
public:
  constexpr static_vector() = default;
  template <typename... Args>
  constexpr static_vector(Args&&... entries) {
    (push_back(entries), ...);
  }
  constexpr auto push_back(const T& value) { 
    values[vec_size] = value; 
    vec_size++;
  }
  [[nodiscard]] constexpr const auto& operator[](std::size_t i) const { 
    return values[i]; 
  }
  [[nodiscard]] constexpr auto& operator[](std::size_t i) { 
    return values[i]; 
  }
  [[nodiscard]] constexpr auto begin() const { return &values[0]; }
  [[nodiscard]] constexpr auto end() const { return &values[0] + vec_size; }
  [[nodiscard]] constexpr auto size() const { return vec_size; }
  [[nodiscard]] constexpr auto empty() const { return not vec_size; }
  [[nodiscard]] constexpr auto capacity() const { return N; }

private:
  T values[N]{};
  std::size_t vec_size{0};
};
}

#endif /* _STATIC_VECTOR_HPP_ */

// End /home/hari/repos/s2s/include/lib/containers/static_vector.hpp

// Begin /home/hari/repos/s2s/include/lib/algorithms/algorithms.hpp
#ifndef _ALGORITHMS_HPP_
#define _ALGORITHMS_HPP_
 
// todo namespace algorithms
constexpr auto find_index(const std::ranges::range auto& ts, auto& t) -> std::size_t {
  for(auto i = 0u; i < ts.size(); ++i) {
    if(ts[i] == t) {
      return i;
    }
  }

  return ts.size();
}

constexpr auto find_index_if(const std::ranges::range auto& ts, auto predicate) -> std::size_t {
  for(auto i = 0u; i < ts.size(); ++i) {
    if(predicate(ts[i])) {
      return i;
    }
  }

  return ts.size();
}

constexpr auto equal_ranges(const std::ranges::range auto& xs, const std::ranges::range auto& ys) -> bool {
  if(xs.size() != ys.size()) return false;

  for(auto i = 0u; i < xs.size(); ++i) {
    if(xs[i] != ys[i])
      return false;
  }

  return true;
}

template <typename T>
constexpr void swap_objects(T& a, T& b) {
  T temp = a;
  a = b;
  b = temp;
}

constexpr auto sort_ranges(std::ranges::range auto& ts, auto predicate) {
  for(auto i = 0u; i < ts.size(); ++i) {
    for(auto j = 0u; j < ts.size() - 1; ++j) {
      if(predicate(ts[j + 1], ts[j])) {
        swap_objects(ts[j], ts[j + 1]);
      }
    }
  }
}


#endif /* _ALGORITHMS_HPP_ */

// End /home/hari/repos/s2s/include/lib/algorithms/algorithms.hpp

// Begin /home/hari/repos/s2s/include/lib/containers/static_map.hpp
#ifndef _STATIC_MAP_HPP_
#define _STATIC_MAP_HPP_
 
 
 
 
namespace s2s {
template <typename Key, typename Value>
class Node {
public:
  std::pair<Key, Value> element;

  constexpr Node() = default;
  constexpr Node(Key&& key, Value&& value): element(key, value) {}
  constexpr Node(const Key& key, const Value& value): element(key, value) {}
  constexpr Node(const Node& other): element(other.element) {}
  constexpr Node& operator=(const Node& other) {
    element.first = other.element.first;
    element.second = other.element.second;
    return *this;
  }

  constexpr bool operator<(const Node& rhs) const noexcept {
    return element.first < rhs.element.first;
  }

  constexpr const auto& operator*() const noexcept {
    return element;
  }

  constexpr const auto* operator->() const noexcept {
    return &element;
  }
};


template <typename Key, typename Value, std::size_t N /*, compare? */>
class static_map {
public:
  constexpr static_map() = default;
  constexpr static_map(const std::pair<Key, Value> (&entries)[N]): 
    map(generate_map<N>(entries, std::make_index_sequence<N>{})) {}
  constexpr auto operator[](const Key& key) const  -> std::optional<Value> {
    auto key_index = find_index_if(map, [&key](auto t){ return t.element.first == key; });
    if(key_index != map.size())
      return map[key_index].element.second;
    return std::nullopt;
  }
  [[nodiscard]] constexpr auto begin() const { return map.begin(); }
  [[nodiscard]] constexpr auto end() const { return map.end(); }
  [[nodiscard]] constexpr auto size() const { return map.size(); }
  [[nodiscard]] constexpr auto empty() const { return not map.size(); }
  [[nodiscard]] constexpr auto capacity() const { return N; }

private:
  static_vector<Node<Key, Value>, N> map{};
  template <std::size_t C, std::size_t... Is>
  constexpr auto generate_map(const std::pair<Key, Value> (&entries)[C], std::index_sequence<Is...>) {
    static_vector<Node<Key, Value>, N> m{};
    ([&]() {
      auto key = entries[Is].first;
      auto key_index = find_index_if(m, [&key](auto t){ return t.element.first == key; });
      if(key_index == m.size()) {
        m.push_back(Node(entries[Is].first, entries[Is].second));
        return;
      }
      m[key_index].element.second = entries[Is].second;
    }(), ...);
    sort_ranges(m, std::less<>{});
    return m;
  }
};
}

#endif /* _STATIC_MAP_HPP_ */

// End /home/hari/repos/s2s/include/lib/containers/static_map.hpp

// Begin /home/hari/repos/s2s/include/lib/containers/static_set.hpp
#ifndef _STATIC_SET_HPP_
#define _STATIC_SET_HPP_
 
 
namespace s2s {
template <typename T, std::size_t N>
class static_set {
public:
  constexpr static_set() = default;
  template <typename... Args>
  constexpr static_set(Args&&... entries) {
    (insert(entries), ...);
  }
  constexpr static_set(const static_vector<T, N>& vec) {
    for(auto value: vec) { insert(value); }
  }
  constexpr static_set(const std::array<T, N>& vec) {
    for(auto value: vec) { insert(value); }
  }
  constexpr auto insert(const T& value) { 
    if(find_index(*this, value) == set.size()) {
      set.push_back(value);
    }
  }
  [[nodiscard]] constexpr const auto& operator[](std::size_t i) const { 
    return set[i]; 
  }
  [[nodiscard]] constexpr auto& operator[](std::size_t i) { 
    return set[i]; 
  }
  [[nodiscard]] constexpr auto begin() const { return set.begin(); }
  [[nodiscard]] constexpr auto end() const { return set.end(); }
  [[nodiscard]] constexpr auto size() const { return set.size(); }
  [[nodiscard]] constexpr auto empty() const { return not set.size(); }
  [[nodiscard]] constexpr auto capacity() const { return N; }

private:
  static_vector<T, N> set{};
};
}

#endif /* _STATIC_SET_HPP_ */

// End /home/hari/repos/s2s/include/lib/containers/static_set.hpp

// Begin /home/hari/repos/s2s/include/error/cast_error.hpp
#ifndef _CAST_ERROR_HPP_
#define _CAST_ERROR_HPP_
 
 
namespace s2s {
enum error_reason {
  buffer_exhaustion,
  validation_failure,
  type_deduction_failure
};


struct cast_error {
  error_reason failure_reason;
  std::string_view failed_at;
};


using rw_result = std::expected<void, error_reason>;
using cast_result = std::expected<void, cast_error>;

} /* namespace s2s */

#endif // _CAST_ERROR_HPP_

// End /home/hari/repos/s2s/include/error/cast_error.hpp

// Begin /home/hari/repos/s2s/include/lib/metaprog/mp.hpp
#ifndef _MP_HPP_
#define _MP_HPP_
 
 
 
namespace meta {
using type_identifier = std::size_t;

namespace meta_impl {
// todo: fix warning due to friend injection
template <type_identifier>
struct type_id_key {
  constexpr auto friend get(type_id_key);
};

template <typename T>
struct type_id_value {
  using value_type = T;

  template <std::size_t left = 0u, std::size_t right = 1024u - 1u>
  static constexpr auto gen() -> std::size_t {
    if constexpr (left >= right) {
      constexpr bool exists = requires { get(type_id_key<type_identifier{left}>{}); };
      return (exists ? left + 1 : left);
    } else if constexpr (
      constexpr std::size_t mid = left + (right - left) / 2u; 
      requires { get(type_id_key<type_identifier{mid}>{}); }) 
    {
      return gen<mid + 1u, right>();
    } else {
      return gen<left, mid - 1u>();
    }
  }

  static constexpr auto id = type_identifier{gen()};

  constexpr auto friend get(type_id_key<id>) { 
    return type_id_value{}; 
  }
};
}

template<class T>
inline constexpr type_identifier type_id = meta_impl::type_id_value<T>::id;

template<type_identifier our_id>
using type_of = typename decltype(get(meta_impl::type_id_key<our_id>{}))::value_type;

template<class Fn, class T = decltype([]{})>
[[nodiscard]] inline constexpr auto invoke(Fn&& fn, type_identifier meta) {
  constexpr auto dispatch = [&]<std::size_t... Ns>(std::index_sequence<Ns...>) {
    return std::array{
      []<type_identifier N> {
        return +[](Fn fn) {
          if constexpr (requires { fn.template operator()<N>(); }) {
            return fn.template operator()<N>();
          }
        };
      }.template operator()<type_identifier{Ns}>()...
    };
  }(std::make_index_sequence<std::size_t{type_id<T>}>{});
  return dispatch[std::size_t{meta}](fn);
}

template <template<typename...> typename T, class... Ts, auto = []{}>
[[nodiscard]] inline constexpr auto invoke(type_identifier id) {
  return invoke([]<type_identifier id> {
    using type = type_of<id>;
    if constexpr(requires { T<Ts..., type>::value; }) {
      return T<Ts..., type>::value;
    } else {
      return type_id<typename T<Ts..., type>::type>;
    }
  }, id);
}
}


#endif /* _MP_HPP_ */

// End /home/hari/repos/s2s/include/lib/metaprog/mp.hpp

// Begin /home/hari/repos/s2s/include/field/field_type_info.hpp
#ifndef _FIELD_NODE_HPP_
#define _FIELD_NODE_HPP_
 
// todo better name
struct field_type_info {
  meta::type_identifier id;
  std::size_t occurs_at_idx;
};

#endif /* _FIELD_NODE_HPP_ */

// End /home/hari/repos/s2s/include/field/field_type_info.hpp

// Begin /home/hari/repos/s2s/include/lib/containers/fixed_string.hpp
#ifndef _FIXED_STRING_HPP_
#define _FIXED_STRING_HPP_
 
 
 
 
namespace s2s {
// todo extend for other char types like wchar
template <std::size_t N>
struct fixed_string {
  std::array<char, N + 1> value;
  constexpr fixed_string(): value{} {};
  constexpr fixed_string(const char (&str)[N + 1]) {
    std::copy_n(str, N + 1, value.data());
  }
  constexpr const char* data() const { return value.data(); }
  constexpr char* data() { return value.data(); }
  constexpr std::size_t size() const { return N; }
  constexpr auto to_sv() -> std::string_view const {
    return std::string_view{data()};
  }
};

template <std::size_t N>
fixed_string(const char (&)[N]) -> fixed_string<N - 1>;

template <std::size_t N1, std::size_t N2>
constexpr bool operator==(fixed_string<N1> lhs, fixed_string<N2> rhs) {
  if constexpr(N1 != N2) return false;
  return std::string_view{lhs.data()} == std::string_view{rhs.data()};
}

template <std::size_t N1, std::size_t N2>
constexpr bool operator!=(fixed_string<N1> lhs, fixed_string<N2> rhs) {
  return !(lhs == rhs);
}
} /* namespace s2s */

#endif // _FIXED_STRING_HPP_

// End /home/hari/repos/s2s/include/lib/containers/fixed_string.hpp

// Begin /home/hari/repos/s2s/include/field/field_accessor.hpp
#ifndef _FIELD_ACCESSOR_HPP_
#define _FIELD_ACCESSOR_HPP_
 
namespace s2s {
template <fixed_string id>
struct field_accessor {
  static constexpr auto field_id = id;
};
}

namespace s2s_literals {
template <s2s::fixed_string id>
constexpr auto operator""_f() {
  return s2s::field_accessor<id>{};
}
}

#endif // _FIELD_ACCESSOR_HPP_

// End /home/hari/repos/s2s/include/field/field_accessor.hpp

// Begin /home/hari/repos/s2s/include/lib/metaprog/fixed_string_list.hpp
#ifndef _FIXED_STRING_LIST_HPP_
#define _FIXED_STRING_LIST_HPP_


// status: might be deprecated due to value MP
 
namespace s2s {
template <fixed_string... fs>
struct fixed_string_list {};

template <fixed_string... fs>
using with_fields = fixed_string_list<fs...>;

template <typename T>
struct is_field_name_list;

template <typename T>
struct is_field_name_list {
  static constexpr bool res = false;
};

template <fixed_string... fs>
struct is_field_name_list<with_fields<fs...>> {
  static constexpr bool res = true;
};

template <typename T>
inline constexpr bool is_field_name_list_v = is_field_name_list<T>::res;


struct empty_list{};
struct not_a_list{};
struct out_of_bound{};


template <typename T>
struct size;

template <>
struct size<fixed_string_list<>> {
  static constexpr std::size_t N = 0;
};

template <fixed_string head, fixed_string... tail>
struct size<fixed_string_list<head, tail...>> {
  static constexpr std::size_t N = 1 + size<fixed_string_list<tail...>>::N;
};

template <typename T>
inline constexpr std::size_t size_v = size<T>::N;

template <typename T>
struct front;

template <fixed_string head, fixed_string... tail>
struct front<fixed_string_list<head, tail...>> {
  static constexpr auto string = head;
};

template <typename T>
inline constexpr auto front_t = front<T>::string;

template <std::size_t idx, std::size_t key, std::size_t count, typename T>
struct get;

template <std::size_t key, std::size_t count, fixed_string head, fixed_string... tail>
struct get<key, key, count, fixed_string_list<head, tail...>> {
  static constexpr auto string = head;
};

template <std::size_t idx, std::size_t key, std::size_t count, fixed_string head, fixed_string... tail>
struct get<idx, key, count, fixed_string_list<head, tail...>> {
  static constexpr auto string = get<idx + 1, key, count, fixed_string_list<tail...>>::string;
};

template <std::size_t key, typename T>
inline constexpr auto get_t = get<0, key, size_v<T>, T>::string; 

template <std::size_t count, typename T>
struct pop;

template <std::size_t count>
struct pop<count, fixed_string_list<>> {
  using type = fixed_string_list<>;
};

template <std::size_t count, fixed_string head, fixed_string... tail>
struct pop<count, fixed_string_list<head, tail...>> {
  using type = typename pop<count - 1, fixed_string_list<tail...>>::type;
};

template <fixed_string head, fixed_string... tail>
struct pop<0, fixed_string_list<head, tail...>> {
  using type = fixed_string_list<head, tail...>;
};

template <std::size_t count, typename T>
using pop_t = typename pop<count, T>::type;

template <typename T>
concept field_name_list = is_field_name_list_v<T>;
} /* namespace s2s */


#endif // _FIXED_STRING_LIST_HPP_

// End /home/hari/repos/s2s/include/lib/metaprog/fixed_string_list.hpp

// Begin /home/hari/repos/s2s/include/lib/metaprog/typelist.hpp
#ifndef _TYPELIST_HPP_
#define _TYPELIST_HPP_


// status: might be deprecated due to value MP
 
 
namespace s2s {
namespace typelist {
struct null {};

template <typename... ts>
struct list;

template <typename... ts>
struct list{};

template <>
struct list<>{};

template <typename... ts>
struct any_of;

template <typename... ts>
struct any_of {};

template <typename t>
struct any_of<list<>, t> { static constexpr bool res = false; };

template <typename t, typename... rest>
struct any_of<list<t, rest...>, t> { static constexpr bool res = true; };

template <typename t, typename u, typename... rest>
struct any_of<list<u, rest...>, t> { static constexpr bool res = false || any_of<list<rest...>, t>::res; };

template <typename list, typename type>
inline constexpr bool any_of_v = any_of<list, type>::res;

template <typename... ts>
struct all_are_same;

template <>
struct all_are_same<list<>> {
  static constexpr auto all_same = true;
};

template <typename T>
struct all_are_same<list<T>> {
  static constexpr auto all_same = true;
};

template <typename T, typename U, typename... rest>
struct all_are_same<list<T, U, rest...>> {
  static constexpr auto all_same = std::is_same_v<T, U> && all_are_same<list<U, rest...>>::all_same;
};

template <typename T, typename... rest>
struct all_are_same<list<T, rest...>> {
  static constexpr auto all_same = false;
};

template <typename L>
inline constexpr bool all_are_same_v = all_are_same<L>::all_same;

template <typename... ts>
struct front;

template <typename t, typename... ts>
struct front<list<t, ts...>> {
  using front_t = t;
};

template <>
struct front<list<>> {
  using front_t = null;
};

template <typename L>
using front_t = typename front<L>::front_t;

} // namespace list
} /* namespace s2s */

#endif // _TYPELIST_HPP_

// End /home/hari/repos/s2s/include/lib/metaprog/typelist.hpp

// Begin /home/hari/repos/s2s/include/field_size/field_size.hpp
#ifndef _FIELD_SIZE_HPP_
#define _FIELD_SIZE_HPP_
 
 
 
namespace s2s {
template <typename size_type>
struct field_size;

template <typename size_type>
struct field_size {
  using size_type_t = size_type;
};

struct size_dont_care {};

template <std::size_t N>
struct fixed;

template <std::size_t N>
struct fixed {
  static constexpr auto count = N;
};

template <fixed_string id>
using len_from_field = field_accessor<id>;

template <auto callable, field_name_list req_fields>
struct size_from_fields;

// todo constraint for callable
template <auto callable, field_name_list req_fields>
struct size_from_fields {
  static constexpr auto f = callable;
  static constexpr auto req_field_list = req_fields{};
};

template <auto callable, field_name_list ids>
using len_from_fields = size_from_fields<callable, ids>;

// todo size type for holding multiple sizes in case of union fields
template <typename... size_type>
struct size_choices;

template <typename... size_type>
struct size_choices {
  using choices = typelist::list<size_type...>;
  static auto constexpr num_of_choices = sizeof...(size_type);
};

// Metafunctions for checking if a type is a size type
template <typename T>
struct is_fixed_size;

template <std::size_t N>
struct is_fixed_size<field_size<fixed<N>>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_fixed_size {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_fixed_size_v = is_fixed_size<T>::res;

template <typename T>
struct is_variable_size;

template <typename T>
struct is_variable_size {
  static constexpr bool res = false;
};

template <fixed_string id>
struct is_variable_size<field_size<len_from_field<id>>> {
  static constexpr bool res = true;
};

template <auto callable, field_name_list ids>
struct is_variable_size<field_size<len_from_fields<callable, ids>>> {
  static constexpr bool res = true;
};

template <typename T>
inline constexpr bool is_variable_size_v = is_variable_size<T>::res;

// Concepts for checking if a type is a size type
template <typename T>
concept fixed_size_like = is_fixed_size_v<T>;

template <typename T>
concept variable_size_like = is_variable_size_v<T>;

template <typename T>
struct is_selectable_size;

template <typename T>
concept atomic_size = fixed_size_like<T> || variable_size_like<T>;

template <atomic_size... size_type>
struct is_selectable_size<field_size<size_choices<size_type...>>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_selectable_size {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_selectable_size_v = is_fixed_size<T>::res;

template <typename T>
concept selectable_size_like = is_selectable_size_v<T>;

template <typename T>
concept is_size_like = fixed_size_like<T>    ||
                       variable_size_like<T> ||
                       selectable_size_like<T>;

template <typename T>
struct is_size_dont_care;

template <>
struct is_size_dont_care<field_size<size_dont_care>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_size_dont_care {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_size_dont_care_v = is_size_dont_care<T>::res;

template <typename T>
concept size_dont_care_like = is_size_dont_care_v<T>;

} /* namespace s2s */


#endif // _FIELD_SIZE_HPP_

// End /home/hari/repos/s2s/include/field_size/field_size.hpp

// Begin /home/hari/repos/s2s/include/field_list/field_list_base.hpp
#ifndef _FIELD_LIST_BASE_HPP_
#define _FIELD_LIST_BASE_HPP_
 
namespace s2s {
struct struct_field_list_base {};

template <typename T>
concept field_list_like = std::is_base_of_v<struct_field_list_base, T>;
} /* namespace s2s */

#endif // _FIELD_LIST_BASE_HPP_

// End /home/hari/repos/s2s/include/field_list/field_list_base.hpp

// Begin /home/hari/repos/s2s/include/lib/s2s_traits/type_traits.hpp
#ifndef _S2S_TYPE_TRAITS_HPP_
#define _S2S_TYPE_TRAITS_HPP_
 
 
 
 
 
namespace s2s {
// Arithmetic concept
template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

template <typename T>
concept integral = std::is_integral_v<T>;

template <typename T>
concept floating_point = std::is_floating_point_v<T>;

template <typename T>
concept trivial = floating_point<T> || integral<T>;

template <typename T>
concept unsigned_integral = std::is_integral_v<T> && std::is_unsigned_v<T>;

template <typename T>
struct is_fixed_string;

template <std::size_t N>
struct is_fixed_string<fixed_string<N>> {
  static constexpr bool is_same = true;
};

template <typename T>
struct is_fixed_string {
  static constexpr bool is_same = false;
};

template <typename T>
inline constexpr bool is_fixed_string_v = is_fixed_string<T>::is_same;

template <typename T>
concept fixed_string_like = is_fixed_string_v<T>;

template <typename T>
struct is_fixed_array;

template <typename T, std::size_t N>
  requires (arithmetic<T> || is_fixed_array<T>::is_same)
struct is_fixed_array<std::array<T, N>> {
  static constexpr bool is_same = true;
};

template <typename T>
struct is_fixed_array {
  static constexpr bool is_same = false;
};

template <typename T>
inline constexpr bool is_fixed_array_v = is_fixed_array<T>::is_same;

template <typename T>
concept fixed_array_like = is_fixed_array_v<T>;

template <typename T>
struct is_c_array;

template <typename T, std::size_t N>
  requires (arithmetic<T> || is_c_array<T>::is_same)
struct is_c_array<T[N]> {
  static constexpr bool is_same = true;
};

template <typename T>
struct is_c_array {
  static constexpr bool is_same = false;
};

template <typename T>
inline constexpr bool is_c_array_v = is_c_array<T>::is_same;

// fixed_buffer_like concept
// todo check if md string is ok
template <typename T>
concept fixed_buffer_like = 
  is_fixed_array_v<T> ||
  is_c_array_v<T> ||
  is_fixed_string_v<T>;

struct not_a_vec{};

template <typename T>
struct extract_type_from_vec;

template <typename T>
struct extract_type_from_vec<std::vector<T>> {
  using type = T;
};

template <typename T>
struct extract_type_from_vec {
  using type = not_a_vec;
};

template <typename T>
using extract_type_from_vec_t = typename extract_type_from_vec<T>::type;

template <typename T>
struct is_variant_like;

template <typename T>
struct is_variant_like {
  static constexpr bool res = false;
};

template <typename... ts>
struct is_variant_like<std::variant<ts...>> {
  static constexpr bool res = true;
};

template <typename T>
inline constexpr bool is_variant_like_v = is_variant_like<T>::res;

template <typename T>
concept variant_like = is_variant_like_v<T>;

// todo: add constraints such that user defined optionals can also be used 
template <typename T>
struct is_optional_like;

template <typename T>
struct is_optional_like {
  static inline constexpr bool res = false;
};

template <typename T>
struct is_optional_like<std::optional<T>> {
  static inline constexpr bool res = true;
};

template <typename T>
inline constexpr bool is_optional_like_v = is_optional_like<T>::res;

template <typename T>
concept optional_like = is_optional_like_v<T>;

template <typename T>
struct is_vector_like;

// todo vector of vectors or vector of arrays?
template <typename T>
  requires (arithmetic<T> || is_fixed_array<T>::is_same)
struct is_vector_like<std::vector<T>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_vector_like {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_vector_v = is_vector_like<T>::res;

template <typename T>
concept vector_like = is_vector_v<T>;

template <typename T>
struct is_string_like;

template <>
struct is_string_like<std::string> {
  static constexpr bool res = true;
};

template <typename T>
struct is_string_like {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_string_v = is_string_like<T>::res;

template <typename T>
concept string_like = is_string_v<T>;

template <typename T>
concept field_containable = fixed_buffer_like<T> || arithmetic<T>;

template <typename T>
struct is_array_of_records;

template <field_list_like T, std::size_t N>
struct is_array_of_records<std::array<T, N>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_array_of_records {
  static constexpr bool res = false;
};

template <typename T>
constexpr inline bool is_array_of_records_v = is_array_of_records<T>::res;

template <typename T>
concept array_of_records_like = is_array_of_records_v<T>;

template <typename T>
struct is_vector_of_records;

template <field_list_like T>
struct is_vector_of_records<std::vector<T>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_vector_of_records {
  static constexpr bool res = false;
};

template <typename T>
constexpr inline bool is_vector_of_records_v = is_vector_of_records<T>::res;

template <typename T>
concept vector_of_records_like = is_vector_of_records_v<T>;

struct not_an_array {};

template <typename T>
struct extract_type_from_array;

template <typename T, std::size_t N>
struct extract_type_from_array<std::array<T, N>> {
  using type = T;
};

template <typename T>
struct extract_type_from_array {
  using type = not_an_array;
};

template <typename T>
using extract_type_from_array_v = extract_type_from_array<T>::type;

template <typename T>
struct extract_size_from_array;

template <typename T, std::size_t N>
struct extract_size_from_array<std::array<T, N>> {
  static constexpr auto value = N;
};

template <typename T>
inline constexpr std::size_t extract_size_from_array_v = extract_size_from_array<T>::value;

template <typename T>
concept variable_sized_buffer_like = vector_like<T> || string_like<T>;

template <typename T>
concept constant_sized_like = fixed_buffer_like<T> || trivial<T>;

template <typename T>
concept buffer_like = fixed_buffer_like<T> || variable_sized_buffer_like<T>;
} /* namespace s2s */

#endif // _S2S_TYPE_TRAITS_HPP_

// End /home/hari/repos/s2s/include/lib/s2s_traits/type_traits.hpp

// Begin /home/hari/repos/s2s/include/field_validation/field_value_constraints.hpp
#ifndef _FIELD_VALUE_CONSTRAINTS_HPP_
#define _FIELD_VALUE_CONSTRAINTS_HPP_ 
 
 
 
 
 
 
namespace s2s {
// Concept for strict callable
template <typename T, typename Arg>
concept strict_callable = requires(T t, Arg arg) {
  { t(arg) } -> std::convertible_to<bool>;
} && std::is_same_v<T, typename std::remove_cvref_t<Arg>>;


// Concepts
template <typename T>
concept equality_comparable = requires(T a, T b) {
    { a == b } -> std::same_as<bool>;
    { a != b } -> std::same_as<bool>;
} && !std::is_floating_point_v<T>;

template <typename T>
concept comparable = requires(T a, T b) {
    { a < b } -> std::same_as<bool>;
    { a > b } -> std::same_as<bool>;
    { a <= b } -> std::same_as<bool>;
    { a >= b } -> std::same_as<bool>;
};

template <typename T>
concept inequality_comparable = comparable<T> && 
    (std::is_integral_v<T> || is_fixed_string_v<T>);

// Structs for predefined constraints
template <equality_comparable T>
struct eq {
  T v;
  constexpr eq(T value) : v(value) {}
  constexpr eq() : v{} {}
  constexpr bool operator()(const T& actual_v) const { return actual_v == v; }
};

template <equality_comparable T>
struct neq {
  T v;
  constexpr neq(T value) : v(value) {}
  constexpr neq() : v{} {}
  constexpr bool operator()(const T& actual_v) const { return actual_v != v; }
};

template <comparable T>
struct lt {
  T v;
  constexpr lt(T value) : v(value) {}
  constexpr lt() : v{} {}
  constexpr bool operator()(const T& actual_v) const { return actual_v < v; }
};

template <comparable T>
struct gt {
  T v;
  constexpr gt(T value) : v(value) {}
  constexpr gt() : v{} {}
  constexpr bool operator()(const T& actual_v) const { return actual_v > v; }
};

template <typename T>
struct lte {
  T v;
  constexpr lte(T value) : v(value) {}
  constexpr lte() : v{} {}
  constexpr bool operator()(const T& actual_v) const { return actual_v <= v; }
};

template <inequality_comparable T>
struct gte {
  T v;
  constexpr gte(T value) : v(value) {}
  constexpr gte() : v{} {}
  constexpr bool operator()(const T& actual_v) const { return actual_v >= v; }
};

template <typename T>
struct no_constraint {
  constexpr bool operator()([[maybe_unused]] const T& actual_v) const { 
    return true; 
  }
};

template <typename T, typename... Ts>
  requires (typelist::all_are_same_v<typelist::list<T, Ts...>>)
struct any_of {
  std::array<T, 1 + sizeof...(Ts)> possible_values;

  constexpr any_of(T first, Ts... rest) : possible_values{first, rest...} {}

  constexpr bool operator()(const T& actual_v) const {
    return std::find(possible_values.begin(), 
                     possible_values.end(), 
                     actual_v) != possible_values.end();
  }
};

// Range struct
template <typename T>
struct range {
  T a;
  T b;

  constexpr range(T value1, T value2) : a(value1), b(value2) {
    static_assert(value1 < value2, "Range start must be less than range end");
  }
};

// CTAD for range
template <typename T>
range(T, T) -> range<T>;

// Struct to check if a value is in any of the open intervals
template <typename t, typename... ts>
  requires (typelist::all_are_same_v<typelist::list<ts...>>)
struct is_in_open_range {
  std::array<range<t>, 1 + sizeof...(ts)> open_ranges;

  constexpr is_in_open_range(range<t> r, range<ts>... rs) : open_ranges{r, rs...} {
    std::sort(open_ranges.begin(), open_ranges.end(), [](const range<t>& r1, const range<t>& r2) {
      return r1.a < r2.a;
    });
  }

  constexpr bool operator()(const t& value) const {
    auto it = std::lower_bound(open_ranges.begin(), open_ranges.end(), value, [](const range<t>& r, const t& v) {
      return r.b < v;
    });
    if (it != open_ranges.begin() && (it == open_ranges.end() || it->a > value)) {
      --it;
    }
    return it != open_ranges.end() && it->a < value && value < it->b;
  }
};

// Struct to check if a value is in any of the closed intervals
template <typename T, std::size_t N>
struct is_in_closed_range {
  std::array<range<T>, N> closed_ranges;

  constexpr is_in_closed_range(std::array<range<T>, N> ranges) : closed_ranges(ranges) {
    std::sort(closed_ranges.begin(), closed_ranges.end(), [](const range<T>& r1, const range<T>& r2) {
      return r1.a < r2.a;
    });
  }

  constexpr bool operator()(const T& value) const {
    auto it = std::lower_bound(closed_ranges.begin(), closed_ranges.end(), value, [](const range<T>& r, const T& v) {
      return r.b < v;
    });
    if (it != closed_ranges.begin() && (it == closed_ranges.end() || it->a > value)) {
      --it;
    }
    return it != closed_ranges.end() && it->a <= value && value <= it->b;
  }
};

// CTAD (Class Template Argument Deduction) guides
template <typename T>
eq(T) -> eq<T>;

template <typename T>
neq(T) -> neq<T>;

template <typename T>
lt(T) -> lt<T>;

template <typename T>
gt(T) -> gt<T>;

template <typename T>
lte(T) -> lte<T>;

template <typename T>
gte(T) -> gte<T>;

template <typename t, typename... ts> 
any_of(t, ts...) -> any_of<t, ts...>;

template <typename t, typename... ts> 
is_in_open_range(range<t>, range<ts>...) -> is_in_open_range<t, ts...>;

template <typename T, std::size_t N>
is_in_closed_range(std::array<range<T>, N>) -> is_in_closed_range<T, N>;
}

#endif // _FIELD_VALUE_CONSTRAINTS_HPP_

// End /home/hari/repos/s2s/include/field_validation/field_value_constraints.hpp

// Begin /home/hari/repos/s2s/include/field/field.hpp
#ifndef _FIELD_HPP_
#define _FIELD_HPP_
 
 
 
namespace s2s {
template <fixed_string id,
          typename T,
          typename size_type,
          auto constraint_on_value>
struct field {
  using field_type = T;
  using field_size = size_type;

  static constexpr auto field_id = id;
  static constexpr auto constraint_checker = constraint_on_value;
  field_type value{};
};

template <typename T>
struct to_optional_field;

template <fixed_string id, typename T, typename size_type, auto constraint_on_value>
struct to_optional_field<field<id, T, size_type, constraint_on_value>> {
  using res = field<id, std::optional<T>, size_type, no_constraint<std::optional<T>>{}>;
};

template <typename T>
using to_optional_field_v = to_optional_field<T>::res;

template <typename T>
struct no_variance_field;

template <fixed_string id,
          typename T,
          typename size_type>
struct no_variance_field<field<id, T, size_type, no_constraint<T>{}>> {
  static constexpr bool res = true;
};

template <typename T>
struct no_variance_field {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool no_variance_field_v = no_variance_field<T>::res;

template <typename T>
concept no_variance_field_like = no_variance_field_v<T>;

template <typename... choices>
struct field_choice_list {};

template <fixed_string id, typename... args>
struct to_field_choices;

template <fixed_string id, typename T, typename field_size>
struct to_field_choice {
  using field_choice = field<id, T, field_size, no_constraint<T>{}>;
};

template <fixed_string id, typename T, typename field_size>
using to_field_choice_v = to_field_choice<id, T, field_size>::field_choice;

template <fixed_string id, typename... types, typename... sizes>
struct to_field_choices<id, std::variant<types...>, field_size<size_choices<sizes...>>> {
  using choices = field_choice_list<to_field_choice_v<id, types, sizes>...>;
};


template <typename arg>
struct are_unique_types;

template <typename head>
struct are_unique_types<field_choice_list<head>> {
  static constexpr bool res = true;
};

template <typename head, typename neck, typename... tail>
struct are_unique_types<field_choice_list<head, neck, tail...>> {
  constexpr static bool res =
    (!std::is_same_v<head, neck> && ... && (!std::is_same_v<head, tail>)) &&
    are_unique_types<field_choice_list<neck, tail...>>::res;
};


template <typename choice_list>
inline constexpr bool are_unique_types_v = are_unique_types<choice_list>::res;

template <no_variance_field_like base_field,
          typename present_only_if,
          typename optional = to_optional_field_v<base_field>>
class maybe_field : public optional
{
public:
  using field_base_type = base_field;
  using field_presence_checker = present_only_if;
};


template <fixed_string id, typename type_deducer, auto type_choices>
  requires are_unique_types_v<
    typename to_field_choices<
      id, 
      typename type_deducer::variant, 
      typename type_deducer::sizes
    >::choices
  >
struct union_field: public 
    field<
      id, 
      typename type_deducer::variant, 
      size_dont_care, 
      no_constraint<typename type_deducer::variant>{}
    > 
{
  using type_deduction_guide = type_deducer;
  static constexpr auto variant_size = std::variant_size_v<typename type_deducer::variant>;
  using field_choices = typename to_field_choices<
      id, 
      typename type_deducer::variant, 
      typename type_deducer::sizes
    >::choices;
};

} /* namespace s2s */

#endif // _FIELD_HPP_

// End /home/hari/repos/s2s/include/field/field.hpp

// Begin /home/hari/repos/s2s/include/field/field_traits.hpp
#ifndef _FIELD_TRAITS_HPP_
#define _FIELD_TRAITS_HPP_
 
 
 
namespace s2s {
template <typename T>
struct is_fixed_sized_field;

// Specialization for field with fixed_size_like size
template <fixed_string id, field_containable T, fixed_size_like size, auto constraint_on_value>
struct is_fixed_sized_field<field<id, T, size, constraint_on_value>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_fixed_sized_field {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_fixed_sized_field_v = is_fixed_sized_field<T>::res;

template <typename T>
concept fixed_sized_field_like = is_fixed_sized_field_v<T>;

template <typename T>
struct is_array_of_record_field;

template <fixed_string id, field_list_like T, std::size_t N, typename size, auto constraint_on_value>
struct is_array_of_record_field<field<id, std::array<T, N>, size, constraint_on_value>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_array_of_record_field {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_array_of_record_field_v = is_array_of_record_field<T>::res;

template <typename T>
concept array_of_record_field_like = is_array_of_record_field_v<T>;


template <typename T>
struct is_variable_sized_field;

// Specialization for field with variable_size_like size
template <fixed_string id, variable_sized_buffer_like T, variable_size_like size, auto constraint_on_value>
struct is_variable_sized_field<field<id, T, size, constraint_on_value>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_variable_sized_field {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_variable_sized_field_v = is_variable_sized_field<T>::res;

// Concept for variable_sized_field_like
template <typename T>
concept variable_sized_field_like = is_variable_sized_field_v<T>;

template <typename T>
struct is_vector_of_record_field;

template <fixed_string id, field_list_like T, typename size, auto constraint_on_value>
struct is_vector_of_record_field<field<id, std::vector<T>, size, constraint_on_value>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_vector_of_record_field {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_vector_of_record_field_v = is_vector_of_record_field<T>::res;

template <typename T>
concept vector_of_record_field_like = is_vector_of_record_field_v<T>;

template <typename T>
struct is_struct_field;

// Specialization for field with variable_size_like size
template <fixed_string id, field_list_like T, typename size, auto constraint_on_value>
struct is_struct_field<field<id, T, size, constraint_on_value>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_struct_field {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_struct_field_v = is_struct_field<T>::res;

// Concept for variable_sized_field_like
template <typename T>
concept struct_field_like = is_struct_field_v<T>;


template <typename T>
struct is_optional_field;

// Specialization for maybe_field with a field
template <fixed_string id, 
          typename T, 
          typename size, 
          auto constraint, 
          typename present_only_if, 
          typename optional>
struct is_optional_field<
    maybe_field<field<id, T, size, constraint>, present_only_if, optional>
  >
{
  static constexpr bool res = true;
};

template <typename T>
struct is_optional_field {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_optional_field_v = is_optional_field<T>::res;

// Concept for optional_field_like
template <typename T>
concept optional_field_like = is_optional_field_v<T>;


template <typename T>
struct is_union_field;

template <fixed_string id, typename type_deducer, auto field_choices>
struct is_union_field<
    union_field<id, type_deducer, field_choices>
  > 
{
  static constexpr bool res = true;
};

template <typename T>
struct is_union_field {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_union_field_v = is_union_field<T>::res;

template <typename T>
concept union_field_like = is_union_field_v<T>;

template <typename T>
concept field_like = fixed_sized_field_like<T> || 
                     variable_sized_field_like<T> ||
                     array_of_record_field_like<T> ||
                     vector_of_record_field_like<T> ||
                     struct_field_like<T> || 
                     optional_field_like<T> || 
                     union_field_like<T>;
} /* namespace s2s */

#endif /*_FIELD_TRAITS_HPP_*/

// End /home/hari/repos/s2s/include/field/field_traits.hpp

// Begin /home/hari/repos/s2s/include/field_list/field_list_metafunctions.hpp
#ifndef _FIELD_LIST_METAFUNCTIONS_HPP_
#define _FIELD_LIST_METAFUNCTIONS_HPP_

// status: might be deprecated, use the file for reorganising components

#endif // _FIELD_LIST_METAFUNCTIONS_HPP_

// End /home/hari/repos/s2s/include/field_list/field_list_metafunctions.hpp

// Begin /home/hari/repos/s2s/include/field_list/field_list.hpp
#ifndef _FIELD_LIST_HPP_
#define _FIELD_LIST_HPP_

// status: rename to impl
 
 
 
 
 
 
 
 
namespace s2s {

template <typename list_metadata>
constexpr auto lookup_field(std::string_view field_name) -> std::optional<field_type_info>;

template <typename metadata, typename... fields>
struct struct_field_list_impl : struct_field_list_base, fields... {
  using list_metadata = metadata;

  struct_field_list_impl() = default;

  template <typename field_accessor>
    // todo custom optional to use as nttp for cleaner template
    requires (lookup_field<list_metadata>(as_sv(field_accessor::field_id)) != std::nullopt)
  constexpr auto& operator[](field_accessor)  {
    constexpr auto res = lookup_field<list_metadata>(as_sv(field_accessor::field_id));
    using field_type_ref = meta::type_of<res->id>&;
    return static_cast<field_type_ref>(*this).value;
  }

  template <typename field_accessor>
    requires (lookup_field<list_metadata>(as_sv(field_accessor::field_id)) != std::nullopt)
  constexpr const auto& operator[](field_accessor) const {
    constexpr auto res = lookup_field<list_metadata>(as_sv(field_accessor::field_id));
    using field_type_cref = const meta::type_of<res->id>&;
    return static_cast<field_type_cref>(*this).value;
  }
};
} /* namespace s2s */


#endif // _FIELD_LIST_HPP_

// End /home/hari/repos/s2s/include/field_list/field_list.hpp

// Begin /home/hari/repos/s2s/include/field_compute/computation_from_fields.hpp
#ifndef _COMPUTATION_FROM_FIELDS_HPP_
#define _COMPUTATION_FROM_FIELDS_HPP_
 
using namespace s2s_literals;

namespace s2s {
template <auto callable, typename return_type, typename struct_field_list_t, field_name_list field_list>
struct is_invocable;

template <auto callable, 
          typename return_type, 
          typename struct_field_list_t, 
          fixed_string... req_fields>
struct is_invocable<callable, 
                    return_type, 
                    struct_field_list_t, 
                    fixed_string_list<req_fields...>> {
  static constexpr bool res = 
      std::is_invocable_r_v<
        return_type, 
        decltype(callable),
        decltype(struct_field_list_t{}[field_accessor<req_fields>{}])...
      >;
};

template <auto Func, 
          typename R, 
          typename struct_fields, 
          typename req_fields>
concept can_eval_R_from_fields = 
  is_invocable<
    Func, 
    R, 
    struct_fields, 
    req_fields
  >::res;


template <auto callable, typename R, field_name_list Fs>
struct compute;

// todo: static_vector over fixed_string list?
template <auto callable, typename R, fixed_string... req_fields>
struct compute<callable, R, fixed_string_list<req_fields...>>{
  template <typename... fields>
    requires (can_eval_R_from_fields<
                callable, 
                R,
                struct_field_list_impl<fields...>,
                fixed_string_list<req_fields...>>)
  constexpr auto operator()(const struct_field_list_impl<fields...>& flist) const -> R {
    return callable(flist[field_accessor<req_fields>{}]...);
  }
};

template <auto callable, field_name_list req_fields>
using eval_bool_from_fields = compute<callable, bool, req_fields>;
template <auto callable, field_name_list req_fields>
using predicate = compute<callable, bool, req_fields>;
template <auto callable, field_name_list req_fields>
using eval_size_from_fields = compute<callable, std::size_t, req_fields>;
template <auto callable, field_name_list req_fields>
using parse_if = eval_bool_from_fields<callable, req_fields>;

template <typename T>
struct is_compute_like;

template <auto callable, typename R, fixed_string... req_fields>
struct is_compute_like<compute<callable, R, with_fields<req_fields...>>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_compute_like {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_compute_like_v = is_compute_like<T>::res;

template <typename T>
struct is_eval_bool_from_fields;

template <auto callable, fixed_string... req_fields>
struct is_eval_bool_from_fields<compute<callable, bool, with_fields<req_fields...>>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_eval_bool_from_fields {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_eval_bool_from_fields_v = is_eval_bool_from_fields<T>::res;

template <typename T>
struct is_eval_size_from_fields;

template <auto callable, fixed_string... req_fields>
struct is_eval_size_from_fields<compute<callable, std::size_t, with_fields<req_fields...>>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_eval_size_from_fields {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_eval_size_from_fields_v = is_eval_size_from_fields<T>::res;
} /* namespace s2s */

#endif // _COMPUTATION_FROM_FIELDS_HPP_

// End /home/hari/repos/s2s/include/field_compute/computation_from_fields.hpp

// Begin /home/hari/repos/s2s/include/field_size/field_size_deduce.hpp
#ifndef _FIELD_SIZE_DEDUCE_HPP_
#define _FIELD_SIZE_DEDUCE_HPP_
 
 
 
 
namespace s2s {
template <typename T>
struct deduce_field_size;

template <std::size_t N>
struct deduce_field_size<field_size<fixed<N>>> {
  using field_size_type = field_size<fixed<N>>;

  constexpr auto operator()() -> std::size_t {
    return field_size_type::size_type_t::count;
  }
};

template <fixed_string id>
struct deduce_field_size<field_size<field_accessor<id>>> {
  using field_size_type = field_accessor<id>;
  
  template <typename... fields>
  constexpr auto operator()(const struct_field_list_impl<fields...>& struct_fields) -> std::size_t {
    return struct_fields[field_size_type{}];
  }
};

template <auto callable, field_name_list req_fields>
struct deduce_field_size<field_size<size_from_fields<callable, req_fields>>> {
  using field_size_type = compute<callable, std::size_t, req_fields>;
  template <typename... fields>
  constexpr auto operator()(const struct_field_list_impl<fields...>& struct_fields) -> std::size_t {
    return field_size_type{}(struct_fields);
  }
};
} /* namespace s2s */


#endif // _FIELD_SIZE_DEDUCE_HPP_

// End /home/hari/repos/s2s/include/field_size/field_size_deduce.hpp

// Begin /home/hari/repos/s2s/include/type_deduction/type_deduction_tags.hpp
#ifndef _TYPE_DEDUCTION_TAGS_
#define _TYPE_DEDUCTION_TAGS_
 
 
 
 
namespace s2s {
template <trivial T, fixed_size_like S>
  requires (deduce_field_size<S>{}() <= sizeof(T))
struct as_trivial {
  using type = T;
  using size = S;
};

template <field_list_like T>
struct as_struct {
  using type = T;
  using size = field_size<size_dont_care>;
};

// todo how to handle array of array
template <trivial T, std::size_t N> 
struct as_fixed_arr {
  using type = std::array<T, N>;
  using size = field_size<fixed<N * sizeof(T)>>;
};

template <std::size_t N> 
struct as_fixed_string {
  using type = fixed_string<N>;
  using size = field_size<fixed<N + 1>>;
};

template <trivial T, variable_size_like S> 
struct as_vec {
  using type = std::vector<T>;
  using size = S;
};

template <variable_size_like S> 
struct as_string {
  using type = std::string;
  using size = S;
};

template <field_list_like T, std::size_t N>
struct as_arr_of_records {
  using type = std::array<T, N>;
  using size = field_size<size_dont_care>;
};

template <field_list_like T, variable_size_like S>
struct as_vec_of_records {
  using type = std::vector<T>;
  using size = S;
};

template <typename T>
struct is_type_tag;

template <typename T, typename size>
struct is_type_tag<as_trivial<T, size>> {
  static constexpr bool res = true;
};

template <typename T, std::size_t size>
struct is_type_tag<as_fixed_arr<T, size>> {
  static constexpr bool res = true;
};

template <std::size_t size>
struct is_type_tag<as_fixed_string<size>> {
  static constexpr bool res = true;
};

template <typename T, typename size>
struct is_type_tag<as_vec<T, size>> {
  static constexpr bool res = true;
};

template <typename size>
struct is_type_tag<as_string<size>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_type_tag<as_struct<T>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_type_tag {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_type_tag_v = is_type_tag<T>::res;

template <typename T>
concept type_tag_like = is_type_tag_v<T>;
} /* namespace s2s */

#endif // _TYPE_DEDUCTION_TAGS_

// End /home/hari/repos/s2s/include/type_deduction/type_deduction_tags.hpp

// Begin /home/hari/repos/s2s/include/type_deduction/type_deduction_match_case.hpp
#ifndef _TYPE_DEDUCTION_MATCH_CASE_HPP_
#define _TYPE_DEDUCTION_MATCH_CASE_HPP_
 
namespace s2s {
// todo constrain to data types possible for fields
template <auto v, type_tag_like T>
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
} /* namespace s2s */

#endif // _TYPE_DEDUCTION_MATCH_CASE_HPP_

// End /home/hari/repos/s2s/include/type_deduction/type_deduction_match_case.hpp

// Begin /home/hari/repos/s2s/include/type_deduction/type_deduction_clause.hpp
#ifndef _TYPE_DEDUCTION_CLAUSE_HPP_
#define _TYPE_DEDUCTION_CLAUSE_HPP_
 
 
namespace s2s {

template <typename T>
concept evaluates_to_bool = is_eval_bool_from_fields_v<T>;

template <evaluates_to_bool eval, type_tag_like T>
struct branch {
  static constexpr auto e = eval{};
  using type_tag = T;
};


template <typename T>
struct is_branch;

template <typename T>
struct is_branch {
  static constexpr bool res = false;
};

template <typename eval, typename T>
struct is_branch<branch<eval, T>> {
  static constexpr bool res = true;
};

template <typename T>
inline constexpr bool is_branch_v = is_branch<T>::res;

template <typename T>
concept branch_like = is_branch_v<T>;
} /* namespace s2s */

#endif // _TYPE_DEDUCTION_CLAUSE_HPP_

// End /home/hari/repos/s2s/include/type_deduction/type_deduction_clause.hpp

// Begin /home/hari/repos/s2s/include/type_deduction/type_deduction_helper.hpp
#ifndef _TYPE_DEDUCTION_HELPER_HPP_
#define _TYPE_DEDUCTION_HELPER_HPP_
 
 
namespace s2s {
using type_deduction_idx = std::optional<std::size_t>;
using type_deduction_res = std::expected<std::size_t, error_reason>;

constexpr auto operator|(const type_deduction_idx& res, auto&& callable) -> type_deduction_idx {
  return res ? res : callable();
}

template <typename T>
concept type_condition_like = match_case_like<T> || branch_like<T>;

template <type_condition_like match_case>
struct type_from_type_condition;

template <type_condition_like match_case>
struct type_from_type_condition {
  using type = typename match_case::type_tag::type;
};

template <type_condition_like match_case>
struct size_from_type_condition;

template <type_condition_like match_case>
struct size_from_type_condition {
  using size = typename match_case::type_tag::size;
};

template <typename T>
using type_from_type_condition_v = type_from_type_condition<T>::type;

template <typename T>
using size_from_type_condition_v = size_from_type_condition<T>::size;

template <type_condition_like... cases>
struct variant_from_type_conditions {
  using variant = std::variant<type_from_type_condition_v<cases>...>;
};

template <type_condition_like... cases>
using variant_from_type_conditions_v = variant_from_type_conditions<cases...>::variant;

template <type_condition_like... cases>
struct size_choices_from_type_conditions {
  using choices = field_size<size_choices<size_from_type_condition_v<cases>...>>;
};

template <type_condition_like... cases>
using size_choices_from_type_conditions_v = size_choices_from_type_conditions<cases...>::choices;
} /* namespace s2s */ 

#endif // _TYPE_DEDUCTION_HELPER_HPP_

// End /home/hari/repos/s2s/include/type_deduction/type_deduction_helper.hpp

// Begin /home/hari/repos/s2s/include/type_deduction/type_deduction_ladder.hpp
#ifndef _TYPE_DEDUCTION_LADDER_HPP_
#define _TYPE_DEDUCTION_LADDER_HPP_
 
namespace s2s {
template <branch_like... branches>
  requires (sizeof...(branches) > 0)
struct type_if_else;

template <std::size_t idx, typename branch>
struct type_if_else_impl;

template <std::size_t idx, typename branch>
struct type_if_else_impl {
  template <typename... fields>
  constexpr auto operator()(const struct_field_list_impl<fields...>& field_list) const -> 
    type_deduction_idx
  {
    if(branch::e(field_list)) return idx;
    return std::nullopt;
  }
};

template <typename... branches>
struct type_if_else_helper {
  template <typename... fields, std::size_t... idx>
  constexpr auto operator()(
    const struct_field_list_impl<fields...>& field_list, 
    const std::index_sequence<idx...>&) const 
  -> type_deduction_idx
  {
    type_deduction_idx pipeline_seed = std::nullopt;
    return (
      pipeline_seed |
      ... |
      [&]() { return type_if_else_impl<idx, branches>{}(field_list); }
    );
  }
};


template <branch_like... branches>
  requires (sizeof...(branches) > 0)
struct type_if_else {
  using variant = variant_from_type_conditions_v<branches...>;
  using sizes = size_choices_from_type_conditions_v<branches...>;

  template <typename... fields>
  constexpr auto operator()(const struct_field_list_impl<fields...>& field_list) const -> 
    type_deduction_res
  {
    auto res = type_if_else_helper<branches...>{}(
      field_list,
      std::make_index_sequence<sizeof...(branches)>{}
    );
    if(!res)
      return std::unexpected(error_reason::type_deduction_failure);
    return std::expected<std::size_t, error_reason>(*res);
  }
};

template <typename T>
struct is_type_if_else;

template <typename T>
struct is_type_if_else {
  static constexpr bool res = false;
};

template <branch_like branch_head, branch_like... branch_tail>
struct is_type_if_else<type_if_else<branch_head, branch_tail...>> {
  static constexpr bool res = true;
};

template <typename T>
static constexpr bool is_type_if_else_v = is_type_if_else<T>::res;

template <typename T>
concept type_if_else_like = is_type_if_else_v<T>;
} /* namespace s2s */

#endif // _TYPE_LADDER_HPP_

// End /home/hari/repos/s2s/include/type_deduction/type_deduction_ladder.hpp

// Begin /home/hari/repos/s2s/include/type_deduction/type_deduction_switch.hpp
#ifndef _TYPE_DEDUCTION_SWITCH_HPP_
#define _TYPE_DEDUCTION_SWITCH_HPP_
 
namespace s2s {

// todo check if case and eval result match in terms of types
template <std::size_t idx, typename match_case>
struct type_switch_impl {
  constexpr auto operator()(const auto& v) const -> 
    type_deduction_idx
  {
    if(v == match_case::value) return idx;
    else return std::nullopt;
  }
};

template <typename... branches>
struct type_switch_helper {
  template <std::size_t... idx>
  constexpr auto operator()(
    const auto& v, 
    const std::index_sequence<idx...>&) const 
  -> type_deduction_idx 
  {
    type_deduction_idx pipeline_seed = std::nullopt;
    return (
      pipeline_seed |
      ... |
      [&]() { return type_switch_impl<idx, branches>{}(v); }
    );
  }
};

template <match_case_like... cases>
  requires (sizeof...(cases) > 0)
struct type_switch {
  using variant = variant_from_type_conditions_v<cases...>;
  using sizes = size_choices_from_type_conditions_v<cases...>;

  template <typename... fields>
  constexpr auto operator()(const auto& v) const -> 
    type_deduction_res
  {
    auto res =
      type_switch_helper<cases...>{}(
        v, std::make_index_sequence<sizeof...(cases)>{}
      );
    if(!res)
      return std::unexpected(error_reason::type_deduction_failure);
    return std::expected<std::size_t, error_reason>(*res);
  } 
};

template <typename T>
struct is_type_switch;

template <typename T>
struct is_type_switch {
  static constexpr bool res = false;
};

template <match_case_like case_head, match_case_like... case_tail>
struct is_type_switch<type_switch<case_head, case_tail...>> {
  static constexpr bool res = true;
};

template <typename T>
static constexpr bool is_type_switch_v = is_type_switch<T>::res;

template <typename T>
concept type_switch_like = is_type_switch_v<T>;

} /* namespace s2s */


#endif // _TYPE_SWITCH_HPP_

// End /home/hari/repos/s2s/include/type_deduction/type_deduction_switch.hpp

// Begin /home/hari/repos/s2s/include/type_deduction/type_deduction.hpp
#ifndef _TYPE_DEDUCTION_HPP_
#define _TYPE_DEDUCTION_HPP_
 
namespace s2s {
template <typename... Args>
struct type;


template <fixed_string id>
using match_field = field_accessor<id>;

// todo constraints compute like
template <typename eval_expression, typename tswitch>
struct type<eval_expression, tswitch> {
  using expression = eval_expression;
  using type_switch = tswitch;
  using variant = tswitch::variant;
  using sizes = tswitch::sizes;

  template <typename... fields>
  constexpr auto operator()(const struct_field_list_impl<fields...>& sfl)
    -> std::expected<std::size_t, error_reason> const {
    return type_switch{}(eval_expression{}(sfl)); 
  }
};

template <fixed_string id, typename tswitch>
struct type<match_field<id>, tswitch> {
  using type_switch = tswitch;
  using variant = tswitch::variant;
  using sizes = tswitch::sizes;

  template <typename... fields>
  constexpr auto operator()(const struct_field_list_impl<fields...>& sfl)
    -> std::expected<std::size_t, error_reason> const {
    return type_switch{}(sfl[field_accessor<id>{}]); 
  }
};

// todo constraints
template <typename tladder>
struct type<tladder> {
  using type_ladder = tladder;
  using variant = tladder::variant;
  using sizes = tladder::sizes;

  template <typename... fields>
  constexpr auto operator()(const struct_field_list_impl<fields...>& sfl)
    -> std::expected<std::size_t, error_reason> const {
    return type_ladder{}(sfl);
  }
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


template <typename T>
struct is_type_deduction;

template <>
struct is_type_deduction<no_type_deduction> {
  static constexpr bool res = true;
};

template <typename eval_expression, typename tswitch>
struct is_type_deduction<type<eval_expression, tswitch>> {
  static constexpr bool res = 
    is_compute_like_v<eval_expression> && 
    type_switch_like<tswitch>;
};

template <fixed_string id, typename tswitch>
struct is_type_deduction<type<match_field<id>, tswitch>> {
  static constexpr bool res = type_switch_like<tswitch>;
};

template <typename tladder>
struct is_type_deduction<type<tladder>> {
  static constexpr bool res = type_if_else_like<tladder>;
};

template <typename T>
struct is_type_deduction {
  static constexpr bool res = false;
};

template <typename T>
static constexpr bool is_type_deduction_v = is_type_deduction<T>::res;

template <typename T>
concept type_deduction_like = is_type_deduction_v<T>;
} /* namespace s2s */


#endif // _TYPE_DEDUCTION_HPP_

// End /home/hari/repos/s2s/include/type_deduction/type_deduction.hpp

// Begin /home/hari/repos/s2s/include/field_list/field_list_metadata.hpp
#ifndef _FIELD_LIST_METADATA_HPP_
#define _FIELD_LIST_METADATA_HPP_
 
 
 
 
 
 
 
 
 
 
namespace s2s {
// todo fix these numbers and possibly generate them
static inline constexpr std::size_t max_dep_count_per_field = 8;
static inline constexpr std::size_t max_union_choices = 8;
static inline constexpr std::size_t max_dep_count_per_struct = max_dep_count_per_field * max_union_choices;
static inline constexpr std::size_t max_field_count = 256;


using sv = std::string_view;
using dep_vec = static_vector<sv, max_dep_count_per_struct>;
using field_table_t = static_map<sv, field_type_info, max_field_count>;
using dependency_table_t = static_map<sv, static_vector<sv, max_dep_count_per_struct>, max_field_count>;

// extract dependencies metafunction
template <typename T>
struct extract_length_dependencies;

template <fixed_string id, typename T, fixed_size_like size, auto constraint>
struct extract_length_dependencies<
  field<id, T, size, constraint>
>
{
  static constexpr auto value = static_vector<sv, max_dep_count_per_struct>();
};

template <fixed_string id, typename T, size_dont_care_like size, auto constraint>
struct extract_length_dependencies<
  field<id, T, size, constraint>
>
{
  static constexpr auto value = static_vector<sv, max_dep_count_per_struct>();
};

template <fixed_string id, typename T, fixed_string len_source, auto constraint>
struct extract_length_dependencies<
  field<id, T, field_size<len_from_field<len_source>>, constraint>
>
{
  static constexpr auto value = static_vector<sv, max_dep_count_per_struct>(as_sv(len_source));
};

template <fixed_string id, typename T, auto callable, auto constraint, fixed_string... req_fields>
struct extract_length_dependencies<
  field<id, T, field_size<len_from_fields<callable, fixed_string_list<req_fields...>>>, constraint>
>
{
  static constexpr auto value = static_vector<sv, max_dep_count_per_struct>(as_sv(req_fields)...);
};

template <fixed_string id, typename T, typename size, auto constraint, 
          typename present_only_if, typename optional>
struct extract_length_dependencies<
  maybe_field<field<id, T, size, constraint>, present_only_if, optional>
>
{
  using f = field<id, T, size, constraint>;
  static constexpr auto value = extract_length_dependencies<f>::value;
};

template <std::size_t N>
constexpr auto flatten(const dep_vec (&vecs)[N]) -> dep_vec {
  dep_vec vec;
  for(auto i = 0u; i < N; i++) {
    for(auto& elem: vecs[i]) {
      vec.push_back(elem);
    }
  }
  return vec;
}

template <typename... Ts>
struct extract_length_dependencies_from_field_choices;

template <typename... Ts>
struct extract_length_dependencies_from_field_choices<field_choice_list<Ts...>>{
  static constexpr dep_vec deps[64] = {dep_vec(extract_length_dependencies<Ts>::value)...};
  static constexpr auto value = flatten(deps);
};

template <typename... Ts>
inline constexpr auto extract_length_dependencies_from_field_choices_v = extract_length_dependencies_from_field_choices<Ts...>::value;

template <fixed_string id, typename type_deducer, auto choices>
struct extract_length_dependencies<
  union_field<id, type_deducer, choices>
> 
{
  using field = union_field<id, type_deducer, choices>;
  using field_choices = typename field::field_choices;
  static constexpr auto value = extract_length_dependencies_from_field_choices_v<field_choices>;
};
 

template <typename T>
inline constexpr auto extract_length_dependencies_v = extract_length_dependencies<T>::value;


template <auto callable, typename R, field_name_list Fs>
struct compute;

template <typename T>
struct extract_parse_dependencies;

template <typename T>
struct extract_parse_dependencies {
  static constexpr auto value = static_vector<sv, max_dep_count_per_struct>();
};

template <fixed_string id, typename T, typename size, auto constraint, 
          auto callable, fixed_string... req_fields, typename optional>
struct extract_parse_dependencies<
  maybe_field<field<id, T, size, constraint>, compute<callable, bool, fixed_string_list<req_fields...>>, optional>
>
{
  static constexpr auto value = static_vector<sv, max_dep_count_per_struct>(as_sv(req_fields)...);
};

template <typename T>
inline constexpr auto extract_parse_dependencies_v = extract_parse_dependencies<T>::value;


template <typename T>
struct extract_type_deduction_dependencies;

template <typename T>
struct extract_type_deduction_dependencies {
  static constexpr auto value = static_vector<sv, max_dep_count_per_struct>();
};

template <fixed_string id, fixed_string matched_id, type_switch_like type_switch, auto field_choices>
struct extract_type_deduction_dependencies<
  union_field<
    id,
    type<match_field<matched_id>, type_switch>,
    field_choices
  >
> 
{
  static constexpr auto value = dep_vec(as_sv(matched_id));
};

template <fixed_string id, auto callable, typename R, fixed_string... req_fields, type_switch_like type_switch, auto field_choices>
struct extract_type_deduction_dependencies<
  union_field<
    id,
    type<compute<callable, R, fixed_string_list<req_fields...>>, type_switch>,
    field_choices
  >
> 
{
  static constexpr auto value = dep_vec(as_sv(req_fields)...);
};

template <typename T>
struct extract_req_fields_from_clause;

template <auto callable, fixed_string... req_fields, type_tag_like T>
struct extract_req_fields_from_clause<
  branch<
    compute<callable, bool, fixed_string_list<req_fields...>>,
    T
  >
>
{
  static constexpr auto value = dep_vec(as_sv(req_fields)...);
};

template <typename T>
inline constexpr auto extract_req_fields_from_clause_v = extract_req_fields_from_clause<T>::value;

// todo better implementation
constexpr auto remove_duplicates(const dep_vec& vec) -> dep_vec {
  static_set<sv, max_dep_count_per_struct> set(vec);
  dep_vec res;
  for(auto item: set)
    res.push_back(item);
  return res;
}

// template<typename...>... typename clauses?
template <fixed_string id, auto field_choices, typename... clauses>
struct extract_type_deduction_dependencies<
  union_field<
    id,
    type<type_if_else<clauses...>>,
    field_choices
  >
> 
{
  static constexpr dep_vec deps[64] = {dep_vec(extract_req_fields_from_clause_v<clauses>)...};
  static constexpr auto flat_values = flatten(deps);
  static constexpr auto value = remove_duplicates(flat_values);
};

template <typename T>
inline constexpr auto extract_type_deduction_dependencies_v = extract_type_deduction_dependencies<T>::value;

template <typename... fields>
struct field_list_metadata {
  template <std::size_t... Is>
  static constexpr auto generate_field_table(std::index_sequence<Is...>) {
    return static_map<sv, field_type_info, max_field_count>(
      {
        {as_sv(fields::field_id), field_type_info(meta::type_id<fields>, Is)}...
      }
    );
  }

  static constexpr auto generate_len_dep_table() {
    return static_map<sv, static_vector<sv, max_dep_count_per_struct>, max_field_count>(
      {
        {as_sv(fields::field_id), extract_length_dependencies_v<fields>}...
      }
    );
  }

  static constexpr auto generate_parse_dependency_table() {
    return static_map<sv, static_vector<sv, max_dep_count_per_struct>, max_field_count>(
      {
        {as_sv(fields::field_id), extract_parse_dependencies_v<fields>}...
      }
    );
  }

  static constexpr auto generate_type_deduction_dependency_table() {
    return static_map<sv, static_vector<sv, max_dep_count_per_struct>, max_field_count>(
      {
        {as_sv(fields::field_id), extract_type_deduction_dependencies_v<fields>}...
      }
    );
  }

  static constexpr field_table_t field_table = generate_field_table(std::make_index_sequence<sizeof...(fields)>{});
  static constexpr dependency_table_t length_dependency_table = generate_len_dep_table();
  static constexpr dependency_table_t parse_dependency_table = generate_parse_dependency_table();
  static constexpr dependency_table_t type_deduction_dep_table = generate_type_deduction_dependency_table();
 
};

template <typename list_metadata>
constexpr auto lookup_field(sv field_name) -> std::optional<field_type_info> {
  auto field_table = list_metadata::field_table;
  return field_table[field_name];
}


constexpr bool is_dependencies_resolved(const field_table_t& field_table, const dependency_table_t& dependency_table) {
  for(auto& entry: dependency_table) {
    auto& [field_name, dependencies] = *entry; 
    auto field_info = field_table[field_name];
    auto field_idx = field_info->occurs_at_idx;
    if(dependency_table.size() > 0) {
      for(auto dep_field: dependencies) {
        auto dep_field_info = field_table[dep_field];
        auto dep_field_idx = dep_field_info->occurs_at_idx;
        if(dep_field_idx > field_idx)
          return false;
      }
    }
  }
  return true;
}


// todo use algorithms over raw loops
template <typename list_metadata>
constexpr bool size_dependencies_resolved() {
  auto field_table = list_metadata::field_table;
  auto length_dependency_table = list_metadata::length_dependency_table;
  return is_dependencies_resolved(field_table, length_dependency_table);
}

template <typename list_metadata>
constexpr bool parse_dependencies_resolved() {
  auto field_table = list_metadata::field_table;
  auto parse_dependency_table = list_metadata::parse_dependency_table;
  return is_dependencies_resolved(field_table, parse_dependency_table);
}

template <typename list_metadata>
constexpr bool type_deduction_dependencies_resolved() {
  auto field_table = list_metadata::field_table;
  auto type_deduction_dep_table = list_metadata::type_deduction_dep_table;
  return is_dependencies_resolved(field_table, type_deduction_dep_table);
}

}

#endif /* _FIELD_LIST_METADATA_HPP_ */

// End /home/hari/repos/s2s/include/field_list/field_list_metadata.hpp

// Begin /home/hari/repos/s2s/include/lib/containers/static_array.hpp
#ifndef _STATIC_ARRAY_HPP_
#define _STATIC_ARRAY_HPP_
 
namespace s2s {
template <typename T, std::size_t N>
class static_array {
public:
  // to facilitate usage as non-type template parameter
  T values[N]{};

  template <typename... Args>
  constexpr static_array(Args&&... entries) {
    static_assert(sizeof...(Args) == N, "Initializers shall be exactly as the container size");
    std::size_t vec_size{0};
    ([&] {
      values[vec_size] = entries; 
      vec_size++;
    }(), ...);
  }
  [[nodiscard]] constexpr const auto& operator[](std::size_t i) const { 
    return values[i]; 
  }
  [[nodiscard]] constexpr auto begin() const { return &values[0]; }
  [[nodiscard]] constexpr auto end() const { return &values[0] + N; }
  [[nodiscard]] constexpr auto size() const { return N; }
  [[nodiscard]] constexpr auto empty() const { return not N; }
  [[nodiscard]] constexpr auto capacity() const { return N; }
};
}

#endif /* _STATIC_ARRAY_HPP_ */

// End /home/hari/repos/s2s/include/lib/containers/static_array.hpp

// Begin /home/hari/repos/s2s/include/api/field_descriptors.hpp
#ifndef _FIELD_DESCRIPTORS_HPP_
#define _FIELD_DESCRIPTORS_HPP_
 
 
 
 
 
 
 
 
 
 
 
 
 
namespace s2s {
struct always_true {
  constexpr auto operator()() -> bool {
    return true;
  }
};

using always_present = eval_bool_from_fields<always_true{}, with_fields<>>;

template <typename size, typename field_type>
concept field_fits_to_underlying_type = deduce_field_size<size>{}() <= sizeof(field_type);

template <fixed_string id, integral T, fixed_size_like size_type, auto constraint_on_value = no_constraint<T>{}>
  requires field_fits_to_underlying_type<size_type, T>
using basic_field = field<id, T, size_type, constraint_on_value>;

template <fixed_string id, field_containable T, std::size_t N, auto constraint_on_value = no_constraint<std::array<T, N>>{}>
using fixed_array_field = field<id, std::array<T, N>, field_size<fixed<N * sizeof(T)>>, constraint_on_value>;

template <fixed_string id, field_list_like T, std::size_t N, auto constraint_on_value = no_constraint<std::array<T, N>>{}>
using array_of_records = field<id, std::array<T, N>, field_size<size_dont_care>, constraint_on_value>;

template <fixed_string id, std::size_t N, auto constraint_on_value = no_constraint<fixed_string<N>>{}>
using fixed_string_field = field<id, fixed_string<N>, field_size<fixed<N + 1>>, constraint_on_value>;

template <fixed_string id, field_containable T, std::size_t N, auto constraint_on_value = no_constraint<T[N]>{}>
using c_arr_field = field<id, T[N], field_size<fixed<N * sizeof(T)>>, constraint_on_value>;

template <fixed_string id, std::size_t N, auto constraint_on_value = no_constraint<char[N + 1]>{}>
using c_str_field = field<id, char[N + 1], field_size<fixed<N * sizeof(char) + 1>>, constraint_on_value>;

template <fixed_string id, std::size_t N, auto expected>
using magic_byte_array = field<id, std::array<unsigned char, N>, field_size<fixed<N>>, eq{expected}>;

template <fixed_string id, fixed_string expected>
using magic_string = field<id, fixed_string<expected.size()>, field_size<fixed<expected.size() + 1>>, eq{expected}>;

template <fixed_string id, integral T, fixed_size_like size, auto expected>
using magic_number = field<id, T, size, eq{expected}>;

// todo how user can provide user defined vector impl or allocator
template <fixed_string id, typename T, variable_size_like size, auto constraint_on_value = no_constraint<std::vector<T>>{}>
using vec_field = field<id, std::vector<T>, size, constraint_on_value>;

template <fixed_string id, field_list_like T, variable_size_like size, auto constraint_on_value = no_constraint<std::vector<T>>{}>
using vector_of_records = field<id, std::vector<T>, size, constraint_on_value>;

// todo check if this will work for all char types like wstring
template <fixed_string id, variable_size_like size, auto constraint_on_value = no_constraint<std::string>{}>
using str_field = field<id, std::string, size, constraint_on_value>;

template <fixed_string id, field_list_like T>
using struct_field = field<id, T, field_size<size_dont_care>, no_constraint<T>{}>;

template <no_variance_field_like base_field, typename present_only_if>
  requires is_eval_bool_from_fields_v<present_only_if>
using maybe = maybe_field<base_field, present_only_if>;


template <typename... type_tags>
struct extract_type_from_tags {
  static constexpr auto type_tag_count = sizeof...(type_tags);
  using type_id_vec = static_array<meta::type_identifier, type_tag_count>;
  static constexpr auto value = type_id_vec(meta::type_id<typename type_tags::type>...);
};

template <typename T>
struct extract_field_choices;

template <
  fixed_string matched_id, 
  template<typename...> typename type_switch,
  auto... match_values, typename... type_tags
>
struct extract_field_choices<
  type<
    match_field<matched_id>, 
    type_switch<
      match_case<match_values, type_tags>...
    >
  >
>
{
  static constexpr auto value = extract_type_from_tags<type_tags...>::value;
};

template <
  auto callable, typename R, typename field_name_list,
  template<typename...> typename type_switch,
  auto... match_values, typename... type_tags
>
struct extract_field_choices<
  type<
    compute<callable, R, field_name_list>, 
    type_switch<
      match_case<match_values, type_tags>...
    >
  >
>
{
  static constexpr auto value = extract_type_from_tags<type_tags...>::value;
};

template <
  auto... callables, typename... field_name_lists, typename... type_tags
>
struct extract_field_choices<
  type<
    type_if_else<
      branch<compute<callables, bool, field_name_lists>, type_tags>...
    >
  >
>
{
  static constexpr auto value = extract_type_from_tags<type_tags...>::value;
};

template <fixed_string id, type_deduction_like type_deducer>
using variance = union_field<id, type_deducer, extract_field_choices<type_deducer>::value>;

template <typename... fields>
concept all_field_like = (field_like<fields> && ...);

template <std::size_t N>
constexpr bool are_field_ids_unique(const std::array<std::string_view, N>& field_id_list) {
  static_set<std::string_view, N> field_id_set(field_id_list);
  return equal_ranges(field_id_list, field_id_set);
}

template <std::size_t N>
constexpr auto as_sv(const fixed_string<N>& str) {
  return std::string_view{str.data()};
}

template <typename... fields>
concept has_unique_field_ids = are_field_ids_unique(std::array{as_sv(fields::field_id)...});

template <typename metadata>
struct dependency_check {
  static constexpr bool size_ok = size_dependencies_resolved<metadata>();
  static constexpr bool parse_ok = parse_dependencies_resolved<metadata>();
  static constexpr bool type_ok = type_deduction_dependencies_resolved<metadata>();

  static_assert(size_ok, "Size dependencies not resolved");
  static_assert(parse_ok, "Parse dependencies not resolved");
  static_assert(type_ok, "Type deduction dependencies not resolved");

  static constexpr bool all_ok = size_ok && parse_ok && type_ok;
};

template <typename metadata>
concept all_dependencies_resolved = dependency_check<metadata>::all_ok;

template <typename... fields>
  requires (all_dependencies_resolved<field_list_metadata<fields...>>)
struct create_struct_field_list {
  using metadata = field_list_metadata<fields...>;
  using value = struct_field_list_impl<metadata, fields...>;
};

template <typename... fields>
  requires all_field_like<fields...> &&
           has_unique_field_ids<fields...>
// using struct_field_list = struct_field_list_impl<field_list_metadata<fields...>, fields...>;
using struct_field_list = create_struct_field_list<fields...>::value;

} /* namespace s2s */

#endif /* _FIELD_DESCRIPTORS_HPP_ */

// End /home/hari/repos/s2s/include/api/field_descriptors.hpp

// Begin /home/hari/repos/s2s/include/field/field_metafunctions.hpp
#ifndef _FIELD_METAFUNCTIONS_HPP_
#define _FIELD_METAFUNCTIONS_HPP_
 
 
 
namespace s2s {
struct not_a_field;

template <typename T>
struct extract_type_from_field;

template <fixed_string id, typename field_type, typename size, auto constraint>
struct extract_type_from_field<field<id, field_type, field_size<size>, constraint>> {
  using type = field_type;
};

template <typename T>
struct extract_type_from_field {
  using type = not_a_field;
};

template <typename T>
using extract_type_from_field_v = typename extract_type_from_field<T>::type;
} /* namespace s2s */

#endif // _FIELD_METAFUNCTIONS_HPP_

// End /home/hari/repos/s2s/include/field/field_metafunctions.hpp

// Begin /home/hari/repos/s2s/include/lib/memory/bit.hpp
#ifndef _BIT_HPP_
#define _BIT_HPP_

// todo check if this handrolled implementation is required
// template <std::integral T>
// constexpr auto byteswap(T value) -> T {
//   constexpr auto object_size = sizeof(T);
//   auto value_rep = std::bit_cast<std::array<std::byte, object_size>>(value);
//   for(std::size_t fwd_idx = 0, rev_idx = object_size - 1; 
//       fwd_idx <= rev_idx; 
//       ++fwd_idx, --rev_idx) 
//   {
//     auto tmp = value_rep[fwd_idx];
//     value_rep[fwd_idx] = value_rep[rev_idx];
//     value_rep[rev_idx] = tmp;
//   }
//   return std::bit_cast<T>(value_rep);
// }
//
//
// constexpr auto is_little() -> bool {
//   constexpr uint32_t bait = 0xdeadbeef;
//   constexpr auto bait_size = sizeof(bait);
//   auto value_rep = std::bit_cast<std::array<std::byte, bait_size>>(bait);
//   return value_rep[0] == std::byte{0xef};
// }
//
// static_assert(byteswap(0xdeadbeef) == 0xefbeadde);
//
//
// enum endian: uint32_t {
//   little = 0xdeadbeef,
//   big = 0xefbeadde,
//   native = is_little() ? little : big
// };

#endif // _BIT_HPP_

// End /home/hari/repos/s2s/include/lib/memory/bit.hpp

// Begin /home/hari/repos/s2s/include/stream/stream_traits.hpp
#ifndef _STREAM_TRAITS_HPP_
#define _STREAM_TRAITS_HPP_
 
 
 
namespace s2s {

template <typename T>
concept convertible_to_bool = requires(T obj) {
  { obj.operator bool() } -> std::same_as<bool>;
  { !obj } -> std::same_as<bool>;
};

template <typename T>
concept std_read_trait = requires(T obj, char* dest_mem, std::streamsize size_to_read) {
  { obj.read(dest_mem, size_to_read) } -> std::convertible_to<std::istream&>;
};

template <typename T>
concept std_write_trait = requires(T obj, const char* src_mem, std::size_t size_to_read) {
  { obj.write(src_mem, size_to_read) } -> std::convertible_to<std::ostream&>;
};

template <typename T>
concept read_trait = requires(T obj, char* dest_mem, std::streamsize size_to_read) {
  { obj.read(dest_mem, size_to_read) } -> std::same_as<T&>;
};

template <typename T>
concept write_trait = requires(T obj, const char* src_mem, std::size_t size_to_read) {
  { obj.write(src_mem, size_to_read) } -> std::same_as<T&>;
};

template <typename T, std::size_t N>
concept constexpr_read_trait = requires(T obj, std::array<char, N>& dest_mem, std::streamsize size_to_read) {
  { obj.read(dest_mem, size_to_read) } -> std::same_as<T&>;
};

template <typename T, std::size_t N>
concept constexpr_write_trait = requires(T obj, const std::array<char, N>& src_mem, std::size_t size_to_read) {
  { obj.write(src_mem, size_to_read) } -> std::same_as<T&>;
};

// todo add operator bool, seekg, tellg, fail, bad, eof/s constaint
struct constexpr_stream{};

template <typename T>
concept identified_as_constexpr_stream = std::is_base_of_v<constexpr_stream, T>;

template <typename T>
concept writeable = std_write_trait<T> || write_trait<T>;

template <typename T>
concept readable = std_read_trait<T> || read_trait<T>;

// todo improve this to be more typesafe
template <typename T>
concept input_stream_like = (identified_as_constexpr_stream<T> || readable<T>) && convertible_to_bool<T>;

template <typename T>
concept output_stream_like = writeable<T> && convertible_to_bool<T>;
}

#endif /* _STREAM_TRAITS_HPP_ */

// End /home/hari/repos/s2s/include/stream/stream_traits.hpp

// Begin /home/hari/repos/s2s/include/lib/memory/address_manip.hpp
#ifndef _ADDRESS_MANIP_HPP_
#define _ADDRESS_MANIP_HPP_
 
 
namespace s2s {
template <input_stream_like stream, typename T>
char* byte_addressof(T& obj) {
  return reinterpret_cast<char*>(&obj);
}

template <input_stream_like stream, typename T, std::size_t N>
char* byte_addressof(std::array<T, N>& obj) {
  return reinterpret_cast<char*>(obj.data());
}

template <input_stream_like stream, std::size_t N>
char* byte_addressof(fixed_string<N>& obj) {
  return reinterpret_cast<char*>(obj.data());
}

template <input_stream_like stream, typename T>
char* byte_addressof(std::vector<T>& obj) {
  return reinterpret_cast<char*>(obj.data());
}

template <input_stream_like stream>
inline char* byte_addressof(std::string& obj) {
  return reinterpret_cast<char*>(&obj[0]);
}

// todo generate this as configurable parameter
constexpr std::size_t constexpr_buffer_size = 2048;

template <identified_as_constexpr_stream stream, typename T, std::size_t size = sizeof(T)>
constexpr auto as_byte_buffer(T& obj) -> std::array<char, size> {
  return std::bit_cast<std::array<char, size>>(obj);
}
}

#endif // _ADDRESS_MANIP_HPP_

// End /home/hari/repos/s2s/include/lib/memory/address_manip.hpp

// Begin /home/hari/repos/s2s/include/field_read/read_impl.hpp
#ifndef _READ_IMPL_HPP_
#define _READ_IMPL_HPP_
 
 
 
namespace s2s {
enum cast_endianness {
  host = 0,
  foreign = 1
};


template <std::endian endianness>
constexpr cast_endianness deduce_byte_order() {
  if constexpr(std::endian::native == endianness) 
    return cast_endianness::host;
  else if constexpr(std::endian::native != endianness) 
    return cast_endianness::foreign;
}

template <typename T, identified_as_constexpr_stream stream>
constexpr auto read_native_impl(stream& s, T& obj, std::size_t size_to_read) -> rw_result {
  auto as_byte_buffer_rep = as_byte_buffer<stream>(obj);
  if(!s.read(as_byte_buffer_rep, size_to_read)) {
    return std::unexpected(error_reason::buffer_exhaustion);
  }
  obj = std::bit_cast<T>(as_byte_buffer_rep);
  return {};
}

template <typename T, readable stream>
constexpr auto read_native_impl(stream& s, T& obj, std::size_t size_to_read) -> rw_result {
  if(!s.read(byte_addressof<stream>(obj), size_to_read)) {
    return std::unexpected(error_reason::buffer_exhaustion);
  }
  return {};
}

template <constant_sized_like T, input_stream_like stream>
constexpr auto read_native(stream& s, T& obj, std::size_t size_to_read) -> rw_result {
  return read_native_impl(s, obj, size_to_read);   
}

template <variable_sized_buffer_like T, input_stream_like stream>
constexpr auto read_native(stream& s, T& obj, std::size_t len_to_read) -> rw_result {
  obj.resize(len_to_read);
  return read_native_impl(s, obj, len_to_read * sizeof(T{}[0]));
}

template <trivial T, input_stream_like stream>
constexpr auto read_foreign_scalar(stream& s, T& obj, std::size_t size_to_read) -> rw_result {
  auto res = read_native_impl(s, obj, size_to_read);
  if(res) {
    // todo rollout byteswap if freestanding compiler doesnt provide one
    obj = std::byteswap(obj);
    return {};
  }
  return res;
}

template <buffer_like T, input_stream_like stream>
constexpr auto read_foreign_buffer(stream& s, T& obj, std::size_t len_to_read) -> rw_result {
  auto res = read_native(s, obj, len_to_read);
  if(res) {
    for(auto& elem: obj) 
      obj = std::byteswap(obj);
    return {};
  }
  return res;
}

template <std::endian endianness, typename T, input_stream_like stream>
constexpr auto read_impl(stream& s, T& obj, std::size_t N) -> rw_result {
  auto constexpr byte_order = deduce_byte_order<endianness>();
  if constexpr(byte_order == cast_endianness::host) {
    return read_native(s, obj, N); 
  } else if constexpr(byte_order == cast_endianness::foreign) {
    if constexpr(trivial<T>) {
      return read_foreign_scalar(s, obj, N);
    } else if constexpr(buffer_like<T>) {
      return read_foreign_buffer(s, obj, N);
    }
  }
}


template <output_stream_like stream>
class output_stream {
private:
  stream& s;

public:
  // delete copy constructor?
  template <typename T>
  constexpr auto write(const char* src_mem, std::size_t size_to_read) -> rw_result {
    // eof = buffer_exhaustion
    // bad | fail = io_error
    if(!s.write(src_mem, size_to_read))
      return std::unexpected(error_reason::buffer_exhaustion);
    return {};
  }
};
} /* namespace s2s */

#endif /* _READ_IMPL_HPP_ */

// End /home/hari/repos/s2s/include/field_read/read_impl.hpp

// Begin /home/hari/repos/s2s/include/field_read/field_reader.hpp
#ifndef _FIELD_READER_HPP_
#define _FIELD_READER_HPP_
 
 
 
namespace s2s {
template <typename F, typename L>
struct read_field;

template <fixed_sized_field_like T, field_list_like F>
struct read_field<T, F> {
  T& field;
  F& field_list;

  constexpr read_field(T& field, F& field_list)
    : field(field), field_list(field_list) {}
  
  template <auto endianness, typename stream>
  constexpr auto read(stream& s) const -> rw_result {
    using field_size = typename T::field_size;
    constexpr auto size_to_read = deduce_field_size<field_size>{}();
    return read_impl<endianness>(s, field.value, size_to_read);
  }
};


template <variable_sized_field_like T, field_list_like F>
struct read_field<T, F> {
  T& field;
  F& field_list;
  
  constexpr read_field(T& field, F& field_list)
    : field(field), field_list(field_list){}

  template <auto endianness, typename stream>
  constexpr auto read(stream& s) const -> rw_result {
    using field_size = typename T::field_size;
    auto len_to_read = deduce_field_size<field_size>{}(field_list);
    return read_impl<endianness>(s, field.value, len_to_read);
  }
};


struct not_array_of_records_field {};

template <typename T>
struct create_field_from_array_of_records;

template <array_of_record_field_like T>
struct create_field_from_array_of_records<T> {
  using array_type = typename T::field_type;
  using array_elem_type = extract_type_from_array_v<array_type>;
  static constexpr auto field_id = T::field_id;
  using size = field_size<size_dont_care>;
  static constexpr auto constraint = no_constraint<array_elem_type>{};

  using res = field<field_id, array_elem_type, size, constraint>;
};

template <typename T>
using create_field_from_array_of_records_v = create_field_from_array_of_records<T>::res;

struct not_vector_of_records_field {};

template <typename T>
struct create_field_from_vector_of_records;

template <vector_of_record_field_like T>
struct create_field_from_vector_of_records<T> {
  using vector_type = typename T::field_type;
  using vector_elem_type = extract_type_from_vec_t<vector_type>;
  static constexpr auto field_id = T::field_id;
  using size = field_size<size_dont_care>;
  static constexpr auto constraint = no_constraint<vector_elem_type>{};

  using res = field<field_id, vector_elem_type, size, constraint>;
};

template <typename T>
using create_field_from_vector_of_records_v = create_field_from_vector_of_records<T>::res;

template <typename T, typename F, typename E>
struct read_buffer_of_records {
  T& field;
  F& field_list;
    std::size_t len_to_read;

  constexpr read_buffer_of_records(T& field, F& field_list, std::size_t len_to_read)
    : field(field), field_list(field_list), len_to_read(len_to_read) {}

  template <auto endianness, typename stream>
  constexpr auto read(stream& s) const -> rw_result {
    for(std::size_t count = 0; count < len_to_read; ++count) {
      E elem;
      auto reader = read_field<E, F>(elem, field_list);
      auto res = reader.template read<endianness, stream>(s);
      if(!res) 
        return std::unexpected(res.error());
      field.value[count] = std::move(elem.value);
    }
    return {};
  }
};

template <array_of_record_field_like T, field_list_like F>
struct read_field<T, F> {
  T& field;
  F& field_list;
  
  constexpr read_field(T& field, F& field_list)
    : field(field), field_list(field_list){}

  template <auto endianness, typename stream>
  constexpr auto read(stream& s) const -> rw_result {
    using array_type = typename T::field_type;
    using array_element_field = create_field_from_array_of_records_v<T>;
    using read_impl_t = read_buffer_of_records<T, F, array_element_field>;

    constexpr auto array_len = extract_size_from_array_v<array_type>;
    auto reader = read_impl_t(field, field_list, array_len);
    auto res = reader.template read<endianness>(s);
    return res;
  }
};


template <vector_of_record_field_like T, field_list_like F>
struct read_field<T, F> {
  T& field;
  F& field_list;
  
  constexpr read_field(T& field, F& field_list)
    : field(field), field_list(field_list){}

  template <auto endianness, typename stream>
  constexpr auto read(stream& s) const -> rw_result {
    using vector_element_field = create_field_from_vector_of_records_v<T>;
    using field_size = typename T::field_size;
    using read_impl_t = read_buffer_of_records<T, F, vector_element_field>;

    auto len_to_read = deduce_field_size<field_size>{}(field_list);
    field.value.resize(len_to_read);
    auto reader = read_impl_t(field, field_list, len_to_read);
    auto res = reader.template read<endianness>(s);
    return res;
  }
};


template <typename F, typename stream, auto endianness>
struct struct_cast_impl;

template <struct_field_like T, field_list_like F>
struct read_field<T, F> {
  T& field;
  F& field_list;
  
  constexpr read_field(T& field, F& field_list)
    : field(field), field_list(field_list){}

  template <auto endianness, typename stream>
  constexpr auto read(stream& s) const -> rw_result {
    using field_list_t = extract_type_from_field_v<T>;
    auto res = struct_cast_impl<field_list_t, stream, endianness>{}(s);
    if(!res) {
      auto err = res.error();
      return std::unexpected(err.failure_reason);
    }
    field.value = *res;
    return {};
  }
};



template <optional_field_like T, field_list_like F>
struct read_field<T, F> {
  T& field;
  F& field_list;
  
  constexpr read_field(T& field, F& field_list): 
    field(field), field_list(field_list){}
  
  template <auto endianness, typename stream>
  constexpr auto read(stream& s) -> rw_result {
    if(!typename T::field_presence_checker{}(field_list)) {
      field.value = std::nullopt;
      return {};
    }
    using field_base_type_t = typename T::field_base_type;
    field_base_type_t base_field{};
    read_field<field_base_type_t, F> reader(base_field, field_list);
    auto res = reader.template read<endianness>(s);
    if(!res) 
      return std::unexpected(res.error());
    field.value = base_field.value;
    return {};
  }
};


// Helper function to read bytes into the variant
template<std::size_t idx, typename T, typename F, typename V>
struct read_variant_impl {
  V& variant;
  F& field_list;
  std::size_t idx_r;

  constexpr explicit read_variant_impl(
    V& variant, 
    F& field_list,
    std::size_t idx_r) :
      variant(variant), field_list(field_list), idx_r(idx_r) {}

  template <auto endianness, typename stream>
  constexpr auto read(stream& s) -> rw_result {
    if (idx_r != idx) 
      return {};

    T field;
    auto reader = read_field<T, F>(field, field_list);
    auto res = reader.template read<endianness, stream>(s);
    if(!res)
      return std::unexpected(res.error());
    variant = std::move(field.value);
    return {};
  }
};

constexpr auto operator|(const rw_result& res, auto&& callable) -> rw_result
{
  return res ? callable() : std::unexpected(res.error());
}

template <typename T, typename F, typename field_choices, typename idx_seq>
struct read_variant_helper;

template <typename T, typename F, typename... fields, std::size_t... idx>
struct read_variant_helper<T, F, field_choice_list<fields...>, std::index_sequence<idx...>> {
  T& field;
  F& field_list;
  std::size_t idx_r;
  
  constexpr read_variant_helper(T& field, F& field_list, std::size_t idx_r) 
    : field(field), field_list(field_list), idx_r(idx_r) {}
  
  template <auto endianness, typename stream>
  constexpr auto read(stream& s) -> rw_result {
    rw_result pipeline_seed{};
    return (
      pipeline_seed |
      ... | 
      [&]() {
        auto reader_impl = read_variant_impl<idx, fields, F, typename T::field_type>(field.value, field_list, idx_r);
        return reader_impl.template read<endianness>(s);
      }
    );
  }
};


template <union_field_like T, field_list_like F>
struct read_field<T, F> {
  T& field;
  F& field_list;
  
  constexpr read_field(T& field, F& field_list): 
    field(field), field_list(field_list){}

  template <auto endianness, typename stream>
  constexpr auto read(stream& s) -> rw_result {
    using type_deduction_guide = typename T::type_deduction_guide;
    using field_choices = typename T::field_choices;
    constexpr auto max_type_index = T::variant_size;

    auto type_index_deducer = type_deduction_guide();
    auto type_index_result = type_index_deducer(field_list); 
    if(!type_index_result)
      return std::unexpected(type_index_result.error());

    auto idx_r = *type_index_result;
    using read_helper_t = 
      read_variant_helper<
        T, 
        F, 
        field_choices, 
        std::make_index_sequence<max_type_index>
      >;
    auto field_reader = read_helper_t(field, field_list, idx_r);
    auto field_read_res = field_reader.template read<endianness, stream>(s);
    if(!field_read_res)
      return std::unexpected(field_read_res.error());
    return {};
  }
};
} /* namespace s2s */

#endif // _FIELD_READER_HPP_

// End /home/hari/repos/s2s/include/field_read/field_reader.hpp

// Begin /home/hari/repos/s2s/include/field_validation/field_value_constraints_traits.hpp
#ifndef _FIELD_VALE_CONSTRAINT_TRAITS_HPP_
#define _FIELD_VALE_CONSTRAINT_TRAITS_HPP_
 
namespace s2s {
template <typename T>
struct is_no_constraint;

template <typename T>
struct is_no_constraint<no_constraint<T>> {
  static constexpr bool res = true;
};

template <typename T>
struct is_no_constraint {
  static constexpr bool res = false;
};

template <typename T>
inline constexpr bool is_no_constraint_v = is_no_constraint<T>::res;
}


#endif /* _FIELD_VALE_CONSTRAINT_TRAITS_HPP_ */

// End /home/hari/repos/s2s/include/field_validation/field_value_constraints_traits.hpp

// Begin /home/hari/repos/s2s/include/cast/struct_cast_impl.hpp
#ifndef _STRUCT_CAST_IMPL_HPP_
#define _STRUCT_CAST_IMPL_HPP_

// status: split to cast and cast impl
 
namespace s2s {

constexpr auto operator|(const cast_result& res, auto&& callable) -> cast_result
{
  return res ? callable() : std::unexpected(res.error());
}


template <typename F, typename stream, auto endianness>
struct struct_cast_impl;

template <typename metadata, typename... fields, typename stream, auto endianness>
struct struct_cast_impl<struct_field_list_impl<metadata, fields...>, stream, endianness> {
  using S = struct_field_list_impl<metadata, fields...>;
  using R = std::expected<S, cast_error>;

  constexpr auto operator()(stream& s) -> R {
    S field_list;
    cast_result pipeline_seed{};
    auto res = (
      pipeline_seed |
      ... |
      [&]() -> cast_result {
        auto& field = static_cast<fields&>(field_list);
        auto reader = read_field<fields, S>(field, field_list);
        auto read_res = reader.template read<endianness>(s);
        // Short circuit the remaining pipeline since read failed for current field
        if(!read_res) {
          auto field_name = std::string_view{fields::field_id.data()};
          auto err = read_res.error();
          auto validation_err = cast_error{err, field_name};
          return std::unexpected(validation_err);
        }
        bool field_validation_res = fields::constraint_checker(field.value);
        if(!field_validation_res) {
          auto field_name = std::string_view{fields::field_id.data()};
          auto err = error_reason::validation_failure;
          auto validation_err = cast_error{err, field_name};
          return std::unexpected(validation_err);
        }
        // Both reading and validating went well
        return {};
      }
    );
    return res ? R(field_list) : std::unexpected(res.error());
  }
};

} /* namespace s2s */

#endif // _STRUCT_CAST_IMPL_HPP_

// End /home/hari/repos/s2s/include/cast/struct_cast_impl.hpp

// Begin /home/hari/repos/s2s/include/api/struct_cast.hpp
#ifndef _STRUCT_CAST_HPP_
#define _STRUCT_CAST_HPP_

// status: split to cast and cast impl
 
 
namespace s2s {
template <field_list_like T, input_stream_like stream>
[[nodiscard]] constexpr auto struct_cast_le(stream& s) -> std::expected<T, cast_error> {
  return struct_cast_impl<T, stream, std::endian::little>{}(s);
}

template <field_list_like T, input_stream_like stream>
[[nodiscard]] constexpr auto struct_cast_be(stream& s) -> std::expected<T, cast_error> {
  return struct_cast_impl<T, stream, std::endian::big>{}(s);
}
} /* namespace s2s */

#endif // _STRUCT_CAST_HPP_

// End /home/hari/repos/s2s/include/api/struct_cast.hpp

// Begin /home/hari/repos/s2s/include/s2s.hpp
#ifndef STRUCT_CAST_HPP
#define STRUCT_CAST_HPP
 
 
 
 
 
 
 
 
#endif // STRUCT_CAST_HPP

// End /home/hari/repos/s2s/include/s2s.hpp
