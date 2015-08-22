#ifndef COBRA_H_
#define COBRA_H_

#include <string>

namespace Cobra {

	double Version();

	void SetCommandLineFlags(int argc, const char* argv[]);
	
	/**
	 * @brief Creates an isolated instance of Cobra
	 * @details All scripts and objects need to be created through an isolate.
	 */
	class Isolate
	{
	public:
		void Enter();
		void Exit();
		void Dispose(); 
		Isolate* GetCurrent();

		void* operator new(size_t);

		static Isolate* New();
	};

	/**
	 * @brief Creates the context for the program. 
	 * @details Global variables, functions, objects, etc... can't extend past the context.
	 */
	class Context
	{
	public:
		static Context* New();
		void* operator new(size_t);
		void SetIsolate(Isolate* isolate);
	};

	/**
	 * @brief All Cobra scripts and objects will be wrapped in a handler.
	 * @details The handler is used to hold the memory location of
	 * the object. This allows for the garbage collector to track each object.
	 */
	class Handle
	{
	public:
		bool IsString();
		bool IsScript();
		const char* ToString();
		const char* GetType();
		void Run();
	};

	/**
	 * @brief String class
	 * @details All Cobra strings are stored in a String.
	 */
	class String
	{
	private:
		const char* fileName;
	public:
		static Handle* New(Isolate* isolate);
		static Handle* New(Isolate* isolate, const char* string);
		static Handle* New(Isolate* isolate, const char* string, const char* path);
		static Handle* NewFromFile(Isolate* isolate, const char* path);
		bool IsEmpty();
	};

	/**
	 * @brief A compiled script
	 * @details A script that has been compiled will be wrapped
	 * is this Script class.
	 */
	class Script
	{
	public:
		static Handle* Compile(Isolate* isolate, Handle* handle);
	};

}

void Compile(std::string* source);

#endif // COBRA_H_