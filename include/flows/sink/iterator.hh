#ifndef FLOWS_SINK_ITERATOR_HH
#define FLOWS_SINK_ITERATOR_HH

#include <type_traits>

#include <flows/utils/traits.hh>

namespace flows {

template <class In, class It>
struct iterator_sink_t {

  using input_type = In;

private:

  template <class T, class=decltype(*std::declval<It&>() = std::declval<const T&>(),
                                    ++std::declval<It&>())>
  static std::true_type can_write(T*);
  static std::false_type can_write(...);

  static_assert(decltype(can_write((In*)0))::value,
                "Cannot write input to this iterator");

public:

  It it;

  std::true_type
  constexpr accepting() const
  {
    return {};
  }

  template <class _In>
  require_convertible_to<_In, In>
  push(_In&& x)
  {
    *it = (rcforward_t<_In, In>)x, ++it;
  }

};

template <class It>
struct unbound_iterator_sink_t {

  It it;

  template <class In>
  using bound_sink = iterator_sink_t<In, It>;

  template <class In>
  bound_sink<In>
  constexpr bind() const&
  {
    return { it };
  }

#if __cplusplus >= 201402L

  template <class In>
  bound_sink<In>
  constexpr bind() &&
  {
    return { (It&&)it };
  }

#endif

};

template <class It>
unbound_iterator_sink_t<typename std::decay<It>::type>
constexpr iterator_sink(It&& it)
{
  return { (It&&)it };
}

} // namespace flows

#endif /* FLOWS_SINK_ITERATOR_HH */
