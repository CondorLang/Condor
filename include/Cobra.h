#ifndef COBRA_H_
#define COBRA_H_

#include <string>

namespace Cobra {

	class Isolate
	{
	public:
		Isolate();
		~Isolate();
		// The isolate must be entered into before usage
		void Enter();
		void Exit();

		void* operator new(size_t);

		static Isolate* New();
	};

}

void Compile(std::string* source);

#endif // COBRA_H_