#include "single_header/lru_cache.hpp"
#include <cassert>
#include <iostream>
#include <unordered_map>

int main() {

  size_t mod = 1e9 + 7;

  auto fib = [&](std::function<size_t(size_t)> f, std::size_t n) {
    if (n <= 1)
      return n;
    return (f(n - 1) + f(n - 2)) % mod;
  };

  LruCache<size_t, size_t> my_fib{fib};

  // happens instantly
  for (size_t n = 0; n <= 1000000; n++) {
    my_fib(n);
  }
}
