// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef INTERNAL_H_
#define INTERNAL_H_

#include "cobra/ast/node.h"
#include "cobra/token/token.h"
#include "cobra/global.h"
#include "cobra/error/error.h"
#include "cobra/clock.h"

#include <string>
#include <iostream>

namespace Cobra {
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
		static ASTNode* CallInternal(Isolate* iso, InternalFunctionCallback call, std::vector<ASTLiteral*> nodes){return call(iso, nodes);}
		static TOKEN Bind(ASTFuncCall* call);
	};

} // namespace internal
}

#endif // INTERNAL_H_
