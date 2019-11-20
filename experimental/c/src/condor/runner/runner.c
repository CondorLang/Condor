#include "runner.h"

void InitRunner(Runner* runner, Scope* scope) {
  runner->scope = scope;
  runner->contextSpot = 0;
  for (int i = 0; i < runner->totalContexts; i++){
    RunnerContext* context = &runner->contexts[i];
    context->node = NULL;
    context->dataType = UNDEFINED;
  }
}

RunnerContext* Run(Runner* runner, int scopeId) {
  DEBUG_PRINT_RUNNER("Scope")
  Scope* scope = runner->scope;
  for (int i = 0; i < scope->nodeLength; i++){
    ASTNode* node = &scope->nodes[i];
    if (node->isStmt && node->scopeId == scopeId){
      runner->currentNode = node;
      RunnerContext* context = RunStatement(runner);
      
      if (node->type == RETURN){
        return context;
      }

      if (node->type != FUNC) {
        PrintContext(context);
      }
    }
  }

  return NULL;
}

RunnerContext* RunStatement(Runner* runner){
  ASTNode* node = runner->currentNode;
  int type = (int) node->type;

  switch (type) {
    case FUNC_CALL: {
      return RunFuncCall(runner);
    }
    case RETURN: {
      runner->currentNode = node->meta.returnStmt.value;
      return RunStatement(runner);
    }
    case BINARY: {
      return RunBinary(runner);
    }
  }

  return SetNodeValue(runner, node);
}

RunnerContext* RunBinary(Runner* runner){
  DEBUG_PRINT_RUNNER("Binary expression")
  ASTNode* binary = runner->currentNode;
  ASTNode* left = binary->meta.binaryExpr.left;
  ASTNode* right = binary->meta.binaryExpr.right;
  Token op = binary->meta.binaryExpr.op;

  RunnerContext* leftContext = SetNodeValue(runner, left);
  runner->currentNode = right;
  RunnerContext* rightContext = RunStatement(runner);

  return RunMathContexts(leftContext, rightContext, op);
}

RunnerContext* RunMathContexts(RunnerContext* left, RunnerContext* right, Token op){
  DEBUG_PRINT_RUNNER("Math")

  if (left->dataType == STRING || right->dataType == STRING){
    NOT_IMPLEMENTED("String concatenation");
  }

  if (left->dataType == STRING || left->dataType == CHAR ||
      right->dataType == STRING || right->dataType == CHAR) {
    NOT_IMPLEMENTED("String comparisons not implemented")      
  }
  else {
    double leftVal = ContextToDouble(left);
    double rightVal = ContextToDouble(right);

    // TODO: Figure out true type
    left->value.vDouble = RunMath(leftVal, rightVal, op);
    CastToType(left);
  }

  return left;
}

RunnerContext* RunFuncCall(Runner* runner){
  DEBUG_PRINT_RUNNER("Function Call")
  ASTList* params = GET_FUNC_CALL_PARAMS(runner->currentNode);
  ASTNode* func = GET_FUNC_CALL_FUNC(runner->currentNode);
  return RunFuncWithArgs(runner, func, params);
}

RunnerContext* RunFuncWithArgs(Runner* runner, ASTNode* func, ASTList* args){
  DEBUG_PRINT_RUNNER("Function Scope")

  int argIdx = 0;
  FOREACH_AST(args){
    ASTNode* arg = item->node;
    int paramIdx = 0;
    RunnerContext* context = SetNodeValue(runner, arg);

    FOREACH_AST(GET_FUNC_PARAMS(func)){
      if (argIdx == paramIdx++){
        ASTNode* param = item->node;
        context->node = param;
        break;
      }
    }

    argIdx++;
  }

  return Run(runner, func->meta.funcExpr.body);
}

RunnerContext* SetNodeValue(Runner* runner, ASTNode* node){
  DEBUG_PRINT_RUNNER("Set node value")

  RunnerContext* context = GetContextByNodeId(runner, node->id);
  if (context != NULL){
    return context;
  }

  context = GetNextContext(runner);
  context->node = node;

  int type = (int) node->type;
  switch (type) {
    case BOOLEAN: {
      context->dataType = node->type;
      context->value.vBoolean = node->meta.booleanExpr.value;
      break;
    }
    case BYTE: {
      context->dataType = BYTE;
      context->value.vByte = node->meta.byteExpr.value;
      break;
    }
    case SHORT: {
      context->dataType = SHORT;
      context->value.vShort = node->meta.shortExpr.value;
      break;
    }
    case INT: {
      context->dataType = INT;
      context->value.vInt = node->meta.intExpr.value;
      break;
    }
    case FLOAT: {
      context->dataType = FLOAT;
      context->value.vFloat = node->meta.floatExpr.value;
      break;
    }
    case DOUBLE: {
      context->dataType = DOUBLE;
      context->value.vDouble = node->meta.doubleExpr.value;
      break;
    }
    case LONG: {
      context->dataType = LONG;
      context->value.vLong = node->meta.longExpr.value;
      break;
    }
    case CHAR: {
      context->dataType = CHAR;
      context->value.vChar = node->meta.charExpr.value;
      break;
    }
    case STRING: {
      context->dataType = STRING;
      context->value.vString = node->meta.stringExpr.value;
      break;
    }
    case VAR: {
      RunSetVarType(runner, context, node);
      break;
    }
  }

  return context;
}

void RunSetVarType(Runner* runner, RunnerContext* context, ASTNode* node){
  int type = (int) node->meta.varExpr.dataType;
  switch (type) {
    case BOOLEAN: {
      context->dataType = BOOLEAN;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.vBoolean = varContext->value.vBoolean;
      break;
    }
    case BYTE: {
      context->dataType = BYTE;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.vByte = varContext->value.vByte;
      break;
    }
    case SHORT: {
      context->dataType = SHORT;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.vShort = varContext->value.vShort;
      break;
    }
    case INT: {
      context->dataType = INT;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.vInt = varContext->value.vInt;
      break;
    }
    case FLOAT: {
      context->dataType = FLOAT;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.vFloat = varContext->value.vFloat;
      break;
    }
    case DOUBLE: {
      context->dataType = DOUBLE;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.vDouble = varContext->value.vDouble;
      break;
    }
    case LONG: {
      context->dataType = LONG;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.vLong = varContext->value.vLong;
      break;
    }
    case CHAR: {
      context->dataType = CHAR;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.vChar = varContext->value.vChar;
      break;
    }
    case STRING: {
      context->dataType = STRING;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.vString = varContext->value.vString;
      break;
    }
  }
}

RunnerContext* GetContextByNodeId(Runner* runner, int nodeId){
  for (int i = 0; i < runner->totalContexts; i++){
    RunnerContext* context = &runner->contexts[i];
    if (context->node != NULL && context->node->id == nodeId){
      return &runner->contexts[i];
    }
  }

  return NULL;
}

RunnerContext* GetNextContext(Runner* runner) {
  if (runner->contextSpot + 1 > runner->totalContexts) {
    RUNTIME_ERROR("Ran out of contexts");
  }
  return &runner->contexts[runner->contextSpot++];
}

void PrintContext(RunnerContext* context){
  int type = (int) context->dataType;
  switch (type) {
    case BOOLEAN: {
      printf("%d\n", context->value.vBoolean);
      break;
    }
    case BYTE: {
      printf("%d\n", context->value.vByte);
      break;
    }
    case SHORT: {
      printf("%d\n", context->value.vShort);
      break;
    }
    case INT: {
      printf("%d\n", context->value.vInt);
      break;
    }
    case FLOAT: {
      printf("%f\n", context->value.vFloat);
      break;
    }
    case DOUBLE: {
      printf("%f\n", context->value.vDouble);
      break;
    }
    case LONG: {
      printf("%ld\n", context->value.vLong);
      break;
    }
    case CHAR: {
      printf("%d\n", context->value.vChar);
      break;
    }
    case STRING: {
      printf("%s\n", context->value.vString);
      break;
    }
  }
}