#ifndef COBRA_H_
#define COBRA_H_

#include <string>

namespace Cobra {

	class Isolate
	{
	public:
		// The isolate must be entered into before usage
		void Enter();
		void Exit();
		void Dispose();
		Isolate* GetCurrent();

		void* operator new(size_t);

		static Isolate* New();
	};

	class Handle
	{
	public:
		bool IsString();
		const char* ToString();
	};

	class String
	{
	public:
		static Handle* New(Isolate* isolate);
		static Handle* New(Isolate* isolate, const char* string);
	};

}

void Compile(std::string* source);

#endif // COBRA_H_