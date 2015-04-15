#include "context.h"

namespace Cobra{
	Context::Context(){

	}

	void Context::addImport(const char* import){
		if (!hasImport(import)){
			imports.push_back(import);
			printf("%s%s\n", import, " imported");
		}
		else{
			printf("%s%s\n", "unable to import ", import);
		}
	}

	bool Context::hasImport(const char* import){
		return !imports.empty() && std::find(imports.begin(), imports.end(), import) == imports.end();
	}
}