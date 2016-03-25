#ifndef FLOWS_FLOW_MAP_HH
#define FLOWS_FLOW_MAP_HH

#include <type_traits>

#include <flows/utils/traits.hh>

namespace flows {

template <class F, class In>
struct map_flow_t {

  using input_type = In;
  using output_type = typename std::result_of<F(In)>::type;

  F f;

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
    ds.push(f((cforward_t<_In, In>)x));
  }

};

template <class F>
struct unbound_map_flow_t {

  F f;

  template <class In>
  using bound_flow = map_flow_t<F, In>;

  template <class In>
  bound_flow<In>
  constexpr bind() const&
  {
    return { f };
  }

#if __cplusplus >= 201402L

  template <class In>
  bound_flow<In>
  constexpr bind() &&
  {
    return { (F&&)f };
  }

#endif

};

template <class F>
unbound_map_flow_t<typename std::decay<F>::type>
constexpr map(F&& f)
{
  return { (F&&)f };
}

} // namespace flows

#endif /* FLOWS_FLOW_MAP_HH */
