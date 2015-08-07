#ifndef COBRA_H_
#define COBRA_H_

#include <string>

namespace Cobra {

	class Isolate
	{
	public:
		Isolate();
		~Isolate();
		static Isolate* New();
	};

}

void Compile(std::string* source);

#endif // COBRA_H_