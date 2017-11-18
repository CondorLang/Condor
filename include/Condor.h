// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef CONDOR_H_
#define CONDOR_H_

#include <string>
#include <vector>
#include <algorithm>

namespace Condor {

	const char* Version();
	void Initialize();

	void SetCommandLineFlags(int argc, const char* argv[]);
	std::string GetBaseFile();
		
	class Context
	{
	public:
		void Enter();
		void Exit();
		void Dispose();	
		void AllExport();	
	};
		
	/**
	 * @brief Creates an isolated instance of Condor
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
	public:
		static String* New(Isolate* isolate);
		static String* New(Isolate* isolate, const char* string);
		static String* New(Isolate* isolate, const char* string, const char* path);
		static String* NewFromFile(Isolate* isolate, const char* path);
		static String* NewFromBase(Isolate* isolate);
		bool IsEmpty();
		void Free(Isolate* isolate);
	};

	class Script
	{
	public:
		static Script* Compile(Context* context, String* string);
		void Run();
		bool HasError();
		void Free(Isolate* isolate);
		std::string GetErrorMsg();
	};

	class Directory
	{
	public:
		static std::vector<std::string> ReadDir(std::string path, bool recursive);
		static bool IsDir(std::string path);
	};

}

void Compile(std::string* source);

#endif // CONDOR_H_