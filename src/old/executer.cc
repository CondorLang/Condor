// #include "executer.h"

// using namespace Cobra;

// Executer::Executer(Parser* mParser){
// 	parser = mParser;
// }

// Executer::~Executer(){
// 	delete parser;
// }

// void Executer::Run(){
// 	if (parser->callStack.empty()){
// 		CallFunction("main");
// 	}
// 	else {
		
// 	}
// }

// void Executer::CallFunction(const char* name){
// 	ASTFunction* func = parser->GetFunctionByName(name);
	
// 	std::map<std::string, ASTNode*> scope;

// 	if (func == NULL) throw Errors::FUNCTION_NOT_FOUND;
// 	else {
// 		for (int i = 0; i < func->executeOrder.size(); i++){
// 			ASTNode* node = func->executeOrder[i];
// 			if (node->type == "var"){
// 				ASTVar* newVar = new ASTVar;
// 				ASTVar* oldVar = (ASTVar*) node;
// 				newVar->name = oldVar->name;
// 				newVar->value = setVarValue(oldVar->value, &scope);
// 				if (newVar->name == "return"){
// 					//printf("%s\n", newVar->value->type.c_str());
// 					if (newVar->value->type == "string"){
// 						ASTString* str = (ASTString*) newVar->value;
// 						printf("%s\n", str->value.c_str());
// 					}
// 				}
// 				else{
// 					scope[newVar->name] = newVar;
// 				}
// 			}
// 		}
// 	}
// }

// ASTNode* Executer::setVarValue(ASTNode* tree, std::map<std::string, ASTNode*> *scope){

// 	if (tree->type == "operator"){

// 	}
// 	else if (tree->type == "var"){
// 		ASTVar* var = (ASTVar*) tree;
// 		std::string name = var->name;
// 		auto pos = scope->find(name);
// 		if (pos != scope->end()){
// 			return pos->second;
// 		}
// 		else {
// 			throw Errors::UNDEFINED;
// 		}
// 	}
// 	else if (tree->type == "string"){
// 		if (tree->Right == NULL && tree->Left == NULL){
// 			return tree;
// 		}
// 		else{
// 			// what to do??
// 		}
// 	}

// 	return NULL;
// }