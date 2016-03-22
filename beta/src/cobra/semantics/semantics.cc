#include "semantics.h"

namespace Cobra {
namespace internal{

	Semantics::Semantics(Isolate* iso){
		isolate = iso;
		row = -1;
		col = -1;
		currentScope = NULL;
		baseScope = NULL;
		trace = TRACE_SEMANTIC;
		indent = 0;
	}

	Semantics::~Semantics(){}

	Semantics* Semantics::New(Isolate* iso, Parser* parser){
		void* p = (Semantics*) iso->GetMemory(sizeof(Semantics));
		Semantics* n = new(p) Semantics(iso);
		n->parser = parser;
		return n;
	}

	void Semantics::SetRowCol(ASTNode* node){
		row = node->row;
		col = node->col;
	}

	void Semantics::Trace(const char* first, const char* second){
		if (trace) {
			std::string tabs = "";
			for (int i = 0; i < indent; i++) tabs += "  ";
			printf("%s%s - %s\n", tabs.c_str(), first, second);
		}
	}

	void Semantics::Trace(const char* first, TOKEN second){
		if (trace) {
			Token* t = Token::New(isolate, second);
			std::string tabs = "";
			for (int i = 0; i < indent; i++) tabs += "  ";
			printf("%s%s - %s\n", tabs.c_str(), first, t->String().c_str());
		}
	}
	
	void Semantics::Evaluate(Scope* scope){
		baseScope = scope;
		currentScope = scope;
		ScanScope(currentScope);
	}

	void Semantics::ScanScope(Scope* scope){
		Trace("Scanning Scope", (std::to_string(scope->Size()) + " items").c_str());
		for (int i = 0; i < scope->Size(); i++){
			ASTNode* node = scope->Get(i);
			SetRowCol(node);
			int type = (node->type);
			switch (type){
				case VAR: ValidateVar((ASTVar*) node); break;
				case FOR: ValidateFor((ASTForExpr*) node); break;
				default: {
					printf("d: %s\n", Token::ToString(node->type));
					throw Error::NOT_IMPLEMENTED;
				}
			}
		}
	}

	void Semantics::ValidateVar(ASTVar* var){
		Trace("Validating Var", var->name.c_str());
		indent++;
		var->assignmentType = ValidateExpr((ASTExpr*) var->value);
		Trace("Type", var->assignmentType);
		indent--;
	}

	TOKEN Semantics::ValidateExpr(ASTExpr* expr){
		if (expr == NULL) throw Error::MISSING_EXPR;
		int type = (int) expr->type;
		switch (type){
			case UNDEFINED: return UNDEFINED;
			case LITERAL: return ValidateLiteral((ASTLiteral*) expr);
		}
		return UNDEFINED;
	}

	TOKEN Semantics::ValidateLiteral(ASTLiteral* lit){
		Trace("Literal Value", lit->value.c_str());
		return lit->litType;
	}

	void Semantics::ValidateFor(ASTForExpr* expr){
		Trace("Validating", "For");
		indent++;
		ValidateVar((ASTVar*) expr->var);
		Trace("Var", expr->var->name.c_str());
		ValidateBoolean((ASTBinaryExpr*) expr->condition);
		expr->scope = Parser::Parse(isolate, expr->scope);
		expr->scope->InsertBefore(expr->var);
		ScanScope(expr->scope);
		indent--;
	}

	void Semantics::ValidateBoolean(ASTBinaryExpr* expr){
		if (!expr->isBoolean) throw Error::INVALID_BOOLEAN_COMPARISON;
	}

} // namespace internal
}