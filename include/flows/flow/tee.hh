#ifndef FLOWS_FLOW_TEE_HH
#define FLOWS_FLOW_TEE_HH

#include <type_traits>

#include <flows/utils/traits.hh>

namespace flows {

template <class S, class In=typename S::input_type>
struct tee_flow_t {

  using input_type = In;
  using output_type = In;

  static_assert(std::is_convertible<In, typename S::input_type>::value,
                "Cannot push to this sink");

  S s;

  template <class Push>
  auto accepting(Push& ds) const
    -> decltype(ds.accepting())
  {
    return ds.accepting();
  }

  template <class _In, class Push>
  require_convertible_to<_In, In>
  push(_In&& x, Push& ds)
  {
    if (s.accepting())
      s.push(x);
    ds.push((rcforward_t<_In, In>)x);
  }

};

template <class S>
struct unbound_tee_flow_t {

  S s;

  template <class In>
  using bound_flow = tee_flow_t<typename S::template bound_sink<In> >;

  template <class In>
  bound_flow<In>
  constexpr bind() const&
  {
    return { s.template bind<In>() };
  }

#if __cplusplus >= 201402L

  template <class In>
  bound_flow<In>
  constexpr bind() &&
  {
    return { ((S&&)s).template bind<In>() };
  }

#endif

};

// in case S is bound but we want to bind to 
template <class S>
struct rebindable_tee_flow_t {

  S s;

  template <class In>
  using bound_flow = tee_flow_t<S, In>;

  template <class In>
  bound_flow<In>
  constexpr bind() const&
  {
    return { s };
  }

#if __cplusplus >= 201402L

  template <class In>
  bound_flow<In>
  constexpr bind() &&
  {
    return { (S&&)s };
  }

#endif

};

template <class S>
typename require_types<
  unbound_tee_flow_t<typename std::decay<S>::type>,
  require_unbound_sink<S>
>::type
constexpr tee(S&& s)
{
  return { (S&&)s };
}

template <class S>
typename require_types<
  tee_flow_t<typename std::decay<S>::type>,
  require_sink<S>
>::type
constexpr tee(S&& s)
{
  return { (S&&)s };
}

FLOWS_OP_HELPER(tee);

} // namespace flows

#endif /* FLOWS_FLOW_TEE_HH */
