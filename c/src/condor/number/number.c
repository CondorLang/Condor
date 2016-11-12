#include "number.h"

void SetNumberType(ASTNode* node, char* value){
	double byte_max = pow(2, 7);
	double short_max = pow(2, 15);
	double int_max = pow(2, 31);
	double long_max = pow(2, 63);

	double byte_min = -pow(2, 7);
	double short_min = -pow(2, 15);
	double int_min = -pow(2, 31);
	double long_min = -pow(2, 63);

	double val = atof(value);

	// Check for decimal
	if (val == 0 || val == 1) {
		node->meta.booleanExpr.value = (bool) val;
		node->type = BOOLEAN;
	}
	else if (val == (int) val){
		if (val < byte_max && val > byte_min) {
			node->meta.byteExpr.value = (signed char) val;
			node->type = BYTE;
		}
		else if (val < short_max && val > short_min) {
			node->meta.shortExpr.value = (short) val;
			node->type = SHORT;
		}
		else if (val < int_max && val > int_min) {
			node->meta.intExpr.value = (int) val;
			node->type = INT;
		}
		else if (val < long_max && val > long_min) {
			node->meta.longExpr.value = (long) val;
			node->type = LONG;
		}
	}
	else{
		NOT_IMPLEMENTED("Float and Double");
	}
}