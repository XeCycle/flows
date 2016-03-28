#ifndef FLOWS_FLOW_CATEGORY_HH
#define FLOWS_FLOW_CATEGORY_HH

#include <type_traits>
#include <utility>

#include <flows/flow/contramap.hh>
#include <flows/utils/static-fold.hh>
#include <flows/utils/macros.hh>
#include <flows/utils/traits.hh>

namespace flows {

template <class In>
struct identity_flow_t {

  using input_type = In;
  using output_type = In;

  template <class DS>
  constexpr auto accepting(DS& ds) const
    -> decltype(ds.accepting())
  {
    return ds.accepting();
  }

  template <class _In, class DS>
  require_convertible_to<_In, In>
  push(_In&& x, DS& ds) const
  {
    ds.push((rcforward_t<_In, In>)x);
  }

};

template <class F, class G>
struct flow_compose2_t {

  using input_type = typename G::input_type;
  using output_type = typename F::output_type;

  static_assert(std::is_convertible<typename G::output_type, typename F::input_type>(),
                "Cannot compose because of type mismatch");

  F f;
  G g;

  template <class DS>
  auto accepting(DS& ds)
    -> decltype(g.accepting(
                  std::declval<contramap_sink_proxy_t<F, typename std::decay<DS>::type>&>()))
  {
    contramap_sink_proxy_t<F, typename std::decay<DS>::type> proxy(f, ds);
    return g.accepting(proxy);
  }

  template <class In, class DS>
  require_convertible_to<In, input_type>
  push(In&& x, DS& ds)
  {
    contramap_sink_proxy_t<F, typename std::decay<DS>::type> proxy(f, ds);
    g.push((rcforward_t<In, input_type>)x, proxy);
  }

};

template <class F, class G>
struct unbound_flow_compose2_t {

private:
  template <class In>
  using g_out = typename G::template bound_flow<In>::output_type;

public:

  template <class In>
  using bound_flow = flow_compose2_t<typename F::template bound_flow<g_out<In> >,
                                     typename G::template bound_flow<In> >;

  F f;
  G g;

  template <class In>
  bound_flow<In>
  constexpr bind() const&
  {
    using g_out = typename G::template bound_flow<In>::output_type;
    return { f.template bind<g_out>(), g.template bind<In>() };
  }

#if __cplusplus >= 201402L

  template <class In>
  bound_flow<In>
  constexpr bind() &&
  {
    using g_out = typename G::template bound_flow<In>::output_type;
    return { ((F&&)f).template bind<g_out>(), ((G&&)g).template bind<In>() };
  }

#endif

};

// bound . unbound is still unbound because input type unknown
template <class F, class U>
struct r_unbound_flow_compose2_t {

private:
  template <class In>
  using u_out = typename U::template bound_flow<In>::output_type;

public:

  template <class In>
  using bound_flow = typename std::enable_if<
    std::is_convertible<u_out<In>, typename F::input_type>::value,
    flow_compose2_t<F, typename U::template bound_flow<In> >
  >::type;

  F f;
  U u;

  template <class In>
  bound_flow<In>
  constexpr bind() const&
  {
    return { f, u.template bind<u_out<In> >() };
  }

#if __cplusplus >= 201402L

  template <class In>
  bound_flow<In>
  constexpr bind() &&
  {
    return { (F&&)f, ((U&&)u).template bind<u_out<In> >() };
  }
  
#endif

};

// both unbound
template <class F, class G>
typename require_types<
  unbound_flow_compose2_t<typename std::decay<F>::type, typename std::decay<G>::type>,
  require_unbound_flow<F>, require_unbound_flow<G>
>::type
constexpr compose2(F&& f, G&& g)
{
  return { (F&&)f, (G&&)g };
}

// both bound
template <class F, class G>
typename require_types<
  flow_compose2_t<typename std::decay<F>::type, typename std::decay<G>::type>,
  require_flow<F>, require_flow<G>
>::type
constexpr compose2(F&& f, G&& g)
{
  return { (F&&)f, (G&&)g };
}

// rhs unbound
template <class F, class U>
typename require_types<
  r_unbound_flow_compose2_t<typename std::decay<F>::type, typename std::decay<U>::type>,
  require_flow<F>, require_unbound_flow<U>
>::type
constexpr compose2(F&& f, U&& g)
{
  return { (F&&)f, (U&&)g };
}

// lhs unbound --- auto bind
template <class U, class G>
typename require_types<
  flow_compose2_t<typename U::template bound_flow<typename G::output_type>,
                  typename std::decay<G>::type>,
  require_unbound_flow<U>, require_flow<G>
>::type
constexpr compose2(U&& u, G&& g)
{
  return { ((U&&)u).template bind<typename G::output_type>(), (G&&)g };
}

FLOWS_OP_HELPER(compose2);

template <class... F>
constexpr auto compose(F&&... f)
  -> decltype(static_foldr(op_compose2, (F&&)f...))
{
  return static_foldr(op_compose2, (F&&)f...);
}

inline
namespace operators {

template <class F, class G>
auto operator<<(F&& f, G&& g)
  -> decltype(flows::compose2((F&&)f, (G&&)g))
{
  return flows::compose2((F&&)f, (G&&)g);
}

template <class G, class F>
auto operator>>(G&& g, F&& f)
  -> decltype(flows::compose2((F&&)f, (G&&)g))
{
  return flows::compose2((F&&)f, (G&&)g);
}

} // inline namespace operators

} // namespace flows

#endif /* FLOWS_FLOW_CATEGORY_HH */
