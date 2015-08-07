#include "scope.h"

namespace Cobra{
namespace internal{
	Scope::Scope(){
		count = 1;
		outer = NULL;
	}

	Scope::~Scope(){
		if (outer != NULL) delete outer;
		ordered.erase(ordered.begin(), ordered.end());
		//runtime.erase(runtime.begin(), runtime.end());
	}

	Scope* Scope::NewScope(){
		Scope* scope = new Scope;
		scope->outer = this;
		return scope;
	}

	ASTNode* Scope::Lookup(std::string name){
		if (name.empty()) return NULL;
		std::map<std::string, ASTNode*>::const_iterator obj = objects.find(name);
		if (obj == objects.end()){
			return NULL;
		}
		else{
			return obj->second;
		}
	}

	void Scope::Insert(ASTNode* node){
		if (node != NULL){
			if (node->name.length() > 0 && node->name[0] != '%'){
				objects[node->name] = node;
			}
			ordered.push_back(node);
			count++;
		}
	}

	void Scope::InsertType(Type* type){
		if (type != NULL){
			runtime[type->name] = type;
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

	Type* Scope::LookupType(std::string name){
		if (name.empty()) return NULL;
		std::map<std::string, Type*>::const_iterator obj = runtime.find(name);
		if (obj == runtime.end()){
			return NULL;
		}
		else{
			return obj->second;
		}
	}

	void Scope::InsertBefore(ASTNode* node){
		if (node != NULL){
			if (node->name.length() > 0 && node->name[0] != '%'){
				objects[node->name] = node;
			}
			ordered.insert(ordered.begin(), node);
			count++;
		}
	}
} // namespace internal
} // namespace Cobra	