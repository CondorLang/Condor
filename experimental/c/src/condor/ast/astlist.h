// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef AST_LIST_H_
#define AST_LIST_H_

#include "ast.h"

typedef struct ASTNode ASTNode; // forward declare

/**
 * This is a lightweight implementation of a linked list. 
 * The purpose of this implementation is to optimize the
 * use of the stack to the fullest amount possible.
 */
typedef struct ASTList ASTList;
typedef struct ASTListItem ASTListItem; 

struct ASTList{
	ASTListItem* first;
	ASTListItem* last;
	ASTListItem* current;
	int size;
	int nodeId; // Linked back to the node
};

struct ASTListItem{
	ASTNode* node;
	ASTListItem* prev;
	ASTListItem* next;
};

#define FOREACH_AST(list) for (ASTListItem* item = list->first; item != NULL; item = item->next)

void InitParams(ASTList* params, int totalFuncs);
void InitParamItems(ASTListItem* paramItems, int totalParamItems);

#endif // AST_LIST_H_
