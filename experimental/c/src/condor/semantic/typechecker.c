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

	if (IsBooleanOperator(op)) return BOOLEAN;

	NOT_IMPLEMENTED(TokenToString(op));
	return UNDEFINED;
}

#ifdef IS_TEST
	void Test_GetBinaryType(){
		ASTNode node;
		node.type = BINARY;

		ASTNode left;
		ASTNode right;

		Token numberTypes[] = {BOOLEAN, BYTE, SHORT, INT, FLOAT, DOUBLE, LONG, DOUBLE, CHAR, STRING};
		Token operators[] = {LAND, LOR, EQL, NEQ, LEQ, GEQ, NOT, LESS, GREATER};

		for (int i = 0; i < 8; i++){
			for (int j = 0; j < 8; j++){
				left.type = numberTypes[i];
				right.type = numberTypes[j];
				node.meta.binaryExpr.left = &left;
				node.meta.binaryExpr.right = &right;
				for (int k = 0; k < 9; k++){
					node.meta.binaryExpr.op = operators[k];
					Token result = GetBinaryType(&node);
					if (result != BOOLEAN || result == UNDEFINED) FAILED_TEST3(TokenToString(numberTypes[i]), TokenToString(numberTypes[j]), TokenToString(operators[k]));
				}
			}
		}
		SUCCESS_TEST("Binary Types")
	}
#endif