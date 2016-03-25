#ifndef FLOWS_FLOW_ALTERNATIVE_HH
#define FLOWS_FLOW_ALTERNATIVE_HH

// L || R
// push to L first, if L did not produce any results, push to R
// .accepting() decided by L

#include <type_traits>

#include <flows/flow/contramap.hh>
#include <flows/utils/traits.hh>

namespace flows {

template <class L, class R, class In=typename L::input_type>
struct alternative2_flow_t {

  using input_type = In;
  using output_type = typename std::common_type<
    typename L::output_type, typename R::output_type>::type;

  static_assert(std::is_convertible<In, typename L::input_type>::value,
                "Left side does not accept this input_type");
  static_assert(std::is_convertible<In, typename R::input_type>::value,
                "Right side does not accept this input_type");

  L l;
  R r;

  template <class DS>
  auto accepting(DS& ds)
    -> decltype(l.accepting(ds))
  {
    return l.accepting(ds);
  }

private:

  template <class S, class _In>
  struct check_has_output_sink_t {

    S* ps;
    bool* pflag;

    using input_type = _In;

    auto accepting() const
      -> decltype(ps->accepting())
    {
      return ps->accepting();
    }

    template <class __In>
    require_convertible_to<__In, _In>
    push(__In&& x)
    {
      ps->push((cforward_t<__In, _In>)x);
      *pflag = true;
    }

  };

public:

  template <class _In, class DS>
  require_convertible_to<_In, In>
  push(_In&& x, DS& ds)
  {
    bool l_has_output = false;
    check_has_output_sink_t<DS, output_type> proxy { std::addressof(ds), &l_has_output };
    l.push(x, proxy);
    if (l_has_output)
      return;
    if (r.accepting(ds))
      r.push((cforward_t<__In, _In>)x, ds);
  }

};

template <class L, class R>
struct unbound_alternative2_flow_t {

  L l;
  R r;

  template <class In>
  using bound_flow = alternative2_flow_t<typename L::template bound_flow<In>,
                                         typename R::template bound_flow<In> >;

  template <class In>
  bound_flow<In>
  constexpr bind() const&
  {
    return { l.template bind<In>(), r.template bind<In>() };
  }

#if __cplusplus >= 201402L

  template <class In>
  bound_flow<In>
  constexpr bind() &&
  {
    return { ((L&&)l).template bind<In>(), ((R&&)r).template bind<In>() };
  }

#endif

};

template <class L, class R>
typename require_types<
  unbound_alternative2_flow_t<typename std::decay<L>::type, typename std::decay<R>::type>,
  require_unbound_flow<L>, require_unbound_flow<R>
>::type
constexpr alternative2(L&& l, R&& r)
{
  return { (L&&)l, (R&&)r };
}

template <class L, class R>
typename require_types<
  alternative2_flow_t<typename std::decay<L>::type, typename std::decay<R>::type>,
  require_flow<L>, require_flow<R>
>::type
constexpr alternative2(L&& l, R&& r)
{
  return { (L&&)l, (R&&)r };
}

// if either side is bound, auto bind the other side

template <class L, class R>
typename require_types<
  alternative2_flow_t<typename std::decay<L>::type::template bound_flow<typename R::input_type>,
                      typename std::decay<R>::type>,
  require_unbound_flow<L>, require_flow<R>
>::type
constexpr alternative2(L&& l, R&& r)
{
  return { ((L&&)l).template bind<typename R::input_type>(), (R&&)r };
}

template <class L, class R>
typename require_types<
  alternative2_flow_t<typename std::decay<L>::type,
                      typename std::decay<R>::type::template bound_flow<typename L::input_type> >,
  require_flow<L>, require_unbound_flow<R>
>::type
constexpr alternative2(L&& l, R&& r)
{
  return { (L&&)l, ((R&&)r).template bind<typename L::input_type>() };
}

FLOWS_OP_HELPER(alternative2);

template <class... F>
constexpr auto alternative(F&&... f)
  -> decltype(static_foldr(op_alternative2, (F&&)f...))
{
  return static_foldr(op_alternative2, (F&&)f...);
}

inline
namespace operators {

template <class L, class R>
constexpr auto operator||(L&& l, R&& r)
  -> decltype(flows::alternative2((L&&)l, (R&&)r))
{
  return flows::alternative2((L&&)l, (R&&)r);
}

} // inline namespace operators

} // namespace flows

#endif /* FLOWS_FLOW_ALTERNATIVE_HH */
