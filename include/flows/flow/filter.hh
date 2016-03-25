#ifndef FLOWS_FLOW_FILTER_HH
#define FLOWS_FLOW_FILTER_HH

#include <type_traits>

namespace flows {

template <class Pred, class In>
struct filter_flow_t {

  using input_type = In;
  using output_type = In;

  static_assert(std::is_convertible<typename std::result_of<Pred(In)>::type, bool>(),
                "Cannot apply input type to predicate");

  Pred p;

  template <class DS>
  constexpr auto accepting(DS& ds) const
    -> decltype(ds.accepting())
  {
    return ds.accepting();
  }

  template <class _In, class DS>
  require_convertible_to<_In, In>
  push(_In&& x, DS& ds)
  {
    if (p(x))
      ds.push((cforward_t<_In, In>)x);
  }

};

template <class Pred>
struct unbound_filter_flow_t {

  Pred p;

  template <class In>
  using bound_flow = filter_flow_t<Pred, In>;

  template <class In>
  bound_flow<In>
  constexpr bind() const&
  {
    return { p };
  }

#if __cplusplus >= 201402L

  template <class In>
  bound_flow<In>
  constexpr bind() &&
  {
    return { (Pred&&)p };
  }

#endif

};

template <class F>
unbound_filter_flow_t<typename std::decay<F>::type>
constexpr filter(F&& f)
{
  return { (F&&)f };
}

} // namespace flows

#endif /* FLOWS_FLOW_FILTER_HH */
