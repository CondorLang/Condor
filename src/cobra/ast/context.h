#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <vector>
#include <map>
#include <string>

#include "cobra/mem/isolate.h"
#include "cobra/ast/node.h"

namespace Cobra {
namespace internal{

	class Isolate;
	class Scope;
	class ASTNode;
	
	class Context
	{
	private:
		Isolate* isolate;
		std::vector<std::string> inProgress;
		std::vector<Scope*> root;
		std::map<std::string, bool> included;
		int nextAstId;

	public:
		Context();
		~Context();
		inline void Enter(){}
		inline void Exit(){}
		void SetIsolate(Isolate* isolate);
		Isolate* GetIsolate(){return this->isolate;}
		void AddToInProgress(std::string str);
		int GetNextAstId();
		void AddScope(Scope* scope);
		bool IsIncluded(std::string name);
		std::vector<ASTNode*> Lookup(Scope* scope, std::string name);
	};

} // namespace internal
}

#endif // CONTEXT_H_
