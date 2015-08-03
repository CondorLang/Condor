#include "walk.h"

namespace Cobra {

	ASTLiterary* Walk::AddMinus(ASTExpr* left, ASTExpr* right, TOKEN op){
		int lType = (int) left->type;
		if (lType == LITERARY || lType == IDENT){
			ASTLiterary* leftLit = NULL;
			if (lType == IDENT){
				leftLit = WalkIdent((ASTIdent*) left);
			}
			else{
				leftLit = (ASTLiterary*) left;
			}
			int lKind = (int) leftLit->kind;

			ASTLiterary* rightLit = (ASTLiterary*) WalkStmt(right);
			int rKind = (int) rightLit->kind;

			if (lKind == rKind && lKind == INT){
				int lVal = std::stoi(leftLit->value);
				int rVal = std::stoi(rightLit->value);
				ASTLiterary* lit = new ASTLiterary;
				lit->kind = rightLit->kind;
				if (op == ADD){
					lit->value = std::to_string(lVal + rVal);
				}
				else if (op == SUB){
					lit->value = std::to_string(lVal - rVal);
				}
				return lit;
			}
			else if (lKind == rKind && lKind == DOUBLE){
				double lVal = std::stod(leftLit->value);
				double rVal = std::stod(rightLit->value);
				ASTLiterary* lit = new ASTLiterary;
				lit->kind = rightLit->kind;
				if (op == ADD){
					lit->value = std::to_string(lVal + rVal);
				}
				else if (op == SUB){
					lit->value = std::to_string(lVal - rVal);
				}
				return lit;
			}
			else{
				if (op == ADD){
					throw Error::CANNOT_ADD_UNLIKE_VALUES;
				}
				else{
					throw Error::CANNOT_MINUS_UNLIKE_VALUES;
				}
			}
		}
		else{
			if (op == ADD){
				throw Error::CANNOT_CONCAT_TYPE;
			}
			else if (op == SUB){
				throw Error::CANNOT_SUBTRACT;
			}
		}
		return NULL;
	}

	ASTLiterary* Walk::WalkFuncCall(ASTFuncCallExpr* call){
		Func* func = new Func;

		std::vector<ASTExpr*> args = call->params;
		std::vector<Type*> callArgs;
		for (int i = 0; i < args.size(); i++){
			callArgs.push_back(ParseType((ASTExpr*) args[i]));
		}
		func->SetAST(call->func);
		FuncReturnValue* returnValue = new FuncReturnValue;
		func->Call(&callArgs, returnValue);
		//delete func;
		return returnValue->value;
	}

	ASTLiterary* Walk::WalkIdent(ASTIdent* ident){
		std::map<std::string, ASTLiterary*>::const_iterator it = stack.find(ident->name);
		if (it == stack.end()){
			return NULL;
		}
		else{
			ASTLiterary* lit = it->second;
			if (lit == NULL){
				printf("Ident: %s - %s\n", ident->name.c_str(), "nope");
			}
			return it->second;
		}
	}

	/**
	 * TODO:
	 * 	Add other operators
	 */
	ASTLiterary* Walk::WalkUnary(ASTUnaryExpr* unary){
		int type = (int) unary->op->value;
		switch(type){
			case ASSIGN: {
				ASTLiterary* lit = WalkStmt(unary->value);
				lit->name = unary->name;
				return lit;
			}
		}
		return NULL;
	}

	/**
	 * TODO:
	 * 	Add other operators
	 */
	ASTLiterary* Walk::WalkBinary(ASTBinaryExpr* bin){
		int type = (int) bin->op->value;
		switch (type){
			case ADD: case SUB: return AddMinus(bin->Left, bin->Right, bin->op->value);
		}
		return NULL;
	}

	ASTLiterary* Walk::WalkStmt(ASTNode* stmt){
		if (stmt == NULL) return NULL;
		int type = (int) stmt->type;
		// Token* tok = new Token(stmt->type);
		// printf("D: %s\n", tok->String().c_str());
		switch (type){
			case LITERARY: return (ASTLiterary*) stmt;
			case BINARY: return WalkBinary((ASTBinaryExpr*) stmt);
			case IDENT: return WalkIdent((ASTIdent*) stmt);
			case UNARY: return WalkUnary((ASTUnaryExpr*) stmt);
			case FUNC_CALL: return WalkFuncCall((ASTFuncCallExpr*) stmt);
		}

		return NULL;
	}

	/**
	 * TODO:
	 * 	Walk array
	 * 	Walk object
	 *
	 * 	Not all objects are stmts
	 */
	WalkPair* Walk::WalkVar(ASTVar* var){
		WalkPair* pair = new WalkPair;
		pair->name = var->name;
		pair->value = WalkStmt(var->stmt);
		return pair;
	}

	WalkPair* Walk::WalkNode(ASTNode* node){
		if (node == NULL) return NULL;
		int type = (int) node->type;
		switch (type){
			case VAR: return WalkVar((ASTVar*) node);
			case LITERARY: {
				ASTLiterary* lit = (ASTLiterary*) node;
				WalkPair* pair = new WalkPair;
				pair->name = lit->name;
				pair->value = lit;
				return pair;
			}
		}
		// Token* tok = new Token(node->type);
		// printf("D: %s\n", tok->String().c_str());
		return NULL;
	}

	void Walk::WalkScope(){
		if (scope != NULL){
			for (int i = 0; i < scope->Size(); i++){
				WalkPair* pair = WalkNode(scope->Get(i));
				stack[pair->name] = pair->value;
			}
		}
	}

	void Walk::WalkFunc(ASTFunc* func){
		if (func != NULL){
			scope = func->body->scope;
			WalkScope();
		}
	}

	/**
	 * TODO:
	 * 	Allow for parsing of other types
	 */
	Type* Walk::ParseType(ASTExpr* node){
		if (node->type == IDENT){
			return ParseType(WalkIdent((ASTIdent*) node));
		}
		else if (node->type == LITERARY){
			ASTLiterary* lit = (ASTLiterary*) node;
			int type = (int) lit->kind;
			switch (type){
				case INT: {
					Int* in = new Int;
					in->SetAST(lit);
					return in;
				}
			}
		}
		return NULL;
	}

	ASTLiterary* Walk::GetValue(){
		std::map<std::string, ASTLiterary*>::const_iterator it = stack.find("return");
		if (it == stack.end()){
			return NULL;
		}
		else{
			return it->second;
		}
	}

	Walk::~Walk(){
		if (scope != NULL) delete scope;
		stack.erase(stack.begin(), stack.end());
	}

} // namespace Cobra
