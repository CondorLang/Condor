// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "internal.h"

#include "cobra/types/fs/fs.h" // to prevent a million errors

namespace Cobra {
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
		return lit;
	}

	ASTNode* Internal::GetTime(Isolate* iso, std::vector<ASTLiteral*> lits){
		ASTLiteral* lit = ASTLiteral::New(iso);
		lit->calc = (double) Clock::GetTime();
		lit->isCalc = true;
		lit->litType = LONG;
		return lit;
	}

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
		else if (call->name == "getTime"){
			call->callback = Internal::GetTime;
			return DOUBLE;
		}
		else throw Error::UNDEFINED_FUNC;
	}

} // namespace internal
}