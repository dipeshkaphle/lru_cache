#include "./tuple_hash.hpp"
#include "./types.hpp"
#include <functional>
#include <unordered_map>

template <typename Ret, typename... Params> struct LruCache {
private:
  mutable std::unordered_map<std::tuple<Params...>, Ret> _dict;
  F<Ret, Params...> _f;
  Function<Ret, Params...> _g;

public:
  LruCache() : _dict{} {}
  LruCache(Function<Ret, Params...> f) : _dict{}, _g(f) {

    auto rec = [&](Params &&...args) -> Ret {
      return (*this)(std::forward<Params>(args)...);
    };
    this->_f = [f = std::move(f), g = std::move(rec)](Params &&...args) {
      return f(g, std::forward<Params>(args)...);
    };
  }

  LruCache(LruCache &&c) : _dict{c._dict} {
    auto f = c._g;
    auto rec = [this](Params &&...args) -> Ret {
      return (*this)(std::forward<Params>(args)...);
    };
    this->_f = [f = std::move(f), g = std::move(rec)](Params &&...args) {
      return f(g, std::forward<Params>(args)...);
    };
  }
  LruCache &operator=(LruCache &&c) {
    this->_dict = c._dict;
    auto f = c._g;
    auto rec = [this](Params &&...args) -> Ret {
      return (*this)(std::forward<Params>(args)...);
    };
    this->_f = [f = std::move(f), g = std::move(rec)](Params &&...args) {
      return f(g, std::forward<Params>(args)...);
    };
    return *this;
  }

  Ret &operator()(const Params &...args) const {
    auto args_as_tuple = std::make_tuple(args...);
    if (_dict.find(args_as_tuple) != _dict.end()) {
      return _dict[args_as_tuple];
    }
    _dict.insert({args_as_tuple, _f(args...)});
    return _dict[args_as_tuple];
  }
};

// Here's a deduction guide for our LruCache
// Pair objects initialized with arguments of type T and U should deduce to
// Pair<T, U>
template <typename Ret, typename... Params>
LruCache(Function<Ret, Params...>) -> LruCache<Ret, Params...>;
