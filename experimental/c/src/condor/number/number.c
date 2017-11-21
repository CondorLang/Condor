#include "number.h"

void SetNumberType(ASTNode* node, char* value){
	if (node == NULL) return;
	
	double val = atof(value);

	bool hasDecimal = false;
	int len = strlen(value);

	for (int i = 0; i < len; i++){
		if (value[i] == '.') {
			hasDecimal = true;
			break;
		}
	}

	// Check for decimal
	if (val == 0 || val == 1) {
		node->meta.booleanExpr.value = (bool) val;
		node->type = BOOLEAN;
	}
	else if (!hasDecimal){
		if (val < CHAR_MAX && val > CHAR_MIN) {
			node->meta.byteExpr.value = (signed char) val;
			node->type = BYTE;
		}
		else if (val < SHRT_MAX && val > SHRT_MIN) {
			node->meta.shortExpr.value = (short) val;
			node->type = SHORT;
		}
		else if (val < INT_MAX && val > INT_MIN) {
			node->meta.intExpr.value = (int) val;
			node->type = INT;
		}
		else if (val < LONG_MAX && val > LONG_MIN) {
			node->meta.longExpr.value = (long) val;
			node->type = LONG;
		}
	}
	else{
		if (val < FLT_MAX && val > -FLT_MAX){
			node->meta.floatExpr.value = (float) val;
			node->type = FLOAT;
		}
		else if (val < DBL_MAX && val > -DBL_MAX){
			node->meta.doubleExpr.value = (double) val;
			node->type = DOUBLE;
		}
		else{
			NOT_IMPLEMENTED("Other Numer Type");
		}
	}
}