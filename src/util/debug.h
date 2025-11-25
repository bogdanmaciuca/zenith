#pragma once
#include "io.h"

#ifdef ZN_DEBUG
#define ASSERT(condition, message) \
  if (!(bool)(condition)) {        \
    io_print_assertion(            \
      #condition,                  \
      __PRETTY_FUNCTION__,         \
      __FILE__,                    \
      __LINE__,                    \
      message                      \
    );                             \
    exit(1);                       \
  }
#else
#define ASSERT(condition message) (void(0));
#endif // ZN_DEBUG


