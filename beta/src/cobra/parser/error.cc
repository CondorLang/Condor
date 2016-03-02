#include <string>

#include "error.h"

#define ERR(name, string) string,
  const char* const Error::kNames[NUM_ERRORS] = {
  	ERR_LIST(ERR)
  };
#undef ERR