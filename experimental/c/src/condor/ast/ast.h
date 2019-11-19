// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

/**
 * The end user will not interact with this library.
 * This file is the basic structures for AST.
 *
 * User:
 * 	
 * 
 * Usage:	
 */

#ifndef AST_H_
#define AST_H_

#include <stdio.h>

#include "condor/token/token.h"
#include "condor/mem/allocate.h"
#include "condor/ast/scope.h"
#include "condor/ast/astlist.h"
#include "utils/assert.h"
#include "utils/string/string.h"

extern int ASTNODE_ID_SPOT;

typedef struct Scope Scope; // forward declare
typedef struct ASTNode ASTNode; // forward declare
typedef struct ASTList ASTList; // forward declare

// Getters
#define GET_VAR(node) node->meta.varExpr
#define GET_VAR_VALUE(node) node->meta.varExpr.value
#define GET_VAR_TYPE(node) node->meta.varExpr.dataType
#define GET_VAR_NAME(node) node->meta.varExpr.name
#define GET_BINARY(node) node->meta.binaryExpr;
#define GET_BIN_LEFT(node) node->meta.binaryExpr.left
#define GET_BIN_RIGHT(node) node->meta.binaryExpr.right
#define GET_BIN_OP(node) node->meta.binaryExpr.op
#define GET_FOR_BODY(node) node->meta.forExpr.body
#define GET_FOR_VAR(node) node->meta.forExpr.var
#define GET_FOR_CONDITION(node) node->meta.forExpr.condition
#define GET_WHILE_CONDITION(node) node->meta.whileExpr.condition
#define GET_WHILE_BODY(node) node->meta.whileExpr.body
#define GET_IF_CONDITION(node) node->meta.ifExpr.condition
#define GET_IF_BODY(node) node->meta.ifExpr.body
#define GET_SWITCH_CONDITION(node) node->meta.switchExpr.condition
#define GET_SWITCH_BODY(node) node->meta.switchExpr.body
#define GET_CASE_BODY(node) node->meta.caseStmt.body
#define GET_RETURN_VALUE(node) node->meta.returnStmt.value
#define GET_RETURN_TYPE(node) node->meta.returnStmt.type
#define GET_FUNC_NAME(node) node->meta.funcExpr.name
#define GET_FUNC_PARAMS(node) node->meta.funcExpr.params
#define GET_FUNC_BODY(node) node->meta.funcExpr.body

// Setters
#define SET_RETURN_TYPE(node, t) node->meta.returnStmt.type = t

struct ASTNode {
	Token type;

	int id; // Node ID assigned by ASTNODE_ID_SPOT
	int scopeId;
	int parentScopeId;

	/**
	 * When looping through all the nodes, this flag 
	 * helps identify if this node is a base node to be
	 * executed. For example:
	 *
	 * var a = 10;
	 *
	 * You don't need to execute on the 10, but rather
	 * on the var, then traverse through the tree.
	 */
	bool isStmt;

	/**
	 * A sneaky and memory efficent way to handle
	 * inheritance. This way ASTNode* could be passed
	 * by reference to functions.
	 */
	union {

		struct {
			bool value;
		} booleanExpr;

		struct {
			signed char value;
		} byteExpr;

		struct {
			short value;
		} shortExpr;

		struct {
			int value;
		} intExpr;

		struct {
			float value;
		} floatExpr;

		struct {
			double value;
		} doubleExpr;

		struct {
			long value;
		} longExpr;

		struct {
			char value;
		} charExpr;

		struct {
			char* value;
		} stringExpr;

		struct {
			ASTNode* left;
			ASTNode* right;
			Token op;
		} binaryExpr;

		struct {
			char* name;
			ASTNode* value;
			Token dataType;
			/**
			 * This can have one of three values.
			 * INC or ++
			 * DEC or --
			 * UNDEFINED or unset
			 */
			Token inc;
		} varExpr;

		struct {
			ASTNode* var;
			ASTNode* condition;
			ASTNode* inc;
			int body;
		} forExpr;

		struct {
			ASTNode* condition;
			int body;
		} ifExpr;

		struct {
			ASTNode* condition;
			int body;
		} whileExpr;

		struct {
			ASTNode* condition;
			int body;
		} caseStmt;

		struct {
			ASTNode* condition;
			int body;
		} switchExpr;

		struct {
			ASTNode* value;
			Token type; // Return type
		} returnStmt;

		struct {
			int body;
			char* name;
			// This is the returning data type
			Token dataType;
			ASTList* params;
		} funcExpr;

		struct {
			ASTNode* func;
			ASTList* params;
		} funcCallExpr;

	} meta;
};

void InitNodes(ASTNode nodes[], int len);
void DestroyNodes(ASTNode nodes[], int len);
char* ExpandASTNode(Scope* scope, ASTNode* node, int tab);
ASTNode* FindSymbol(Scope* scope, char* name);

#endif // AST_H_
