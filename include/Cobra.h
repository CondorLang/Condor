#ifndef COBRA_H_
#define COBRA_H_

#include <string>

namespace Cobra {

	/**
	 * This creates an isolated instance of Cobra
	 */
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

	/**
	 * All objects are to be sent to the handler. Handles contain:
	 * 	String
	 * 	Script
	 */
	class Handle
	{
	public:
		bool IsString();
		bool IsScript();
		const char* ToString();
	};

	class String
	{
	private:
		const char* fileName;
	public:
		static Handle* New(Isolate* isolate);
		static Handle* New(Isolate* isolate, const char* string);
		static Handle* New(Isolate* isolate, const char* string, const char* path);
		static Handle* NewFromFile(Isolate* isolate, const char* path);
	};

	class Script
	{
	public:
		static Handle* Compile(Isolate* isolate, Handle* handle);
	};

}

void Compile(std::string* source);

#endif // COBRA_H_