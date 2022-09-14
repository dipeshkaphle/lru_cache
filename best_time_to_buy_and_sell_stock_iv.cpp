#include "include/lru_cache.hpp"
#include "include/tuple_hash.hpp"
#include <iostream>
#include <vector>

using namespace std;
class Solution {
public:
  int maxProfit(int k, vector<int> &prices) {
    if (prices.size() <= 1)
      return 0;

    LruCache<int, int, int> sell;
    auto b = [&](std::function<int(int, int)> by, int k, int i) -> int {
      if (k == 0 || i == prices.size()) {
        return 0;
      }
      return max(sell(k - 1, i + 1) - prices[i], by(k, i + 1));
    };
    LruCache<int, int, int> buy{b};
    auto s = [&](std::function<int(int, int)> sl, int k, int i) -> int {
      if (i == prices.size())
        return 0;
      return max(buy(k, i + 1) + prices[i], sl(k, i + 1));
    };
    sell = LruCache<int, int, int>{s};
    return buy(k, 0);
  }
};

int main() {
  vector<int> prices{3, 2, 6, 5, 0, 3};
  Solution s;
  cout << s.maxProfit(2, prices) << "\n";
}
