#include "api.h"

#include <fstream>
#include <streambuf>
#include <stdlib.h>

#include "cobra/mem/isolate.h"
#include "cobra/types/strings/string.h"
#include "cobra/mem/handle.h"
#include "cobra/token/token.h"
#include "cobra/globals.h"
#include "cobra/types/script/script.h"
#include "cobra/ast/context.h"
#include "cobra/mem/factory.h"
#include "cobra/flags.h"

namespace Cobra{

	/**
	 * @brief Get the version of Cobra
	 * @return double version number
	 */
	double Version(){
		return VERSION;
	}

	/**
	 * @brief Sets the command line flags
	 * @details [long description]
	 * 
	 * @param argc int of arg count
	 * @param argv const char* argv[]
	 */
	void SetCommandLineFlags(int argc, const char* argv[]){
		i::Flags::SetCommandLineFlags(argc, argv);
	}

	void Context::SetIsolate(Isolate* isolate){
		i::Context* context = CAST(i::Context*, this);
		i::Isolate* cobra_isolate = CAST(i::Isolate*, isolate);
		context->SetIsolate(cobra_isolate);
	}

	/**
	 * @brief Create a new Cobra::Context
	 * @return Cobra::Context
	 */
	Context* Context::New(){
		i::Context* context = new i::Context();
		return CAST(Context*, context);
	}

	/**
	 * @brief Create a new context
	 * @details the "new" keyword calls Context::New();
	 * 
	 * @param size Unused but required
	 * @return Cobra::Context
	 */
	void* Context::operator new(size_t size){
		return Context::New();
	}

	/**
	 * @brief Dispose and delete an isolate.
	 * @details In order to delete and dispose an isolate
	 * completely, this method must be called. Once disposed
	 * all memory will be released.
	 */
	void Isolate::Dispose(){
		i::Isolate* isolate = CAST(i::Isolate*, this);
		isolate->FlushAll();
		delete isolate;
	}

	/**
	 * @brief Create a new Isolate
	 * @details the "new" keyword calls Isolate::New();
	 * 
	 * @param size Unused but required
	 * @return Cobra::Isolate
	 */
	void* Isolate::operator new(size_t size){
		return Isolate::New();
	}

	/**
	 * @brief Enter into an isolated instance
	 * @details Entering into an isolate is required
	 * before using any code inside of it.
	 */
	void Isolate::Enter(){
		i::Isolate* isolate = CAST(i::Isolate*, this);
		isolate->Enter();
	}

	/**
	 * @brief Exit an isolate
	 * @details Exits an isolate.
	 */
	void Isolate::Exit(){
		i::Isolate* isolate = CAST(i::Isolate*, this);
		isolate->Exit();
	}
	
	/**
	 * @brief Create a new Cobra::Isolate
	 * @details Creates a new Cobra::Isolate.
	 * @return Cobra::Isolate
	 */
	Isolate* Isolate::New(){
		i::Isolate* isolate = new i::Isolate();
		return CAST(Isolate*, isolate);
	}

	/**
	 * @brief Gets the current Isolate
	 * @details The current, entered into, Isolate is 
	 * stored into a global variable only accessible
	 * by this function.
	 * @return Cobra::Isolate or NULL
	 */
	Isolate* Isolate::GetCurrent(){
		i::Isolate* isolate = i::_currentIsolate;
		if (isolate != NULL){
			return CAST(Isolate*, isolate);
		}
		return NULL;
	}

	/**
	 * @brief Create empty String
	 * @details Creates an empty Cobra::String. The
	 * default path will be "inline"
	 * 
	 * @param isolate The current Isolate
	 * @return Cobra::String
	 */
	Handle* String::New(Isolate* isolate){
		return String::New(isolate, "", "inline");
	}

	/**
	 * @brief Create String
	 * @details Creates a String based on a string value.
	 * The default path will be "inline"
	 * 
	 * @param isolate The current Isolate
	 * @param string const char* string
	 * 
	 * @return Cobra::String
	 */
	Handle* String::New(Isolate* isolate, const char* string){
		return String::New(isolate, string, "inline");
	}

	/**
	 * @brief Create String
	 * @details Creates a String based on a string value.
	 * 
	 * @param isolate The current Isolate
	 * @param string The const char* to store
	 * @param path The location of the string
	 * @return Cobra::String
	 */
	Handle* String::New(Isolate* isolate, const char* string, const char* path){
		i::Isolate* cobra_isolate = CAST(i::Isolate*, isolate);
		i::String* str = cobra_isolate->factory->NewString(string);
		str->SetPath(path);
		i::HeapObject obj;
		obj.address = CAST(i::Address, str);
		obj.type = i::API_STRING;
		i::HeapObject* newStr = cobra_isolate->Insert(obj);
		i::Handle* handle = cobra_isolate->InsertToHeap(new i::Handle(newStr, cobra_isolate), i::HANDLE);
		return CAST(Handle*, handle);
	}

	/**
	 * @brief Create String from file
	 * @details Creates a String from the contents
	 * of a file. If the file or directory doesn't exist,
	 * the contents of the String will be empty
	 * TODO:
	 * 		realpath support for windows: GetFullPathName
	 * 
	 * @param isolate The current Isolate
	 * @param path const char* path to the file
	 * 
	 * @return Cobra::String
	 */
	Handle* String::NewFromFile(Isolate* isolate, const char* path){
		char* absolutePath = realpath(path, NULL);
		std::ifstream in(absolutePath);
		if (!in){
			printf("File, %s, was empty\n", absolutePath);
			return String::New(isolate);
		}
		std::string fileStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		return String::New(isolate, fileStr.c_str(), absolutePath);
	}

	/**
	 * @brief Handle is String
	 * @details Checks if the contents of the Handle
	 * is a Cobra::String.
	 * @return bool
	 */
	bool Handle::IsString(){
		i::Handle* handle = CAST(i::Handle*, this);
		return handle->GetObject()->type == i::STRING;
	}

	/**
	 * @brief Handle is Script
	 * @details Checks if the contents of the Handle
	 * is a Cobra::Script
	 * @return bool
	 */
	bool Handle::IsScript(){
		i::Handle* handle = CAST(i::Handle*, this);
		return handle->GetObject()->type == i::SCRIPT;
	}

	/**
	 * @brief Stringify the Handle object
	 * @details If the contents of the Hanle is a
	 * Cobra::String, the const char* value will be 
	 * returned.
	 * @return const char*
	 */
	const char* Handle::ToString(){
		if (IsString()){
			i::Handle* handle = CAST(i::Handle*, this);
			i::HeapObject* obj = handle->GetObject();
			i::String* str = CAST(i::String*, obj->address);
			return str->GetValue();
		}
		return NULL;
	}

	/**
	 * @brief Get the type of the Handle
	 * @return const char*
	 */
	const char* Handle::GetType(){
		i::Handle* handle = CAST(i::Handle*, this);
		i::Token* tok = new i::Token(handle->GetObject()->type);
		const char* type = tok->String().c_str();
		delete tok;
		return type;
	}

	
	/**
	 * @brief Compile Script
	 * @details To compile a script, this function must be
	 * called.
	 * 
	 * @param isolate The current Isolate
	 * @param handle A Cobra::Handle type of String
	 * 
	 * @return Handle type of Script
	 */
	Handle* Script::Compile(Isolate* isolate, Handle* handle){
		i::Handle* cobra_handle = CAST(i::Handle*, handle);
		i::Isolate* cobra_isolate = CAST(i::Isolate*, isolate);
		i::Script* script = cobra_isolate->factory->NewScript(cobra_handle);
		script->Compile();

		i::HeapObject obj;
		obj.address = CAST(i::Address, script);
		obj.type = i::SCRIPT;
		i::HeapObject* newScript = cobra_isolate->Insert(obj);
		i::Handle* scriptHandle = cobra_isolate->InsertToHeap(new i::Handle(newScript, cobra_isolate), i::HANDLE);
		return CAST(Handle*, scriptHandle);
	}

	const char* Handle::GetErrorMessage(){
		i::Handle* handle = CAST(i::Handle*, this);
		i::Script* script = handle->ToScript();
		return script->GetErrorMsg();
	}

	bool Handle::IsCompiled(){
		i::Handle* handle = CAST(i::Handle*, this);
		i::Script* script = handle->ToScript();
		return script->HasError();
	}

	/**
	 * @brief Runs the script
	 * @details This executes the code, compiles and runs all the imports
	 */
	void Handle::Run(){
		if (IsScript()){
			i::Handle* cobra_handle = CAST(i::Handle*, this);
			i::Script* cobra_script = cobra_handle->ToScript();
			cobra_script->Run();
		}
	}

}