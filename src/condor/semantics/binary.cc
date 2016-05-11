// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "binary.h"

namespace Condor {
namespace internal{

	TOKEN Binary::Compare(TOKEN leftT, TOKEN right, TOKEN op){
		int left = (int) leftT;
		switch (left){
			case LITERAL: {
				if (op == PERIOD && right == OBJECT) return LITERAL;
				throw Error::INVALID_OPERATOR;
			}
			case INT: case BOOLEAN: case TRUE_LITERAL: case FALSE_LITERAL: case VAR: {
				if (op == ADD){
					if (right == INT) return INT;
					else if (right == DOUBLE) return DOUBLE;
					else if (right == LONG) return LONG;
					else if (right == FLOAT) return FLOAT;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return INT;
					else if (right == CHAR) return INT;
					else if (right == STRING) return STRING;
					else if (right == VAR) return VAR;
					else if (right == ARRAY) throw Error::CANNOT_ADD_TO_ARRAY;
					else {
						throw Error::INVALID_ADD_EXPR;
					}
					break;
				}
				else if (op == SUB){
					if (right == INT) return INT;
					else if (right == DOUBLE) return DOUBLE;
					else if (right == FLOAT) return FLOAT;
					else if (right == LONG) return LONG;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return INT;
					else if (right == VAR) return VAR;
					else if (right == CHAR) throw Error::CANNOT_SUB_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_SUB_FROM_ARRAY;
					else {
						throw Error::INVALID_SUB_EXPR;
					}
					break;
				}
				else if (op == DIV){
					if (right == INT) return INT;
					else if (right == DOUBLE) return DOUBLE;
					else if (right == FLOAT) return FLOAT;
					else if (right == LONG) return LONG;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return INT;
					else if (right == VAR) return VAR;
					else if (right == CHAR) throw Error::CANNOT_DIV_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_DIV_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_DIV_FROM_ARRAY;
					else {
						throw Error::INVALID_DIV_EXPR;
					}
					break;
				}
				else if (op == MUL){
					if (right == INT) return INT;
					else if (right == DOUBLE) return DOUBLE;
					else if (right == FLOAT) return FLOAT;
					else if (right == LONG) return LONG;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return INT;
					else if (right == VAR) return VAR;
					else if (right == CHAR) throw Error::CANNOT_MULT_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_MULT_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_MULT_FROM_ARRAY;
					else {
						throw Error::INVALID_MULT_EXPR;
					}
					break;
				}
				else if (op == MOD){
					if (right == INT) return INT;
					else if (right == DOUBLE) return DOUBLE;
					else if (right == FLOAT) return FLOAT;
					else if (right == LONG) return LONG;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return INT;
					else if (right == VAR) return VAR;
					else if (right == CHAR) throw Error::CANNOT_MOD_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_MOD_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_MOD_FROM_ARRAY;
					else {
						throw Error::INVALID_MOD_EXPR;
					}
					break;
				}
			}
			case DOUBLE: case LONG: {
				if (op == ADD){
					if (right == INT) return DOUBLE;
					else if (right == DOUBLE) return DOUBLE;
					else if (right == FLOAT) return FLOAT;
					else if (right == LONG) return LONG;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return DOUBLE;
					else if (right == CHAR) return DOUBLE;
					else if (right == STRING) return STRING;
					else if (right == ARRAY) throw Error::CANNOT_ADD_TO_ARRAY;
					else {
						throw Error::INVALID_ADD_EXPR;
					}
					break;
				}
				else if (op == SUB){
					if (right == INT) return DOUBLE;
					else if (right == DOUBLE) return DOUBLE;
					else if (right == FLOAT) return FLOAT;
					else if (right == LONG) return LONG;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return DOUBLE;
					else if (right == CHAR) throw Error::CANNOT_SUB_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_SUB_FROM_ARRAY;
					else {
						throw Error::INVALID_SUB_EXPR;
					}
					break;
				}
				else if (op == DIV){
					if (right == INT) return DOUBLE;
					else if (right == DOUBLE) return DOUBLE;
					else if (right == FLOAT) return FLOAT;
					else if (right == LONG) return LONG;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return DOUBLE;
					else if (right == CHAR) throw Error::CANNOT_DIV_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_DIV_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_DIV_FROM_ARRAY;
					else {
						throw Error::INVALID_DIV_EXPR;
					}
					break;
				}
				else if (op == MUL){
					if (right == INT) return DOUBLE;
					else if (right == DOUBLE) return DOUBLE;
					else if (right == FLOAT) return FLOAT;
					else if (right == LONG) return LONG;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return DOUBLE;
					else if (right == CHAR) throw Error::CANNOT_MULT_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_MULT_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_MULT_FROM_ARRAY;
					else {
						throw Error::INVALID_MULT_EXPR;
					}
					break;
				}
				else if (op == MOD){
					if (right == INT) return DOUBLE;
					else if (right == DOUBLE) return DOUBLE;
					else if (right == FLOAT) return FLOAT;
					else if (right == LONG) return LONG;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return DOUBLE;
					else if (right == CHAR) throw Error::CANNOT_MOD_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_MOD_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_MOD_FROM_ARRAY;
					else {
						throw Error::INVALID_MOD_EXPR;
					}
					break;
				}
			}
			case FLOAT: {
				if (op == ADD){
					if (right == INT) return FLOAT;
					else if (right == DOUBLE) return FLOAT;
					else if (right == FLOAT) return FLOAT;
					else if (right == LONG) return FLOAT;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return FLOAT;
					else if (right == CHAR) return FLOAT;
					else if (right == STRING) return STRING;
					else if (right == ARRAY) throw Error::CANNOT_ADD_TO_ARRAY;
					else {
						throw Error::INVALID_ADD_EXPR;
					}
					break;
				}
				else if (op == SUB){
					if (right == INT) return FLOAT;
					else if (right == DOUBLE) return FLOAT;
					else if (right == FLOAT) return FLOAT;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return FLOAT;
					else if (right == LONG) return FLOAT;
					else if (right == CHAR) throw Error::CANNOT_SUB_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_SUB_FROM_ARRAY;
					else {
						throw Error::INVALID_SUB_EXPR;
					}
					break;
				}
				else if (op == DIV){
					if (right == INT) return FLOAT;
					else if (right == DOUBLE) return FLOAT;
					else if (right == FLOAT) return FLOAT;
					else if (right == LONG) return FLOAT;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return FLOAT;
					else if (right == CHAR) throw Error::CANNOT_DIV_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_DIV_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_DIV_FROM_ARRAY;
					else {
						throw Error::INVALID_DIV_EXPR;
					}
					break;
				}
				else if (op == MUL){
					if (right == INT) return FLOAT;
					else if (right == DOUBLE) return FLOAT;
					else if (right == FLOAT) return FLOAT;
					else if (right == LONG) return FLOAT;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return FLOAT;
					else if (right == CHAR) throw Error::CANNOT_MULT_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_MULT_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_MULT_FROM_ARRAY;
					else {
						throw Error::INVALID_MULT_EXPR;
					}
					break;
				}
				else if (op == MOD){
					if (right == INT) return FLOAT;
					else if (right == DOUBLE) return FLOAT;
					else if (right == LONG) return FLOAT;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return FLOAT;
					else if (right == CHAR) throw Error::CANNOT_MOD_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_MOD_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_MOD_FROM_ARRAY;
					else {
						throw Error::INVALID_MOD_EXPR;
					}
					break;
				}
			}
			case CHAR: {
				if (op == ADD){
					if (right == INT) return INT;
					else if (right == DOUBLE) return DOUBLE;
					else if (right == FLOAT) return FLOAT;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return INT;
					else if (right == LONG) return LONG;
					else if (right == CHAR) return STRING;
					else if (right == STRING) return STRING;
					else if (right == ARRAY) throw Error::CANNOT_ADD_TO_ARRAY;
					else if (right == UNDEFINED || right == kNULL) return STRING;
					else {
						throw Error::INVALID_ADD_EXPR;
					}
					break;
				}
				else if (op == SUB){
					if (right == INT) return INT;
					else if (right == DOUBLE) return DOUBLE;
					else if (right == FLOAT) return FLOAT;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return INT;
					else if (right == LONG) return LONG;
					else if (right == CHAR) throw Error::CANNOT_SUB_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_SUB_FROM_ARRAY;
					else {
						throw Error::INVALID_SUB_EXPR;
					}
					break;
				}
				else if (op == DIV){
					if (right == INT) return INT;
					else if (right == DOUBLE) return DOUBLE;
					else if (right == FLOAT) return FLOAT;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return INT;
					else if (right == LONG) return LONG;
					else if (right == CHAR) throw Error::CANNOT_DIV_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_DIV_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_DIV_FROM_ARRAY;
					else {
						throw Error::INVALID_DIV_EXPR;
					}
					break;
				}
				else if (op == MUL){
					if (right == INT) return INT;
					else if (right == DOUBLE) return DOUBLE;
					else if (right == FLOAT) return FLOAT;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return INT;
					else if (right == LONG) return LONG;
					else if (right == CHAR) throw Error::CANNOT_MULT_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_MULT_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_MULT_FROM_ARRAY;
					else {
						throw Error::INVALID_MULT_EXPR;
					}
					break;
				}
				else if (op == MOD){
					if (right == INT) return INT;
					else if (right == DOUBLE) return DOUBLE;
					else if (right == FLOAT) return FLOAT;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return INT;
					else if (right == LONG) return LONG;
					else if (right == CHAR) throw Error::CANNOT_MOD_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_MOD_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_MOD_FROM_ARRAY;
					else {
						throw Error::INVALID_MOD_EXPR;
					}
					break;
				}
			}
			case STRING: {
				if (op == ADD){
					if (right == INT) return STRING;
					else if (right == DOUBLE) return STRING;
					else if (right == FLOAT) return STRING;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) return STRING;
					else if (right == CHAR) return STRING;
					else if (right == LONG) return STRING;
					else if (right == STRING) return STRING;
					else if (right == ARRAY) throw Error::CANNOT_ADD_TO_ARRAY;
					else if (right == UNDEFINED || right == kNULL) return STRING;
					else {
						throw Error::INVALID_ADD_EXPR;
					}
					break;
				}
				else if (op == SUB){
					if (right == INT) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
					else if (right == DOUBLE) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
					else if (right == FLOAT) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
					else if (right == LONG) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) throw Error::CANNOT_SUB_STRING_FROM_TYPE;
					else if (right == CHAR) return STRING;
					else if (right == STRING) return STRING;
					else if (right == ARRAY) throw Error::CANNOT_SUB_FROM_ARRAY;
					else {
						throw Error::INVALID_SUB_EXPR;
					}
					break;
				}
				else if (op == DIV){
					if (right == INT) throw Error::CANNOT_DIV_INT_FROM_TYPE;
					else if (right == DOUBLE) throw Error::CANNOT_DIV_DOUBLE_FROM_TYPE;
					else if (right == FLOAT) throw Error::CANNOT_DIV_FLOAT_FROM_TYPE;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) throw Error::CANNOT_DIV_INT_FROM_TYPE;
					else if (right == LONG) throw Error::CANNOT_DIV_INT_FROM_TYPE;
					else if (right == CHAR) throw Error::CANNOT_DIV_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_DIV_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_DIV_FROM_ARRAY;
					else {
						throw Error::INVALID_DIV_EXPR;
					}
					break;
				}
				else if (op == MUL){
					if (right == INT) throw Error::CANNOT_MULT_INT_FROM_TYPE;
					else if (right == DOUBLE) throw Error::CANNOT_MULT_DOUBLE_FROM_TYPE;
					else if (right == FLOAT) throw Error::CANNOT_MULT_FLOAT_FROM_TYPE;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) throw Error::CANNOT_MULT_INT_FROM_TYPE;
					else if (right == LONG) throw Error::CANNOT_MULT_INT_FROM_TYPE;
					else if (right == CHAR) throw Error::CANNOT_MULT_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_MULT_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_MULT_FROM_ARRAY;
					else {
						throw Error::INVALID_MULT_EXPR;
					}
					break;
				}
				else if (op == MOD){
					if (right == INT) throw Error::CANNOT_MOD_INT_FROM_TYPE;
					else if (right == DOUBLE) throw Error::CANNOT_MOD_DOUBLE_FROM_TYPE;
					else if (right == FLOAT) throw Error::CANNOT_MOD_FLOAT_FROM_TYPE;
					else if (right == BOOLEAN || right == TRUE_LITERAL || right == FALSE_LITERAL) throw Error::CANNOT_MOD_INT_FROM_TYPE;
					else if (right == LONG) throw Error::CANNOT_MOD_INT_FROM_TYPE;
					else if (right == CHAR) throw Error::CANNOT_MOD_CHAR_FROM_TYPE;
					else if (right == STRING) throw Error::CANNOT_MOD_STRING_FROM_TYPE;
					else if (right == ARRAY) throw Error::CANNOT_MOD_FROM_ARRAY;
					else {
						throw Error::INVALID_MOD_EXPR;
					}
					break;
				}
			}
			default: {
				if (op > BOOLEAN_COMPARISON_START && op < BOOLEAN_COMPARISON_END) return BOOLEAN;
				else if (op > BEGIN_ASSIGNMENT && op < END_ASSIGNMENT) return right;
			}
		}
		throw Error::INVALID_ASSIGNMENT_TO_TYPE;
	}

} // namespace internal
}
