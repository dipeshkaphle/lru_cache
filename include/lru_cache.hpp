#include <functional>
#include <unordered_map>

template <typename Ret, typename... Params> class LruCache {
private:
  mutable std::unordered_map<std::tuple<Params...>, Ret> _dict;
  using FuncType = std::function<Ret(Params...)>;
  FuncType _f;
  std::function<Ret(FuncType, Params...)> _g;

public:
  LruCache() : _dict{} {}
  LruCache(std::function<Ret(FuncType, Params...)> f) : _dict{}, _g(f) {

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
