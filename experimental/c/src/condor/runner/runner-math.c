#include "runner-math.h"

int MathBooleanBoolean(bool a, bool b, Token op) {
  int type = (int) op;
  int x = (int)a;
  int y = (int)b;
  switch (type) {
    case ADD: {
      return x + y;
    }
    case SUB: {
      return x - y;
    }
    case MUL: {
      return x * y;
    }
    case DIV: {
      return x / y;
    }
  }

  RUNTIME_ERROR("Invalid operator")
}