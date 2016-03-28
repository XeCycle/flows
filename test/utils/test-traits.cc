#include <flows/utils/traits.hh>

int main()
{

  using namespace flows;

  // add_const_to_lref
  static_assert(std::is_same<typename add_const_to_lref<int&>::type, const int&>::value,
                "on lref -> const lref");
  static_assert(std::is_same<typename add_const_to_lref<const int&>::type, const int&>::value,
                "on const lref -> const lref");
  static_assert(std::is_same<typename add_const_to_lref<int&&>::type, int&&>::value,
                "on rref -> rref");
  static_assert(std::is_same<typename add_const_to_lref<const int&&>::type, const int&&>::value,
                "on const rref -> const rref");

  return 0;
}
