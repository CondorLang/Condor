#include "scope.h"

namespace Cobra{
	Scope::Scope(){
		count = 1;
	}

	Scope::~Scope(){
		delete outer;
		for (int i = 0; i < ordered.size(); i++){
			delete ordered[0];
		}
	}

	Scope* Scope::NewScope(){
		Scope* scope = new Scope;
		scope->outer = this;
		return scope;
	}

	ASTNode* Scope::Lookup(std::string name){
		auto obj = objects.find(name);
		if (obj == objects.end()){
			return NULL;
		}
		else{
			return obj->second;
		}
	}

	void Scope::Insert(ASTNode* node){
		if (node != NULL){
			if (node->name[0] != '%'){
				objects[node->name] = node;
			}
			ordered.push_back(node);
			count++;
		}
	}

	ASTNode* Scope::NewObject(std::string name){
		ASTNode* obj = new ASTNode;
		obj->name = name;
		Insert(obj);
		return obj;
	}

	ASTNode* Scope::Get(int index){
		return ordered[index];
	}

	int Scope::Size(){
		return ordered.size();
	}

	void Scope::String(){
		for (std::map<std::string, ASTNode*>::iterator it = objects.begin(); it != objects.end(); ++it){
			printf("Found: %s\n", it->first.c_str());
		}
	}
} // namespace Cobra	