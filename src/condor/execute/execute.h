// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef EXECUTE_H_
#define EXECUTE_H_

#include <string>
#include <vector>
#include <map>

#include "stack.h"

#include "condor/assert.h"
#include "condor/ast/node.h"
#include "condor/ast/scope.h"
#include "condor/mem/isolate.h"
#include "condor/token/token.h"
#include "condor/error/error.h"
#include "condor/semantics/binary.h"
#include "condor/semantics/semantics.h"

namespace Condor {
namespace internal{

	class Semantics;

	class Execute
	{
	private:
		std::vector<Scope*> scopes;
		std::vector<RPNStack*> stack;
		std::vector<ASTObjectInstance*> objStack;
		Isolate* isolate;
		bool trace;
		bool steps;
		int numOfSteps;
		bool canBreak;
		bool postixPeriod;
		bool rpnStack;
		bool isReturning;
		ASTNode* returnValue;

		void Trace(std::string first, std::string msg2);
		void PrintStep(std::string msg);
		void OpenScope(Scope* sc);
		void CloseScope(){scopes.erase(scopes.begin());}
		Scope* GetCurrentScope(){return scopes[0];}
		void AddObject(ASTObjectInstance* obj){objStack.push_back(obj);}
		void RemoveObject(){objStack.erase(objStack.begin());}
		ASTObjectInstance* GetCurrentObject(){if (objStack.size() > 0) return objStack[0]; return NULL;}
		void SetRowCol(ASTNode* node);
		void NewStack(){stack.insert(stack.begin(), RPNStack::New(isolate));}
		void CloseStack(){isolate->FreeMemory(stack[0], sizeof(RPNStack)); stack.erase(stack.begin());}
		RPNStack* GetCurrentStack(){if (stack.size() == 0) return NULL; return stack[0];}

		ASTLiteral* EvaluateFuncCall(ASTFuncCall* call);
		ASTLiteral* EvaluateBinary(ASTBinaryExpr* binary); 
		ASTLiteral* EvaluateValue(ASTNode* node);
		ASTLiteral* EvaluateVar(ASTVar* var);
		void EvaluateFor(ASTForExpr* expr);
		void FillPostix(ASTBinaryExpr* binary);
		ASTLiteral* Calculate();
		ASTLiteral* Calc(ASTToken* tok);
		void FormatLit(ASTLiteral* lit, bool forceType = false);
		void TruncZeros(ASTLiteral* lit);
		void SetCast(ASTExpr* expr, ASTLiteral* value);
		void EvaluateWhile(ASTWhileExpr* expr);
		bool EvaluateIf(ASTIf* expr);
		void Assign(ASTBinaryExpr* binary);
		ASTVar* GetVar(ASTNode* node);
		void SetCalc(ASTLiteral* lit);
		void EvaluateSwitch(ASTSwitch* expr);
		ASTLiteral* StackCall(ASTFuncCall* call, ASTToken* tok, ASTLiteral* first);
		void SetLitType(ASTLiteral* lit);

	public:
		static Execute* New(Isolate* isolate, Scope* scope);
		Execute(Scope* scope);
		~Execute(){}
		void Evaluate();
		std::string GetSource(){return GetCurrentScope()->raw;}
		Semantics* semantic;

		int row;
		int col;
	};

} // namespace internal
} // namespace Condor

#endif // EXECUTE_H_