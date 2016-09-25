// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "internal.h"

#include "condor/types/fs/fs.h" // to prevent a million errors

namespace Condor {
namespace internal{

	ASTNode* Internal::PrintF(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return NULL;
		ASTLiteral* v = lits[0];
		if (v->type == LITERAL) {
			printf("%s", v->value.c_str());
		}
		return NULL;
	}

	ASTNode* Internal::ReadLine(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() > 0) Internal::PrintF(iso, lits);
		ASTLiteral* l = ASTLiteral::New(iso);
		std::string result;
		std::getline (std::cin, result);
		l->value = result;
		return l;
	}

	ASTNode* Internal::GetStringLength(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return NULL;
		ASTLiteral* v = lits[0];
		if (v->type != LITERAL) return NULL;
		ASTLiteral* l = ASTLiteral::New(iso);
		l->litType = INT;
		int len = (int) v->value.length();
		l->value = std::to_string(len);
		l->calc = (double) len;
		l->isCalc = true;
		return l;
	}

	ASTNode* Internal::GetLiteralType(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return NULL;
		ASTLiteral* v = lits[0];
		if (v->type != LITERAL) return NULL;
		ASTLiteral* l = ASTLiteral::New(iso);
		l->litType = STRING;
		l->value = Token::ToString(v->litType);
		return l;
	}

	ASTNode* Internal::ReadFile(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return NULL;
		ASTLiteral* v = lits[0];
		if (v->type != LITERAL) return NULL;
		ASTLiteral* l = ASTLiteral::New(iso);
		l->litType = STRING;
		l->value = FS::ReadFile(v->value);
		return l;
	}

	ASTNode* Internal::FileExists(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return NULL;
		ASTLiteral* v = lits[0];
		if (v->type != LITERAL) return NULL;
		ASTLiteral* l = ASTLiteral::New(iso);
		l->litType = BOOLEAN;
		bool does = FS::FileExists(v->value);
		if (does) l->value = "true";
		else l->value = "false";
		return l;
	}

	ASTNode* Internal::WriteFile(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() < 2) return NULL;
		ASTLiteral* path = lits[0];
		ASTLiteral* contents = lits[1];
		FS::WriteFile(path->value, contents->value);
		return NULL;
	}

	ASTNode* Internal::Quit(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 1){
			ASTLiteral* code = lits[0];
			exit((int)code->calc);
			return NULL;
		}
		exit(0);
		return NULL;
	}

	ASTNode* Internal::GetPlatform(Isolate* iso, std::vector<ASTLiteral*> lits){
		ASTLiteral* platform = ASTLiteral::New(iso);
		platform->litType = STRING;
		#ifdef _WIN32
			platform->value = "Windows (32)";
			#ifdef _WIN64
		  	platform->value = "Windows (64)";
		  #endif
		#elif __APPLE__
			#include "TargetConditionals.h"
		    #if TARGET_IPHONE_SIMULATOR
		     	platform->value = "IOS Simulator";
		    #elif TARGET_OS_IPHONE
		    	platform->value = "IOS";
		    #elif TARGET_OS_MAC
		    	platform->value = "Mac";
		    #else
		    	platform->value = "Unknown Apple Product";
		    #endif
		#elif __linux__
			platform->value = "Linux";
		#elif __unix__ // all unices not caught above
			platform->value = "Unix";
		#elif defined(_POSIX_VERSION)
			platform->value = "Posix";
		#else
			platform->value = "Unknown";
		#endif
		return platform;
	}

	ASTNode* Internal::GetClockId(Isolate* iso, std::vector<ASTLiteral*> lits){
		ASTLiteral* result = ASTLiteral::New(iso);
		result->litType = INT;
		if (CBClockContainer::GlobalClockContainer == NULL){
			CBClockContainer::GlobalClockContainer = CBClockContainer::New(iso);
		}
		result->calc = CBClockContainer::GlobalClockContainer->AddClock();
		result->isCalc = true;
		return result;
	}

	ASTNode* Internal::StartClock(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return NULL;
		ASTLiteral* lit = lits[0];
		Clock* clock = CBClockContainer::GlobalClockContainer->GetClock(lit->calc - 1);
		if (clock == NULL) {
			// TODO: Throw Error
			return NULL;
		}
		clock->Start();
		return NULL;
	}

	ASTNode* Internal::StopClock(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return NULL;
		ASTLiteral* lit = lits[0];
		Clock* clock = CBClockContainer::GlobalClockContainer->GetClock((int) (lit->calc - 1));
		if (clock == NULL) {
			// TODO: Throw Error
			return NULL;
		}
		clock->Stop();
		return NULL;
	}

	ASTNode* Internal::GetClockDuration(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return NULL;
		ASTLiteral* lit = lits[0];
		Clock* clock = CBClockContainer::GlobalClockContainer->GetClock((int) (lit->calc - 1));
		if (clock == NULL) {
			// TODO: Throw Error
			return NULL;
		}
		lit->calc = clock->GetDuration();
		lit->isCalc = true;
		lit->litType = DOUBLE;
		return lit;
	}

	ASTNode* Internal::GetTime(Isolate* iso, std::vector<ASTLiteral*> lits){
		ASTLiteral* lit = ASTLiteral::New(iso);
		lit->calc = (double) Clock::GetTime();
		lit->isCalc = true;
		lit->litType = LONG;
		return lit;
	}

	ASTNode* Internal::GetDate(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return ASTUndefined::New(iso);
		ASTLiteral* lit = lits[0];
		std::string format = "%a %b %d %H:%M:%S %Y";
		if (lits.size() > 1) format = lits[1]->value;
		ASTLiteral* result = ASTLiteral::New(iso);
		result->value = Clock::GetDate(lit->calc, format);
		return result;
	}

	ASTNode* Internal::GetVersion(Isolate* iso, std::vector<ASTLiteral*> lits){
		ASTLiteral* result = ASTLiteral::New(iso);
		result->litType = STRING;
		result->value = std::to_string(MAJOR) + "." + std::to_string(MINOR) + "." + std::to_string(BUILD);
		return result;
	}

	ASTNode* Internal::DeleteFile(Isolate* iso, std::vector<ASTLiteral*> lits){
		ASTLiteral* result = ASTLiteral::New(iso);
		if (lits.size() == 0) {
			result->calc = 0;
			result->value = "false";
			result->litType = FALSE_LITERAL;
			return result;
		}
		ASTLiteral* lit = lits[0];
		result->calc = (int) FS::DeleteFile(lit->value);
		return result;
	}

	ASTNode* Internal::GetArrayLength(Isolate* iso, std::vector<ASTLiteral*> lits){
		ASTLiteral* result = ASTLiteral::New(iso);
		result->litType = INT;
		if (lits.size() == 0 || lits[0]->type != ARRAY) {
			result->calc = 0;
			result->value = "0";
		}
		else{
			ASTArray* ary = (ASTArray*) lits[0];
			result->calc = ary->members.size();
		}
		return result;
	}

	ASTNode* Internal::ArrayPush(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() < 2 || lits[0]->type != ARRAY) {
			return ASTUndefined::New(iso);
		}
		ASTArray* result = (ASTArray*) lits[0];
		ASTLiteral* val = lits[1];
		result->members.push_back(val);
		return result;
	}

	ASTNode* Internal::ArrayErase(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() < 2 || lits[0]->type != ARRAY) {
			return ASTUndefined::New(iso);
		}
		ASTArray* result = (ASTArray*) lits[0];
		ASTLiteral* val = lits[1];
		if (result->members.size() <= val->calc) return result;
		result->members.erase(result->members.begin() + ((int) val->calc));
		return result;
	}

	ASTNode* Internal::ArrayMerge(Isolate* iso, std::vector<ASTLiteral*> lits){
		//if (lits.size() == 0) return ASTUndefined::New(iso);
		if (lits.size() < 2 || lits[0]->type != ARRAY) {
			return ASTUndefined::New(iso);
		}
		ASTArray* result = (ASTArray*) lits[0];
		ASTLiteral* second = (ASTLiteral*) lits[1];
		if (second->type == ARRAY){ // base []
			ASTArray* ary = (ASTArray*) second;
			result->members.insert(result->members.end(), ary->members.begin(), ary->members.end());
		}
		else{
			result->members.insert(result->members.end(), second);
		}
		return result;
	}

	ASTNode* Internal::GetObjectName(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return ASTUndefined::New(iso);
		if (lits[0]->type != OBJECT_INSTANCE) return ASTUndefined::New(iso);
		ASTObjectInstance* obj = (ASTObjectInstance*) lits[0];
		if (obj->base == NULL) return ASTUndefined::New(iso);
		ASTLiteral* result = ASTLiteral::New(iso);
		result->value = obj->base->name;
		result->litType = STRING;
		return result;
	}

	ASTNode* Internal::ReadDir(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return ASTUndefined::New(iso);
		std::string dirPath = lits[0]->value;
		bool recursive = false;
		if (lits.size() > 1) recursive = lits[1]->value == "true";
		std::vector<std::string> paths = FS::ReadDir(dirPath, recursive);
		ASTArray* result = ASTArray::New(iso);
		result->litType = ARRAY;
		for (int i = 0; i < paths.size(); i++){
			ASTLiteral* path = ASTLiteral::New(iso);
			path->litType = STRING;
			path->value = paths[i];
			result->members.push_back(path);
		}
		return result;
	}

	ASTNode* Internal::IsDir(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() == 0) return ASTUndefined::New(iso);
		std::string dirPath = lits[0]->value;
		ASTLiteral* result = ASTLiteral::New(iso);
		result->calc = FS::IsDir(dirPath);
		result->litType = BOOLEAN;
		return result;
	}

	ASTNode* Internal::MemoryAudit(Isolate* iso, std::vector<ASTLiteral*> lits){
		ASTLiteral* result = ASTLiteral::New(iso);
		result->calc = iso->MemoryAudit();
		result->value = std::to_string(result->calc);
		result->litType = INT;
		return result;
	}

	ASTNode* Internal::PauseThread(Isolate* iso, std::vector<ASTLiteral*> lits){
		return ASTUndefined::New(iso); // TODO: Implement thread sleep
	}

	ASTNode* Internal::SetNumberPercision(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() < 2) return ASTUndefined::New(iso);
		ASTLiteral* val = lits[0];
		ASTLiteral* percision = lits[1];
		ASTLiteral* result = ASTLiteral::New(iso);
		result->value = String::ToStringWithPercision(val->calc, (int) percision->calc);
		result->litType = DOUBLE;
		return result;
	}

	ASTNode* Internal::Sqrt(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() < 1) return ASTUndefined::New(iso);
		ASTLiteral* val = lits[0];
		ASTLiteral* result = ASTLiteral::New(iso);
		result->litType = DOUBLE;
		result->calc = sqrt(val->calc);
		return result;
	}

	ASTNode* Internal::GetTotalLocals(Isolate* iso, std::vector<ASTLiteral*> lits){
		if (lits.size() < 1) {
			ASTLiteral* result = ASTLiteral::New(iso);
			result->litType = INT;
			result->calc = 0;
			result->value = "0";
			return result;
		}
		ASTLiteral* val = lits[0];
		ASTLiteral* result = ASTLiteral::New(iso);
		result->litType = INT;
		result->calc = val->GetTotalLocals();
		result->value = std::to_string(val->GetTotalLocals());
		return result;
	}

	// TODO: Move to a macro
	TOKEN Internal::Bind(ASTFuncCall* call){
		if (call->name == "printf") {
			call->callback = Internal::PrintF;
			return UNDEFINED;
		}
		else if (call->name == "readln") {
			call->callback = Internal::ReadLine;
			return STRING;
		}
		else if (call->name == "getStringLength"){
			call->callback = Internal::GetStringLength;
			return INT;
		}
		else if (call->name == "getLitType"){
			call->callback = Internal::GetLiteralType;
			return STRING;
		}
		else if (call->name == "readFile"){
			call->callback = Internal::ReadFile;
			return STRING;
		}
		else if (call->name == "fileExists"){
			call->callback = Internal::FileExists;
			return BOOLEAN;
		}
		else if (call->name == "writeFile"){
			call->callback = Internal::WriteFile;
			return UNDEFINED;
		}
		else if (call->name == "quit"){
			call->callback = Internal::Quit;
			return UNDEFINED;
		}
		else if (call->name == "getPlatform"){
			call->callback = Internal::GetPlatform;
			return STRING;
		}
		else if (call->name == "getClockId"){
			call->callback = Internal::GetClockId;
			return INT;
		}
		else if (call->name == "startClock"){
			call->callback = Internal::StartClock;
			return UNDEFINED;
		}
		else if (call->name == "stopClock"){
			call->callback = Internal::StopClock;
			return UNDEFINED;
		}
		else if (call->name == "getDuration"){
			call->callback = Internal::GetClockDuration;
			return DOUBLE;
		}
		else if (call->name == "getDate"){
			call->callback = Internal::GetDate;
			return STRING;
		}
		else if (call->name == "getTime"){
			call->callback = Internal::GetTime;
			return DOUBLE;
		}
		else if (call->name == "getVersion"){
			call->callback = Internal::GetVersion;
			return STRING;
		}
		else if (call->name == "deleteFile"){
			call->callback = Internal::DeleteFile;
			return BOOLEAN;
		}
		else if (call->name == "getArrayLength"){
			call->callback = Internal::GetArrayLength;
			return INT;
		}
		else if (call->name == "arrayPush"){
			call->callback = Internal::ArrayPush;
			return ARRAY;
		}
		else if (call->name == "arrayErase"){
			call->callback = Internal::ArrayErase;
			return ARRAY;
		}
		else if (call->name == "arrayMerge"){
			call->callback = Internal::ArrayMerge;
			return ARRAY;
		}
		else if (call->name == "getObjectName"){
			call->callback = Internal::GetObjectName;
			return STRING;
		}
		else if (call->name == "readDir"){
			call->callback = Internal::ReadDir;
			return ARRAY;
		}
		else if (call->name == "isDir"){
			call->callback = Internal::IsDir;
			return BOOLEAN;
		}
		else if (call->name == "memoryAudit"){
			call->callback = Internal::MemoryAudit;
			return INT;
		}
		else if (call->name == "pauseThread"){
			call->callback = Internal::PauseThread;
			return UNDEFINED;
		}
		else if (call->name == "setNumberPercision"){
			call->callback = Internal::SetNumberPercision;
			return DOUBLE; // Is this true?
		}
		else if (call->name == "sqrt"){
			call->callback = Internal::Sqrt;
			return DOUBLE;
		}
		else if (call->name == "getTotalLocals"){
			call->callback = Internal::GetTotalLocals;
			return INT;
		}
		else throw Error::UNDEFINED_FUNC;
	}

} // namespace internal
}