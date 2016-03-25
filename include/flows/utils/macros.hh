#ifndef FLOWS_UTILS_MACROS_HH
#define FLOWS_UTILS_MACROS_HH

// declare an overload set with op_f_t and op_f
#define FLOWS_OP_HELPER(f)                              \
  struct op_##f##_t {                                   \
    template <class... Arg>                             \
    constexpr auto operator()(Arg&&... arg) const       \
      -> decltype(f((Arg&&)arg...))                     \
    {                                                   \
      return f((Arg&&)arg...);                          \
    }                                                   \
  };                                                    \
  constexpr op_##f##_t op_##f {}

#endif /* FLOWS_UTILS_MACROS_HH */
