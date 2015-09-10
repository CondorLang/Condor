#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <map>
#include "cobra/mem/isolate.h"
#include "cobra/types/script/script.h"
#include "cobra/types/strings/string.h"

namespace Cobra {
namespace internal{

	class Isolate;
	class Script;
	
	class Context
	{
	private:
		std::map<Isolate*, std::map<size_t, Script*> > scripts;
		std::vector<std::string> inProgress;
	public:
		Context();
		~Context();
		void SetIsolate(Isolate* isolate);
		void AddScript(Script* script);
		Script* GetScriptByString(Isolate* iso, std::string code);
		bool IsIncluded(Isolate* iso, const char* path);
		void AddToInProgress(std::string str){inProgress.push_back(str);}
		void RemoveFromInProgress(std::string str);
		ASTNode* GetExportedNode(Isolate* iso, std::string name);
	};

} // namespace internal
}

#endif // CONTEXT_H_
