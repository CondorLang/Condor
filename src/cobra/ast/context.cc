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
		String* string = script->GetSource()->ToString();
		std::string sourceCode = string->GetValue();

		std::hash<std::string> hash_fn;
    std::size_t str_hash = hash_fn(sourceCode);
    scripts[script->GetIsolate()][str_hash] = script;
	}

	Script* Context::GetScriptByString(Isolate* iso, std::string str){
		std::hash<std::string> hash_fn;
    std::size_t str_hash = hash_fn(str);
    return scripts[iso][str_hash];
	}

} // namespace internal
}