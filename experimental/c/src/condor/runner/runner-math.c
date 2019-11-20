#include "runner-math.h"

double ContextToDouble(RunnerContext* context){
  int type = (int) context->dataType;
  switch (type) {
    case BOOLEAN: return (double) context->value.vBoolean;
    case BYTE: return (double) context->value.vByte;
    case SHORT: return (double) context->value.vShort;
    case INT: return (double) context->value.vInt;
    case FLOAT: return (double) context->value.vFloat;
    case DOUBLE: return (double) context->value.vDouble;
    case LONG: return (double) context->value.vLong;
  }

  RUNTIME_ERROR("Invalid operator")
}

double RunMath(double left, double right, Token op){
  int type = (int) op;
  switch (type) {
    case ADD: return left + right;
    case SUB: return left - right;
    case MUL: return left * right;
    case DIV: return left * right;
  }

  RUNTIME_ERROR("Invalid operator for math")
}

void CastToType(RunnerContext* context){
  double num = context->value.vDouble;

  if (num == 0 || num == 1) {
    context->value.vBoolean = (bool) num;
    context->dataType = BOOLEAN;
  }
  else if (INT8_MIN <= num && INT8_MAX >= num) {
    context->value.vByte = (unsigned char) num;
    context->dataType = BYTE;
  }
  else if (INT16_MIN <= num && INT16_MAX >= num) {
    context->value.vShort = (short) num;
    context->dataType = SHORT;
  }
  else if (INT32_MIN <= num && INT32_MAX >= num) {
    context->value.vShort = (int) num;
    context->dataType = INT;
  }
  else if (INT64_MIN <= num && INT64_MAX >= num) {
    context->value.vLong = (long) num;
    context->dataType = LONG;
  }
  else if (FLT_MIN <= num && FLT_MAX >= num) {
    context->value.vFloat = (float) num;
    context->dataType = FLOAT;
  }
  else if (DBL_MIN <= num && DBL_MAX >= num) {
    context->value.vFloat = (double) num;
    context->dataType = DOUBLE;
  }
  else {
    RUNTIME_ERROR("Unknown data type")
  }
}