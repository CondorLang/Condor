#include "context.h"

namespace Cobra {
namespace internal{
	
	Context::Context(){

	}

	Context::~Context(){

	}

	void Context::SetIsolate(Isolate* isolate){
		scripts[isolate];
		isolate->SetContext(this);
		inProgress.SetIsolate(isolate);
		imported.SetIsolate(isolate);
	}

	void Context::AddToInProgress(std::string str){
		inProgress.push_back(str);
	}

	void Context::AddScript(Script* script){
		std::hash<std::string> hash_fn;
    std::size_t str_hash = hash_fn(script->GetSourceCode());
    scripts[script->GetIsolate()][str_hash] = script;
	}

	Script* Context::GetScriptByString(Isolate* iso, std::string str){
		std::hash<std::string> hash_fn;
    std::size_t str_hash = hash_fn(str);
    return scripts[iso][str_hash];
	}

	bool Context::IsIncluded(Isolate* iso, const char* path){
		std::string pth = path;
		for (int i = 0; i < inProgress.size(); i++){
			if (pth == inProgress[i]) return true;
		}
		std::hash<std::string> hash_fn;
    std::size_t str_hash = hash_fn(path);
    return scripts[iso].find(str_hash) != scripts[iso].end();
	}

	bool Context::IsImported(Isolate* iso, std::string name){
		return imported.find(name) != -1;
	}

	void Context::SetImport(std::string name){
		imported.push_back(name);
	}

	void Context::RemoveFromInProgress(std::string str){
		std::string pth = str;
		int idx = inProgress.find(pth);
		inProgress.erase(idx);
	}

	ASTNode* Context::GetExportedNode(Isolate* iso, std::string name){
		std::map<size_t, Script*> scrs = scripts[iso];
		for (std::map<size_t, Script*>::const_iterator it = scrs.begin(); it != scrs.end(); it++){
			Script* s = it->second;
			CHECK(s != NULL);
			ASTNode* node = s->GetExportedObject(name);
			if (node != NULL) return node;
		}
		return NULL;
	}

	void Context::PrintExported(Isolate* iso){
		std::map<size_t, Script*> scrs = scripts[iso];
		for (std::map<size_t, Script*>::const_iterator it = scrs.begin(); it != scrs.end(); it++){
			Script* s = it->second;
			CHECK(s != NULL);
			if (s != NULL) s->PrintExported();
		}
	}

} // namespace internal
}