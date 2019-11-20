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

  return NULL;
}

RunnerContext* RunBinary(Runner* runner){
  DEBUG_PRINT_RUNNER("Binary expression")
  ASTNode* binary = runner->currentNode;
  ASTNode* left = binary->meta.binaryExpr.left;
  ASTNode* right = binary->meta.binaryExpr.right;
  Token op = binary->meta.binaryExpr.op;

  RunnerContext* leftContext = SetNodeValue(runner, left);
  RunnerContext* rightContext = SetNodeValue(runner, right);

  return RunMathContexts(leftContext, rightContext, op);
}

RunnerContext* RunMathContexts(RunnerContext* left, RunnerContext* right, Token op){
  DEBUG_PRINT_RUNNER("Math")

  if (left->dataType == STRING || right->dataType == STRING){
    RUNTIME_ERROR("NOT IMPLEMENTED");
  }

  if (left->dataType == BOOLEAN && right->dataType == BOOLEAN) {
    left->value.intValue.value = MathBooleanBoolean(CONTEXT_BOOLEAN_VALUE(left), CONTEXT_BOOLEAN_VALUE(right), op);
    left->dataType = INT;
  }
  else {
    NOT_IMPLEMENTED("Math between other datatypes");
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
      context->dataType = BOOLEAN;
      context->value.booleanValue.value = node->meta.booleanExpr.value;
      break;
    }
    case BYTE: {
      context->dataType = BYTE;
      context->value.byteValue.value = node->meta.byteExpr.value;
      break;
    }
    case SHORT: {
      context->dataType = SHORT;
      context->value.shortValue.value = node->meta.shortExpr.value;
      break;
    }
    case INT: {
      context->dataType = INT;
      context->value.intValue.value = node->meta.intExpr.value;
      break;
    }
    case FLOAT: {
      context->dataType = FLOAT;
      context->value.floatValue.value = node->meta.floatExpr.value;
      break;
    }
    case DOUBLE: {
      context->dataType = DOUBLE;
      context->value.doubleValue.value = node->meta.doubleExpr.value;
      break;
    }
    case LONG: {
      context->dataType = LONG;
      context->value.longValue.value = node->meta.longExpr.value;
      break;
    }
    case CHAR: {
      context->dataType = CHAR;
      context->value.charValue.value = node->meta.charExpr.value;
      break;
    }
    case STRING: {
      context->dataType = STRING;
      context->value.stringValue.value = node->meta.stringExpr.value;
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
      context->value.booleanValue.value = varContext->value.booleanValue.value;
      break;
    }
    case BYTE: {
      context->dataType = BYTE;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.byteValue.value = varContext->value.byteValue.value;
      break;
    }
    case SHORT: {
      context->dataType = SHORT;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.shortValue.value = varContext->value.shortValue.value;
      break;
    }
    case INT: {
      context->dataType = INT;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.intValue.value = varContext->value.intValue.value;
      break;
    }
    case FLOAT: {
      context->dataType = FLOAT;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.floatValue.value = varContext->value.floatValue.value;
      break;
    }
    case DOUBLE: {
      context->dataType = DOUBLE;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.doubleValue.value = varContext->value.doubleValue.value;
      break;
    }
    case LONG: {
      context->dataType = LONG;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.longValue.value = varContext->value.longValue.value;
      break;
    }
    case CHAR: {
      context->dataType = CHAR;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.charValue.value = varContext->value.charValue.value;
      break;
    }
    case STRING: {
      context->dataType = STRING;
      RunnerContext* varContext = SetNodeValue(runner, node->meta.varExpr.value);
      context->value.stringValue.value = varContext->value.stringValue.value;
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
      printf("%d\n", context->value.booleanValue.value);
      break;
    }
    case BYTE: {
      printf("%d\n", context->value.byteValue.value);
      break;
    }
    case SHORT: {
      printf("%d\n", context->value.shortValue.value);
      break;
    }
    case INT: {
      printf("%d\n", context->value.intValue.value);
      break;
    }
    case FLOAT: {
      printf("%f\n", context->value.floatValue.value);
      break;
    }
    case DOUBLE: {
      printf("%f\n", context->value.doubleValue.value);
      break;
    }
    case LONG: {
      printf("%ld\n", context->value.longValue.value);
      break;
    }
    case CHAR: {
      printf("%d\n", context->value.charValue.value);
      break;
    }
    case STRING: {
      printf("%s\n", context->value.stringValue.value);
      break;
    }
  }
}