// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef COBRA_H_
#define COBRA_H_

#include <string>
#include <algorithm>

namespace Cobra {

	const char* Version();

	void SetCommandLineFlags(int argc, const char* argv[]);
		
	class Context
	{
	public:
		void Enter();
		void Exit();
		void Dispose();		
	};
		
	/**
	 * @brief Creates an isolated instance of Cobra
	 * @details All scripts and objects need to be created through an isolate.
	 */
	class Isolate
	{
	public:
		void Dispose();
		static Isolate* New();
		Context* CreateContext();
	};

	class String
	{
	private:
		const char* fileName;
		
	public:
		static String* New(Isolate* isolate);
		static String* New(Isolate* isolate, const char* string);
		static String* New(Isolate* isolate, const char* string, const char* path);
		static String* NewFromFile(Isolate* isolate, const char* path);
		static String* NewFromBase(Isolate* isolate);
		bool IsEmpty();
	};

	class Script
	{
	public:
		static Script* Compile(Context* context, String* string);
		void Run();
		bool HasError();
		std::string GetErrorMsg();
	};

}

void Compile(std::string* source);

#endif // COBRA_H_