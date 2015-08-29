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
	}

	void Context::AddScript(Script* script){
		std::hash<std::string> hash_fn;
    std::size_t str_hash = hash_fn(script->GetAbsolutePath());
    scripts[script->GetIsolate()][str_hash] = script;
	}

	Script* Context::GetScriptByString(Isolate* iso, std::string str){
		std::hash<std::string> hash_fn;
    std::size_t str_hash = hash_fn(str);
    return scripts[iso][str_hash];
	}

	bool Context::IsIncluded(Isolate* iso, const char* path){
		std::string pth = path;
		if (std::find(inProgress.begin(), inProgress.end(), pth) != inProgress.end()) return true;
		std::hash<std::string> hash_fn;
    std::size_t str_hash = hash_fn(path);
    return scripts[iso].find(str_hash) != scripts[iso].end();
	}

	void Context::RemoveFromInProgress(std::string str){
		std::string pth = str;
		inProgress.erase(std::find(inProgress.begin(), inProgress.end(), pth));
	}

} // namespace internal
}