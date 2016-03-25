#include <cassert>

#include <vector>
#include <iterator>

#include <flows/flow/map.hh>
#include <flows/flow/monad.hh>
#include <flows/sink/iterator.hh>

int main()
{

  namespace F = flows;
  using namespace F::operators;

  auto f = F::map([](int x) { return F::map([=](int y) { return x+y; }); });

  std::vector<int> out;

  auto sink = (F::join(f) >> F::iterator_sink(std::back_inserter(out))).bind<int>();

  for (int i=0; sink.accepting() && i<5; ++i)
    sink.push(i);

  assert(out == (std::vector<int> { 0, 2, 4, 6, 8 }));

  return 0;
}
