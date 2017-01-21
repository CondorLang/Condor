#include "typechecker.h"

Token GetBinaryType(ASTNode* node){
	CHECK(node->type == BINARY);
	ASTNode* leftNode = GET_BIN_LEFT(node);
	ASTNode* rightNode = GET_BIN_RIGHT(node);
	Token op = GET_BIN_OP(node);

	Token left = leftNode->type;
	Token right = rightNode->type;

	if (left == VAR) left = GET_VAR_TYPE(leftNode);
	if (right == VAR) right = GET_VAR_TYPE(rightNode);

	if (right == BINARY) right = GetBinaryType(rightNode);

	// Check number types
	if (IsNumber(left) && IsNumber(right) && IsBinaryOperator(op)){
		if (left > right) return left;
		return right;
	}

	if (left == STRING || right == STRING) {
		if (op != ADD && IsBinaryOperator(op)) SEMANTIC_OP_ERROR("Invalid operator against string", op);
		return STRING; // string dominates
	}

	NOT_IMPLEMENTED(TokenToString(op));
	return UNDEFINED;
}