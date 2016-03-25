#ifndef FLOWS_UTILS_STATIC_FOLD_HH
#define FLOWS_UTILS_STATIC_FOLD_HH

namespace flows {

// fold(binary_operator, values...)

namespace fn_impl {

template <size_t arity>
struct static_foldr_impl_t {
  template <class BinOp, class H, class... T>
  static constexpr auto call(BinOp&& op, H&& h, T&&... t)
    -> decltype(op((H&&)h, static_foldr_impl_t<arity-1>::call(op, (T&&)t...)))
  {
    static_assert(sizeof...(t)+1 == arity, "static_foldr: arguments count mismatch");
    return op((H&&)h, static_foldr_impl_t<arity-1>::call(op, (T&&)t...));
  }
};

template <>
struct static_foldr_impl_t<1> {
  template <class BinOp, class T>
  typename std::decay<T>::type
  static constexpr call(BinOp&&, T&& t)
  {
    return (T&&)t;
  }
};

} // namespace fn_impl

template <class BinOp, class... T>
constexpr auto static_foldr(BinOp&& op, T&&... v)
  -> decltype(fn_impl::static_foldr_impl_t<sizeof...(v)>::call(op, (T&&)v...))
{
  return fn_impl::static_foldr_impl_t<sizeof...(v)>::call(op, (T&&)v...);
}

namespace fn_impl {

template <size_t arity>
struct static_foldl_impl_t {
  template <class BinOp, class F, class S, class... T>
  static constexpr auto call(BinOp&& op, F&& f, S&& s, T&&... t)
    -> decltype(static_foldl_impl_t<arity-1>::call(op, op((F&&)f, (S&&)s), (T&&)t...))
  {
    static_assert(sizeof...(t)+2 == arity, "static_foldl: arguments count mismatch");
    return static_foldl_impl_t<arity-1>::call(op, op((F&&)f, (S&&)s), (T&&)t...);
  }
};

template <>
struct static_foldl_impl_t<1> {
  template <class BinOp, class T>
  typename std::decay<T>::type
  static constexpr call(BinOp&&, T&& t)
  {
    return (T&&)t;
  }
};

} // namespace fn_impl

template <class BinOp, class... T>
constexpr auto static_foldl(BinOp&& op, T&&... v)
  -> decltype(fn_impl::static_foldl_impl_t<sizeof...(v)>::call(op, (T&&)v...))
{
  return fn_impl::static_foldl_impl_t<sizeof...(v)>::call(op, (T&&)v...);
}

} // namespace flows

#endif /* FLOWS_UTILS_STATIC_FOLD_HH */
