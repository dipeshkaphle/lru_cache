#include <functional>
#include <iostream>
#include <tuple>
#include <utility>

// hack to make it work with non c++20 compilers
#ifndef CPP20
#define Hashable typename
#endif

#ifdef CPP20
template <typename T>
concept Hashable = requires(const T &a) {
  { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};
#endif

namespace variadic_hasher {

template <typename T, typename = std::void_t<>>
struct is_std_hashable : std::false_type {};

template <typename T>
struct is_std_hashable<
    T, std::void_t<decltype(std::declval<std::hash<T>>()(std::declval<T>()))>>
    : std::true_type {};

template <typename T>
constexpr bool is_std_hashable_v = is_std_hashable<T>::value;

// reference: https://stackoverflow.com/a/2595226
static inline std::size_t hash_combine(std::size_t seed, std::size_t h) {
  seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  return seed;
}

template <Hashable... Args> std::size_t hash(const Args &...args) { return 0; }
template <Hashable T, Hashable... Args>
std::size_t hash(const T &t, const Args &...args) {

#ifndef CPP20
  static_assert(is_std_hashable_v<T>, "Type must be hashable with std::hash");
#endif

  std::size_t h1 = std::hash<T>()(t);
  std::size_t h2 = hash(std::forward<decltype(args)>(args)...);
  return hash_combine(h2, h1);
}

}; // namespace variadic_hasher

template <typename... T> struct std::hash<std::tuple<T...>> {
  size_t operator()(std::tuple<T...> const &arg) const noexcept {
    return std::apply(
        [&](const T &...ts) { return variadic_hasher::hash(ts...); }, arg);
  }
};

template <typename T, typename U> struct std::hash<std::pair<T, U>> {
  size_t operator()(std::pair<T, U> const &arg) const noexcept {

    return std::apply(
        [&](const T &t, const U &u) { return variadic_hasher::hash(t, u); },
        arg);
  }
};
