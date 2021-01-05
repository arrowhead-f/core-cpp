#ifndef _ARROWHEAD_ASSERT_H_
#define _ARROWHEAD_ASSERT_H_

#include <cassert>

#define PP_THIRD_ARG(a,b,c,...) c
#define VA_OPT_SUPPORTED_I(...) PP_THIRD_ARG(__VA_OPT__(,),true,false,)
#define VA_OPT_SUPPORTED VA_OPT_SUPPORTED_I(?)

#define precondition(expr, ...)  assert((expr) && "Precondition failed." __VA_ARGS__);
#define postcondition(expr, ...) assert((expr) && "Postcondition failed." __VA_ARGS__);
#if VA_OPT_SUPPORTED
  #define assertion(expr, ...)     assert((expr) __VA_OPT__(&&) __VA_ARGS__);
#else
  #define assertion(expr, ...)     assert((expr) && __VA_ARGS__);
#endif

// Example
//
//int func(char *ptr) {
//    precondition(ptr, "Shouldn't be nullptr.");
//
//    assertion(5 == 6)
//    assertion(5 == 6, "Almafa");
//
//    postcondition(ptr);
//}

#endif  /* _ARROWHEAD_ASSERT_H_ */
