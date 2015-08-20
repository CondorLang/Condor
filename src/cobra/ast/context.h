#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <map>
#include "cobra/mem/isolate.h"
#include "cobra/types/script/script.h"

namespace Cobra {
namespace internal{

	class Isolate;
	class Script;
	
	class Context
	{
	private:
		std::map<Isolate*, std::map<size_t, Script*> > scripts;
	public:
		Context();
		~Context();
		void SetIsolate(Isolate* isolate);
		void AddScript(Script* script);
		Script* GetScriptByString(Isolate* iso, std::string code);
	};

} // namespace internal
}

#endif // CONTEXT_H_
