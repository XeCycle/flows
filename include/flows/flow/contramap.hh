#ifndef FLOWS_FLOW_CONTRAMAP_HH
#define FLOWS_FLOW_CONTRAMAP_HH

#include <memory>
#include <type_traits>

#include <flows/utils/traits.hh>

namespace flows {

template <class F, class S>
struct contramap_sink_t {

  using input_type = typename F::input_type;

  static_assert(std::is_convertible<typename F::output_type, typename S::input_type>(),
                "Cannot contramap because of type mismatch");

  F f;
  S s;

  auto accepting()
    -> decltype(f.accepting(s))
  {
    return f.accepting(s);
  }

  template <class In>
  require_convertible_to<In, input_type>
  push(In&& x)
  {
    f.push((rcforward_t<In, input_type>)x, s);
  }

};

// similar to above but does not hold ownership of either
template <class F, class S>
struct contramap_sink_proxy_t {

  using input_type = typename F::input_type;

  static_assert(std::is_convertible<typename F::output_type, typename S::input_type>(),
                "Cannot contramap because of type mismatch");

private:
  F* pf;
  S* ps;

public:

  constexpr contramap_sink_proxy_t(F& f, S& s)
    : pf(std::addressof(f)), ps(std::addressof(s))
  {}

  auto accepting() const
    -> decltype(pf->accepting(*ps))
  {
    return pf->accepting(*ps);
  }

  template <class In>
  require_convertible_to<In, input_type>
  push(In&& x)
  {
    pf->push((rcforward_t<In, input_type>)x, *ps);
  }

};

template <class F, class S>
struct unbound_contramap_sink_t {

  F f;
  S s;

private:
  template <class In>
  using f_out = typename F::template bound_flow<In>::output_type;

public:

  template <class In>
  using bound_sink = contramap_sink_t<typename F::template bound_flow<In>,
                                      typename S::template bound_sink<f_out<In> > >;

  template <class In>
  bound_sink<In>
  constexpr bind() const&
  {
    using f_out = typename F::template bound_flow<In>::output_type;
    return { f.template bind<In>(), s.template bind<f_out>() };
  }

#if __cplusplus >= 201402L

  template <class In>
  bound_sink<In>
  constexpr bind() &&
  {
    using f_out = typename F::template bound_flow<In>::output_type;
    return { ((F&&)f).template bind<In>(), ((S&&)s).template bind<f_out>() };
  }

#endif

};

template <class F, class S>
struct f_unbound_contramap_sink_t {

private:
  template <class In>
  using f_out = typename F::template bound_flow<In>::output_type;

public:

  template <class In>
  using bound_sink = typename std::enable_if<
    std::is_convertible<f_out<In>, typename S::input_type>::value,
    contramap_sink_t<typename F::template bound_flow<In>, S>
  >::type;

  F f;
  S s;

  template <class In>
  bound_sink<In>
  constexpr bind() const&
  {
    return { f.template bind<In>(), s };
  }

#if __cplusplus >= 201402L

  template <class In>
  bound_sink<In>
  constexpr bind() &&
  {
    return { ((F&&)f).template bind<In>(), (S&&)s };
  }

#endif

};

// both unbound
template <class F, class S>
typename require_types<
  unbound_contramap_sink_t<typename std::decay<F>::type, typename std::decay<S>::type>,
  require_unbound_flow<F>, require_unbound_sink<S>
>::type
constexpr contramap(F&& f, S&& s)
{
  return { (F&&)f, (S&&)s };
}

// both bound
template <class F, class S>
typename require_types<
  contramap_sink_t<typename std::decay<F>::type, typename std::decay<S>::type>,
  require_flow<F>, require_sink<S>
>::type
constexpr contramap(F&& f, S&& s)
{
  return { (F&&)f, (S&&)s };
}

// f unbound
template <class F, class S>
typename require_types<
  f_unbound_contramap_sink_t<typename std::decay<F>::type, typename std::decay<S>::type>,
  require_unbound_flow<F>, require_sink<S>
>::type
constexpr contramap(F&& f, S&& s)
{
  return { (F&&)f, (S&&)s };
}

// s unbound --- auto bind
template <class F, class S>
typename require_types<
  contramap_sink_t<typename std::decay<F>::type,
                   typename std::decay<S>::type
                   ::template bound_sink<typename std::decay<F>::type::output_type> >,
  require_flow<F>, require_unbound_sink<S>
>::type
constexpr contramap(F&& f, S&& s)
{
  return { (F&&)f, ((S&&)s).template bind<typename std::decay<F>::type::output_type>() };
}

inline
namespace operators {

template <class F, class S>
constexpr auto operator>>(F&& f, S&& s)
  -> decltype(flows::contramap((F&&)f, (S&&)s))
{
  return flows::contramap((F&&)f, (S&&)s);
}

} // inline namespace operators

} // namespace flows

#endif /* FLOWS_FLOW_CONTRAMAP_HH */
