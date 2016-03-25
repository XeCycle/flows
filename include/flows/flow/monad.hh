#ifndef FLOWS_FLOW_MONAD_HH
#define FLOWS_FLOW_MONAD_HH

#include <tuple>

#include <flows/flow/category.hh>
#include <flows/flow/arrow.hh>
#include <flows/flow/map.hh>

namespace flows {

template <class T, class In>
struct pure_flow_t {

  using input_type = In;
  using output_type = T;

  T v;

  template <class DS>
  auto accepting(DS& ds) const
    -> decltype(ds.accepting())
  {
    return ds.accepting();
  }

  template <class _In, class DS>
  require_convertible_to<_In, In>
  push(_In&&, DS& ds) const&
  {
    ds.push(v);
  }

};

template <class T>
struct unbound_pure_flow_t {

  T v;

  template <class In>
  using bound_flow = pure_flow_t<T, In>;

  template <class In>
  bound_flow<In>
  constexpr bind() const&
  {
    return { v };
  }

#if __cplusplus >= 201402L

  template <class In>
  bound_flow<In>
  constexpr bind() &&
  {
    return { (T&&)v };
  }

#endif

};

template <class In>
struct flatten_flow_val_pair_flow_t {

  using input_type = In;
  using output_type = typename std::tuple_element<0, In>::type::output_type;

  static_assert(std::is_convertible<
                  typename std::tuple_element<1, In>::type,
                  typename std::tuple_element<0, In>::type::input_type
                >::value,
                "Cannot push input to this flow");

  template <class DS>
  auto accepting(DS& ds)
    -> decltype(ds.accepting())
  {
    return ds.accepting();
  }

  template <class DS>
  void push(const In& pair, DS& ds)
  {
    auto& f = std::get<0>(pair);
    auto& x = std::get<1>(pair);
    if (f.accepting(ds))
      f.push(x, ds);
  }

};

template <class In>
struct flatten_unbound_flow_val_pair_flow_t {

  using input_type = In;

private:
  using flow_type = typename std::tuple_element<0, In>::type;
  using value_type = typename std::tuple_element<1, In>::type;
  using bound_flow_type = typename flow_type::template bound_flow<value_type>;

public:
  using output_type = typename bound_flow_type::output_type;

  static_assert(std::is_convertible<
                  typename std::tuple_element<1, In>::type,
                  typename bound_flow_type::input_type
                >::value,
                "Cannot push input to this flow");

  template <class DS>
  auto accepting(DS& ds)
    -> decltype(ds.accepting())
  {
    return ds.accepting();
  }

  template <class DS>
  void push(const In& pair, DS& ds)
  {
    bound_flow_type f = std::get<0>(pair).template bind<value_type>();
    auto& x = std::get<1>(pair);
    if (f.accepting(ds))
      ((bound_flow_type&&)f).push(x, ds);
  }

};

struct unbound_flatten_flow_val_pair_flow_t {

private:

  template <class In>
  using flow_type = typename std::tuple_element<0, In>::type;

public:

  template <class In>
  using bound_flow = typename std::conditional<
    is_flow<flow_type<In> >::value,
    flatten_flow_val_pair_flow_t<In>,
    typename std::enable_if<
      is_unbound_flow<flow_type<In> >::value,
      flatten_unbound_flow_val_pair_flow_t<In>
    >::type
  >::type;

  template <class In>
  bound_flow<In>
  constexpr bind() const
  {
    return {};
  }

};

template <class F>
constexpr auto join(F&& f)
  -> typename std::enable_if<
       (is_flow<F>::value || is_unbound_flow<F>::value),
       decltype(to_cref_pair() >> first((F&&)f) >> unbound_flatten_flow_val_pair_flow_t())
     >::type
{
  return to_cref_pair() >> first((F&&)f) >> unbound_flatten_flow_val_pair_flow_t();
}

} // namespace flows

#endif /* FLOWS_FLOW_MONAD_HH */
