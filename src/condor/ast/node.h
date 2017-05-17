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
		virtual ~ASTNode(){}
		std::string name;
		TOKEN type;
		int row;
		int col;
		int id;
		bool isExport;
		bool isInParen;
		bool allowGC;
		bool print;
		int progressDepth;
		int importScopeId;
		std::vector<TOKEN> visibility;
		std::vector<ASTLiteral*> locals; // used for recursion
		void AddLocal(ASTLiteral* l) {locals.push_back(l);}
		ASTLiteral* GetLocal(bool pop = true);
		int GetTotalLocals(){return (int) locals.size();}
		bool HasLocal(){return locals.size() > 0;}
		static ASTNode* New(Isolate* iso);
		static void SetDefaults(ASTNode* node, Isolate* iso);
		void Free(Isolate* iso);
		bool HasVisibility(TOKEN tok);
		virtual std::string ToString(int tabs);
	};

	class ASTToken : public ASTNode
	{
	public:
		static ASTToken* New(Isolate* iso, TOKEN tok);
		~ASTToken();
		Token* value;
		int Precedence(){return value->Precedence();}
	private:
		Isolate* iso;
	};

	class ASTImport : public ASTNode
	{
	public:
		std::string alias;
		std::vector<std::string> what;
		static ASTImport* New(Isolate* iso);
	};

	class ASTInclude : public ASTNode
	{
	public:
		std::string alias;
		std::vector<std::string> what;
		static ASTInclude* New(Isolate* iso);
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
		ASTVar* Clone(Isolate* iso, bool shallow = false);
		std::string ToString(int tabs);
	};

	class ASTFunc : public ASTNode
	{
	public:
		Scope* scope;
		std::vector<ASTVar*> args;
		TOKEN assignmentType;
		static ASTFunc* New(Isolate* iso);
		std::string ToString(int tabs);
	};

	class ASTExpr : public ASTNode
	{
	public:
		static ASTExpr* New(Isolate* iso);
		ASTLiteral* cast;
		bool isBoolean;
		std::string ToString(int tabs);
	};

	class ASTBreak : public ASTExpr
	{
	public:
		static ASTBreak* New(Isolate* iso);		
	};

	class ASTContinue : public ASTExpr
	{
	public:
		static ASTContinue* New(Isolate* iso);		
	};

	class ASTBinaryExpr : public ASTExpr
	{
	public:
		static ASTBinaryExpr* New(Isolate* iso);
		ASTExpr* left;
		ASTExpr* right;
		TOKEN op;
		bool isChain;
		std::string ToString(int tabs);
	};

	class ASTObject : public ASTNode
	{
	public:
		static ASTObject* New(Isolate* iso);
		Scope* scope;
		bool extend;
		std::string ToString(int tabs);
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
		std::string ToString(int tabs);
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
		std::string ToString(int tabs);
	};

	class ASTForExpr : public ASTExpr
	{
	public:
		static ASTForExpr* New(Isolate* iso);
		ASTNode* var;
		ASTExpr* condition;
		ASTExpr* tick;
		Scope* scope; 		
	};

	class ASTWhileExpr : public ASTExpr
	{
	public:
		static ASTWhileExpr* New(Isolate* iso);
		ASTExpr* condition;
		Scope* scope;		
	};

	class ASTTryCatchExpr : public ASTExpr
	{
	public:
		static ASTTryCatchExpr* New(Isolate* iso);
		Scope* tryScope;
		Scope* catchScope;
		std::vector<ASTVar*> catchParams;		
	};

	class ASTThrow : public ASTExpr
	{
	public:
		static ASTThrow* New(Isolate* iso);
		ASTExpr* expr;		
	};

	class ASTIf : public ASTExpr
	{
	public:
		static ASTIf* New(Isolate* iso);
		ASTExpr* condition;
		Scope* scope;
		bool isElse;
		std::vector<ASTIf*> elseIfs;		
	};

	class ASTDelete : public ASTExpr
	{
	public:
		static ASTDelete* New(Isolate* iso);
		ASTNode* node;		
	};

	class ASTCase : public ASTExpr
	{
	public:
		static ASTCase* New(Isolate* iso);
		ASTLiteral* condition;
		Scope* scope;
		bool isDefault;
	};

	class ASTSwitch : public ASTExpr
	{
	public:
		static ASTSwitch* New(Isolate* iso);
		std::vector<ASTCase*> cases;
		ASTNode* value;
	};

	class ASTUndefined : public ASTLiteral
	{
	public:
		static ASTUndefined* New(Isolate* iso);
	};

	class ASTArray : public ASTLiteral
	{
	public:
		static ASTArray* New(Isolate* iso);
		std::vector<ASTExpr*> members;
		bool Get;
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

		ASTFuncCall* constructor;
		bool constructorCalled;
		ASTObject* base;

		ASTObjectInstance* Clone(Isolate* iso, bool shallow = false);
		std::string ToString(int tabs);
	};

} // namespace internal
}

#endif // NODE_H_
