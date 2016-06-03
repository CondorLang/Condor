// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef NODE_H_
#define NODE_H_

#include <string>
#include <typeinfo>
#include <map>

#include "condor/global.h"
#include "condor/token/token.h"
#include "condor/mem/isolate.h"
#include "condor/ast/scope.h"
#include "condor/assert.h"
#include "condor/semantics/internal.h"

namespace Condor {
namespace internal{

	class Isolate;
	class ASTLiteral;
	class Scope;

	typedef ASTNode* (*InternalFunctionCallback)(Isolate* iso, std::vector<ASTLiteral*> lits);

	// TODO: Create a Expand() function which expands the node and the tree
	class ASTNode
	{
	public:
		ASTNode(){}
		~ASTNode(){}
		std::string name;
		TOKEN type;
		int row;
		int col;
		int id;
		bool isExport;
		bool isInParen;
		bool allowGC;
		bool print;
		int importScopeId;
		std::vector<TOKEN> visibility;
		ASTLiteral* local;
		static ASTNode* New(Isolate* iso);
		static void SetDefaults(ASTNode* node, Isolate* iso);
		void Free(Isolate* iso);
		bool HasVisibility(TOKEN tok);
		size_t Size(){return sizeof(ASTNode);}
	};

	class ASTToken : public ASTNode
	{
	public:
		static ASTToken* New(Isolate* iso, TOKEN tok);
		Token* value;
		int Precedence(){return value->Precedence();}
	};

	class ASTImport : public ASTNode
	{
	public:
		std::string alias;
		std::vector<std::string> what;
		static ASTImport* New(Isolate* iso);
		size_t Size(){return sizeof(ASTImport);}
	};

	class ASTInclude : public ASTNode
	{
	public:
		std::string alias;
		static ASTInclude* New(Isolate* iso);
		size_t Size(){return sizeof(ASTInclude);}
	};

	class ASTVar : public ASTNode
	{
	public:
		std::string baseName;
		TOKEN baseType;
		ASTNode* value;
		ASTNode* member;
		TOKEN assignmentType;
		TOKEN op;
		int scopeId;
		bool isArray;
		bool isObject;
		bool isArg;
		bool hasDefault;
		bool previouslyDeclared;
		int order;
		static ASTVar* New(Isolate* iso);		
		size_t Size(){return sizeof(ASTVar);}
		ASTVar* Clone(Isolate* iso, bool shallow = false);
	};

	class ASTFunc : public ASTNode
	{
	public:
		Scope* scope;
		std::vector<ASTVar*> args;
		TOKEN assignmentType;
		static ASTFunc* New(Isolate* iso);
		size_t Size(){return sizeof(ASTFunc);}
	};

	class ASTExpr : public ASTNode
	{
	public:
		static ASTExpr* New(Isolate* iso);
		ASTLiteral* cast;
		size_t Size(){return sizeof(ASTExpr);}
	};

	class ASTBreak : public ASTExpr
	{
	public:
		static ASTBreak* New(Isolate* iso);
		size_t Size(){return sizeof(ASTBreak);}		
	};

	class ASTContinue : public ASTExpr
	{
	public:
		static ASTContinue* New(Isolate* iso);
		size_t Size(){return sizeof(ASTContinue);}		
	};

	class ASTBinaryExpr : public ASTExpr
	{
	public:
		static ASTBinaryExpr* New(Isolate* iso);
		ASTExpr* left;
		ASTExpr* right;
		TOKEN op;
		bool isBoolean;
		bool isChain;
		size_t Size(){return sizeof(ASTBinaryExpr);}
	};

	class ASTObject : public ASTNode
	{
	public:
		static ASTObject* New(Isolate* iso);
		Scope* scope;
		bool extend;
		size_t Size(){return sizeof(ASTObject);}
	};

	class ASTLiteral : public ASTExpr
	{
	public:
		static ASTLiteral* New(Isolate* iso);
		std::string value;
		ASTVar* var;
		ASTObject* obj; // for this
		ASTExpr* member;
		TOKEN litType;
		TOKEN unary;
		double calc;
		bool isCast;
		int scopeId;
		bool isCalc;
		int isPost;
		bool allowAccess;
		ASTLiteral* Clone(Isolate* isolate, bool shallow = false);
		size_t Size(){return sizeof(ASTLiteral);}
	};

	class ASTFuncCall : public ASTExpr
	{
	public:
		static ASTFuncCall* New(Isolate* iso);
		std::vector<ASTExpr*> params;	
		bool isInternal;	
		bool isInit;
		ASTFunc* func;
		InternalFunctionCallback callback;
		size_t Size(){return sizeof(ASTFuncCall);}
	};

	class ASTForExpr : public ASTExpr
	{
	public:
		static ASTForExpr* New(Isolate* iso);
		ASTNode* var;
		ASTExpr* condition;
		ASTExpr* tick;
		Scope* scope; 		
		size_t Size(){return sizeof(ASTForExpr);}
	};

	class ASTWhileExpr : public ASTExpr
	{
	public:
		static ASTWhileExpr* New(Isolate* iso);
		ASTExpr* condition;
		Scope* scope;		
		size_t Size(){return sizeof(ASTWhileExpr);}
	};

	class ASTTryCatchExpr : public ASTExpr
	{
	public:
		static ASTTryCatchExpr* New(Isolate* iso);
		Scope* tryScope;
		Scope* catchScope;
		std::vector<ASTVar*> catchParams;		
		size_t Size(){return sizeof(ASTTryCatchExpr);}
	};

	class ASTThrow : public ASTExpr
	{
	public:
		static ASTThrow* New(Isolate* iso);
		ASTExpr* expr;		
		size_t Size(){return sizeof(ASTThrow);}
	};

	class ASTIf : public ASTExpr
	{
	public:
		static ASTIf* New(Isolate* iso);
		ASTExpr* condition;
		Scope* scope;
		bool isElse;
		std::vector<ASTIf*> elseIfs;	
		size_t Size(){return sizeof(ASTIf);}	
	};

	class ASTDelete : public ASTExpr
	{
	public:
		static ASTDelete* New(Isolate* iso);
		ASTNode* node;		
		size_t Size(){return sizeof(ASTDelete);}
	};

	class ASTCase : public ASTExpr
	{
	public:
		static ASTCase* New(Isolate* iso);
		ASTLiteral* condition;
		Scope* scope;
		bool isDefault;
		size_t Size(){return sizeof(ASTCase);}
	};

	class ASTSwitch : public ASTExpr
	{
	public:
		static ASTSwitch* New(Isolate* iso);
		std::vector<ASTCase*> cases;
		ASTNode* value;
		size_t Size(){return sizeof(ASTSwitch);}
	};

	class ASTUndefined : public ASTLiteral
	{
	public:
		static ASTUndefined* New(Isolate* iso);
		size_t Size(){return sizeof(ASTUndefined);}
	};

	class ASTArray : public ASTLiteral
	{
	public:
		static ASTArray* New(Isolate* iso);
		std::vector<ASTExpr*> members;
		bool Get;
		size_t Size(){return sizeof(ASTArray);}
		ASTArray* Clone(Isolate* iso, bool shallow = false);
	};

	// Eventually we should do a linked list for properties
	class ASTObjectInstance : public ASTLiteral
	{
	public:
		static ASTObjectInstance* New(Isolate* iso);
		std::map<std::string, ASTVar*> properties;
		bool HasProp(std::string name){return properties.find(name) != properties.end();}
		ASTVar* CreateProp(Isolate* isolate, std::string name);
		ASTVar* GetProp(Isolate* isolate, std::string name);
		void SetProp(Isolate* isolate, std::string name, ASTExpr* value);
		ASTFuncCall* constructor;
		bool constructorCalled;
		ASTObject* base;
		void PrintValues();
		ASTObjectInstance* Clone(Isolate* iso, bool shallow = false);
		size_t Size(){return sizeof(ASTObjectInstance);}		
	};

} // namespace internal
}

#endif // NODE_H_
