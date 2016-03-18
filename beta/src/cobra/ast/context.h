#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <vector>
#include <string>

#include "cobra/mem/isolate.h"

namespace Cobra {
namespace internal{

	class Isolate;
	
	class Context
	{
	private:
		Isolate* isolate;
		std::vector<std::string> inProgress;
		int nextAstId;

	public:
		Context();
		~Context();
		void SetIsolate(Isolate* isolate){this->isolate = isolate;}
		Isolate* GetIsolate(){return this->isolate;}
		void AddToInProgress(std::string str);
		int GetNextAstId();
	};

} // namespace internal
}

#endif // CONTEXT_H_
