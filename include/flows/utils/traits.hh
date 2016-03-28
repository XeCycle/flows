#ifndef FLOWS_UTILS_TRAITS_HH
#define FLOWS_UTILS_TRAITS_HH

#include <type_traits>
#include <utility>

namespace flows {

template <class T, class...>
struct require_types {
  using type = T;
};

template <class T, template <class...> class...>
struct require_templates {
  using type = T;
};

template <class T, class U, class result=void>
struct decays_to : std::enable_if<std::is_same<typename std::decay<T>::type, U>::value, result>
{};

template <class T, class U, class result=void>
using require_decays_to = typename decays_to<T, U, result>::type;

template <class T, class U, class result=void>
using require_convertible_to = typename std::enable_if<
  std::is_convertible<T, U>::value,
  result
>::type;

// forward while allowing implicit conversion
//
// do a static_cast to this type; cannot make a helper function because the
// temporary created for conversion would be destructed when the function
// returns
template <class From, class To>
using cforward_t = typename std::conditional<
  std::is_same<typename std::decay<From>::type, To>::value,
  From&&, To&&
>::type;

// add const to lref
template <class T>
struct add_const_to_lref {
  using type = T;
};

template <class T>
struct add_const_to_lref<T&> {
  using type = const T&;
};

// read-only converting forward
template <class From, class To>
using rcforward_t = typename add_const_to_lref<cforward_t<From, To> >::type;

// unbound flows require member template bound_flow
namespace traits_impl {

template <class UnboundFlow>
typename require_templates<std::true_type, UnboundFlow::template bound_flow>::type
check_is_unbound_flow(UnboundFlow&&);

template <class... T>
std::false_type check_is_unbound_flow(T&&...);

template <class T>
using is_unbound_flow = decltype(traits_impl::check_is_unbound_flow(std::declval<T>()));

} // namespace traits_impl

template <class T>
using is_unbound_flow = traits_impl::is_unbound_flow<typename std::decay<T>::type>;

template <class T>
using require_unbound_flow = typename std::enable_if<is_unbound_flow<T>::value>::type;

// unbound sinks require member template bound_sink
namespace traits_impl {

template <class UnboundSink>
typename require_templates<std::true_type, UnboundSink::template bound_sink>::type
check_is_unbound_sink(UnboundSink&&);

template <class... T>
std::false_type check_is_unbound_sink(T&&...);

template <class T>
using is_unbound_sink = decltype(traits_impl::check_is_unbound_sink(std::declval<T>()));

} // namespace traits_impl

template <class T>
using is_unbound_sink = traits_impl::is_unbound_sink<typename std::decay<T>::type>;

template <class T>
using require_unbound_sink = typename std::enable_if<is_unbound_sink<T>::value>::type;

// bound flows require member type input_type and output_type
namespace traits_impl {

template <class Flow>
typename require_types<
  std::true_type,
  typename Flow::input_type, typename Flow::output_type>::type
check_is_flow(Flow&&);

template <class... T>
std::false_type check_is_flow(T&&...);

template <class T>
using is_flow = decltype(traits_impl::check_is_flow(std::declval<T>()));

} // namespace traits_impl

template <class T>
using is_flow = traits_impl::is_flow<typename std::decay<T>::type>;

template <class T>
using require_flow = typename std::enable_if<is_flow<T>::value>::type;

// bound sinks require bool(.accepting()) and .push(const input_type&)
namespace traits_impl {

template <class Sink>
auto check_is_sink(Sink&&)
  -> decltype(bool(std::declval<Sink&>().accepting()),
              std::declval<Sink&>().push(std::declval<typename Sink::input_type const&>()),
              std::true_type());

template <class... T>
std::false_type check_is_sink(T&&...);

template <class T>
using is_sink = decltype(traits_impl::check_is_sink(std::declval<T>()));

} // namespace traits_impl

template <class T>
using is_sink = traits_impl::is_sink<typename std::decay<T>::type>;

template <class T>
using require_sink = typename std::enable_if<is_sink<T>::value>::type;

} // namespace flows

#endif /* FLOWS_UTILS_TRAITS_HH */
