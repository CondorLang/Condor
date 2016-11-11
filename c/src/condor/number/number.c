#include "number.h"

Token GetNumberType(char* value){
	Token type = INT;

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
	if (val == 0 || val == 1) return BOOLEAN;
	if (val == (int) val){
		if (val < byte_max && val > byte_min) return BYTE;
		if (val < short_max && val > short_min) return SHORT;
		if (val < int_max && val > int_min) return INT;
		if (val < long_max && val > long_min) return FLOAT;
	}
	else{
		NOT_IMPLEMENTED("Float and Double");
	}

	return type;
}