#include "runner.h"

void InitRunner(Runner* runner, Scope* scope) {
  runner->scope = scope;
  runner->contextSpot = 0;
}

void Run(Runner* runner, int scopeId) {
  DEBUG_PRINT_RUNNER("Scope")
  Scope* scope = runner->scope;
  for (int i = 0; i < scope->nodeLength; i++){
    ASTNode* node = &scope->nodes[i];
    if (node->isStmt && node->scopeId == scopeId){
      runner->currentNode = node;
      RunStatement(runner);
    }
  }
}

void RunStatement(Runner* runner){
  ASTNode* node = runner->currentNode;
  int type = (int) node->type;

  switch (type) {
    case FUNC_CALL: {
      return RunFuncCall(runner);
    }
  }
}

void RunFuncCall(Runner* runner){
  DEBUG_PRINT_RUNNER("Function Call")
  ASTList* params = GET_FUNC_CALL_PARAMS(runner->currentNode);
  ASTNode* func = GET_FUNC_CALL_FUNC(runner->currentNode);
  RunFuncWithArgs(runner, func, params);
}

void RunFuncWithArgs(Runner* runner, ASTNode* func, ASTList* args){
  DEBUG_PRINT_RUNNER("Function Scope")
  FOREACH_AST(args){
    ASTNode* arg = item->node;
    SetNodeValue(runner, arg);
  }

  Run(runner, func->meta.funcExpr.body);
}

void SetNodeValue(Runner* runner, ASTNode* node){
  DEBUG_PRINT_RUNNER("Set node value")
  RunnerContext* context = GetNextContext(runner);
  context->node = node;
  context->dataType = GET_VAR_TYPE(node);
  int type = (int) context->dataType;
  switch (type) {
    case BOOLEAN: {
      context->value.booleanValue.value = 1;
      break;
    }
    case BYTE: {
      context->value.byteValue.value = 1;
      break;
    }
    case SHORT: {
      context->value.shortValue.value = 1;
      break;
    }
    case INT: {
      context->value.intValue.value = 1;
      break;
    }
    case FLOAT: {
      context->value.floatValue.value = 1;
      break;
    }
    case DOUBLE: {
      context->value.doubleValue.value = 1;
      break;
    }
    case LONG: {
      context->value.longValue.value = 1;
      break;
    }
    case CHAR: {
      context->value.charValue.value = 1;
      break;
    }
    case STRING: {
      context->value.stringValue.value = "";
      break;
    }
  }
}

RunnerContext* GetNextContext(Runner* runner) {
  if (runner->contextSpot + 1 > runner->totalContexts) {
    RUNTIME_ERROR("Ran out of contexts");
  }
  return &runner->contexts[runner->contextSpot++];
}