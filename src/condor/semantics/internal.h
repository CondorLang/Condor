// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef INTERNAL_H_
#define INTERNAL_H_

#include "condor/ast/node.h"
#include "condor/token/token.h"
#include "condor/global.h"
#include "condor/error/error.h"
#include "condor/clock.h"
#include "condor/types/clock/clock.h"
#include "api.h"

#include <string>
#include <iostream>

namespace Condor {
namespace internal{

	class ASTNode;
	class ASTFuncCall;
	class ASTLiteral;
	typedef ASTNode* (*InternalFunctionCallback)(Isolate* iso, std::vector<ASTLiteral*> lits);

	class Internal
	{
	public:
		static ASTNode* PrintF(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* ReadLine(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* GetStringLength(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* GetLiteralType(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* ReadFile(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* FileExists(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* WriteFile(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* Quit(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* GetPlatform(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* GetClockId(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* StartClock(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* StopClock(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* GetClockDuration(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* GetTime(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* GetDate(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* GetVersion(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* DeleteFile(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* GetArrayLength(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* ArrayPush(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* ArrayErase(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* ArrayMerge(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* GetObjectName(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* ReadDir(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* IsDir(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* MemoryAudit(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* PauseThread(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* GetStackTrace(Isolate* iso, std::vector<ASTLiteral*> lits);
		static ASTNode* CallInternal(Isolate* iso, InternalFunctionCallback call, std::vector<ASTLiteral*> nodes){return call(iso, nodes);}
		static TOKEN Bind(ASTFuncCall* call);
	};

} // namespace internal
}

#endif // INTERNAL_H_
