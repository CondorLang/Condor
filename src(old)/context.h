#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <vector>
#include <stdio.h>

namespace Cobra {
	class Context
	{
	private:
		std::vector<const char*> imports;
	public:
		Context();
		~Context(){}
		void addImport(const char* import);
		bool hasImport(const char* import);
	};
}

#endif // CONTEXT_H_