#include "check.h"

namespace Cobra{

	Check::Check(){
		scope = NULL;
		trace = true; // debug
		col = 0;
		row = 0;
	}

	Check::~Check(){

	}

	void Check::CheckFile(ASTFile* f){
		file = f;
		OpenBaseScope();
		CountItemsInScope();
		if (HasMain()){
			ValidateFunc((ASTFunc*) scope->Lookup("main"));
		}
		//ScanScope();
	}

	void Check::Trace(std::string msg1, std::string msg2){
		printf("%s - %s\n", msg1.c_str(), msg2.c_str());
	}
	
	void Check::ValidateVar(ASTNode* node){
		if (node->type == VAR){
			if (trace) Trace("Validating var", node->name);
			ASTVar* var = (ASTVar*) node;
			if (trace) Trace(node->name, "is of type " + (new Token(var->varType))->String());
		}
	}

	void Check::ValidateFor(ASTNode* node){
		if (trace) Trace("Validating", "For loop");
		ASTFor* forStmt = (ASTFor*) node;
		ValidateVar(forStmt->var);
	}

	void Check::CheckScopeLevelNode(ASTNode* node){
		int type = (int) node->type;
		switch (type){
			case FUNC_CALL: {

			}
			case FOR: {
				ValidateFor(node);
			}
		}
	}

	void Check::ScanScope(){
		if (scope->Size() > 0){
			for (int i = 0; i < scope->Size(); i++){
				CheckScopeLevelNode(scope->Get(i));
			}
		}
	}

	void Check::ValidateFuncArgs(std::vector<ASTNode*> ordered){
		if (ordered.size() > 0){
			for (int i = 0; i < ordered.size(); i++){
				ASTNode* node = ordered[i];
				if (node->name.length() < 1) throw Error::EXPECTED_ARG_NAME;
				if (trace) Trace("arg[" + std::to_string(i) + "]", node->name);
			}
		}
	}

	void Check::ValidateFunc(ASTFunc* func){
		if (trace) Trace("\nValidating func", func->name);
		if (trace) Trace(func->name + "() total args: ", std::to_string(func->ordered.size()));
		ValidateFuncArgs(func->ordered);
		if (func->body == NULL) throw Error::MISSING_FUNC_BODY;
		OpenScope(func->body->scope);
		CountItemsInScope();
		ScanScope();
		CloseScope();
	}

	bool Check::HasMain(){
		bool hasMain = scope->Lookup("main") != NULL;
		if (trace && hasMain) Trace("Base Scope", "Has main");
		else Trace("Base Scope", "Does not have main");
		return hasMain;
	}

	void Check::CountItemsInScope(){
		if (trace) Trace("Total items in scope", std::to_string(scope->Size()));
	}

	void Check::CloseScope(){
		scope = scope->outer;
	}

	void Check::OpenScope(Scope* s){
		scope = s;
	}

	void Check::OpenBaseScope(){
		if (file->scope != NULL){
			if (trace) Trace("Base Scope", "Is Opened");
			scope = file->scope;
		}
		else{
			if (trace) Trace("Base Scope", "Is Null");
		}
	}

} // namespace Cobra