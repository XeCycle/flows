#ifndef FLOWS_FLOW_ARROW_HH
#define FLOWS_FLOW_ARROW_HH

// conceptually, just Kleisli []

#include <type_traits>
#include <utility>
#include <functional>

#include <flows/utils/traits.hh>

namespace flows {

// arr is just map

template <class F, class In>
struct first_flow_t {

private:
  using first_type = typename std::decay<decltype(std::get<0>(std::declval<In>()))>::type;
  using second_type = typename std::decay<decltype(std::get<1>(std::declval<In>()))>::type;

public:

  using input_type = In;
  using output_type = std::pair<typename F::output_type, second_type>;

  static_assert(std::is_convertible<first_type, typename F::input_type>::value,
                "Cannot send first part to this flow");

  F f;

private:

  template <class DS, class second_t> // second_t may be const
  struct recombine_proxy_t {

    using input_type = typename F::output_type;

    second_t* psecond;
    DS* ds;

    auto accepting() const
      -> decltype(ds->accepting())
    {
      return ds->accepting();
    }

    template <class _In>
    require_convertible_to<_In, input_type>
    push(_In&& x) const
    {
      ds->push(std::make_pair((cforward_t<_In, input_type>)x, *psecond));
    }

  };

public:

  template <class DS>
  auto accepting(DS& ds)
    -> decltype(f.accepting(std::declval<recombine_proxy_t<DS, second_type>&>()))
  {
    recombine_proxy_t<DS, second_type> proxy { 0, &ds };
    return f.accepting(proxy);
  }

  template <class _In, class DS>
  require_convertible_to<_In, In>
  push(_In&& x, DS& ds)
  {
    recombine_proxy_t<
      DS,
      typename std::remove_pointer<decltype(std::addressof(std::get<1>(x)))>::type>
      proxy { std::addressof(std::get<1>(x)), std::addressof(ds) };
    f.push(std::get<0>((cforward_t<_In, In>)x), proxy);
  }

};

// where F inside is bound but no idea about second
template <class F>
struct f_bound_unbound_first_flow_t {

  F f;

  template <class In>
  using bound_flow = first_flow_t<F, In>;

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

// the F inside is also unbound
template <class F>
struct unbound_first_flow_t {

  F f;

private:
  template <class In>
  using first_type_of = typename std::decay<decltype(std::get<0>(std::declval<In&>()))>::type;

public:

  template <class In>
  using bound_flow = first_flow_t<typename F::template bound_flow<first_type_of<In> >, In>;

  template <class In>
  bound_flow<In>
  constexpr bind() const&
  {
    return { f.template bind<first_type_of<In> >() };
  }

#if __cplusplus >= 201402L

  template <class In>
  bound_flow<In>
  constexpr bind() &&
  {
    return { ((F&&)f).template bind<first_type_of<In> >() };
  }

#endif

};

template <class F>
typename require_types<
  unbound_first_flow_t<typename std::decay<F>::type>,
  require_unbound_flow<F>
>::type
constexpr first(F&& f)
{
  return { (F&&)f };
}

template <class F>
typename require_types<
  f_bound_unbound_first_flow_t<typename std::decay<F>::type>,
  require_flow<F>
>::type
constexpr first(F&& f)
{
  return { (F&&)f };
}

template <class F, class In>
struct second_flow_t {

private:
  using first_type = typename std::decay<decltype(std::get<0>(std::declval<In>()))>::type;
  using second_type = typename std::decay<decltype(std::get<1>(std::declval<In>()))>::type;

public:

  using input_type = In;
  using output_type = std::pair<first_type, typename F::output_type>;

  static_assert(std::is_convertible<second_type, typename F::input_type>::value,
                "Cannot send first part to this flow");

  F f;

private:

  template <class DS, class first_t> // first_t may be const
  struct recombine_proxy_t {

    using input_type = typename F::output_type;

    first_t* pfirst;
    DS* ds;

    auto accepting() const
      -> decltype(ds->accepting())
    {
      return ds->accepting();
    }

    template <class _In>
    require_convertible_to<_In, input_type>
    push(_In&& x) const
    {
      ds->push(std::make_pair(*pfirst, (cforward_t<_In, input_type>)x));
    }

  };

public:

  template <class DS>
  auto accepting(DS& ds)
    -> decltype(f.accepting(std::declval<recombine_proxy_t<DS, second_type>&>()))
  {
    recombine_proxy_t<DS, second_type> proxy { 0, &ds };
    return f.accepting(proxy);
  }

  template <class _In, class DS>
  require_convertible_to<_In, In>
  push(_In&& x, DS& ds)
  {
    recombine_proxy_t<
      DS,
      typename std::remove_pointer<decltype(std::addressof(std::get<0>(x)))>::type>
      proxy { std::addressof(std::get<0>(x)), std::addressof(ds) };
    f.push(std::get<1>((cforward_t<_In, In>)x), proxy);
  }

};

// where F inside is bound but no idea about second
template <class F>
struct f_bound_unbound_second_flow_t {

  F f;

  template <class In>
  using bound_flow = second_flow_t<F, In>;

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

// the F inside is also unbound
template <class F>
struct unbound_second_flow_t {

  F f;

private:
  template <class In>
  using second_type_of = typename std::decay<decltype(std::get<1>(std::declval<In&>()))>::type;

public:

  template <class In>
  using bound_flow = second_flow_t<typename F::template bound_flow<second_type_of<In> >, In>;

  template <class In>
  bound_flow<In>
  constexpr bind() const&
  {
    return { f.template bind<second_type_of<In> >() };
  }

#if __cplusplus >= 201402L

  template <class In>
  bound_flow<In>
  constexpr bind() &&
  {
    return { ((F&&)f).template bind<second_type_of<In> >() };
  }

#endif

};

template <class F>
typename require_types<
  unbound_second_flow_t<typename std::decay<F>::type>,
  require_unbound_flow<F>
>::type
constexpr second(F&& f)
{
  return { (F&&)f };
}

template <class F>
typename require_types<
  f_bound_unbound_second_flow_t<typename std::decay<F>::type>,
  require_flow<F>
>::type
constexpr second(F&& f)
{
  return { (F&&)f };
}

template <class In>
struct to_cref_pair_flow_t {

  using input_type = In;
  using output_type = std::pair<std::reference_wrapper<const In>,
                                std::reference_wrapper<const In> >;

  template <class DS>
  auto accepting(DS& ds) const
    -> decltype(ds.accepting())
  {
    return ds.accepting();
  }

  template <class DS>
  void push(const In& x, DS& ds)
  {
    ds.push(std::make_pair(std::cref(x), std::cref(x)));
  }

};

struct unbound_to_cref_pair_flow_t {

  template <class In>
  using bound_flow = to_cref_pair_flow_t<In>;

  template <class In>
  bound_flow<In>
  constexpr bind() const
  {
    return {};
  }

};

unbound_to_cref_pair_flow_t
constexpr to_cref_pair()
{
  return {};
}

} // namespace flows

#endif /* FLOWS_FLOW_ARROW_HH */
