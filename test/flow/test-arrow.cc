#include <cassert>

#include <type_traits>
#include <utility>

#include <flows/flow/arrow.hh>
#include <flows/flow/map.hh>
#include <flows/flow/category.hh>

int main()
{

  namespace F = flows;

  auto f = F::map([](int x) { return x+1; });
  auto g = F::map([](double x) { return x+x; });

  auto arrow = (first(f) >> second(g)).bind<std::pair<int, double> >();

  struct check {

    using input_type = std::pair<int, double>;

    constexpr std::true_type accepting() const
    {
      return {};
    }

    void push(const input_type& in)
    {
      assert(in == (std::pair<int, double> { 3, 4.0 }));
    }

  };

  check c;
  arrow.push(std::make_pair(2, 2.0), c);

  return 0;
}
