#include <functional>

template <typename Ret, typename... Args> using F = std::function<Ret(Args...)>;

template <typename Ret, typename... Args>
using Function = std::function<Ret(F<Ret, Args...>, Args...)>;
