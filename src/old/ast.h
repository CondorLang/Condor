#ifndef AST_H_
#define AST_H_

#include <string>
#include <map>
#include <vector>
#include "limits.h"

namespace Cobra {

	enum OPERATOR {
		PLUS,
		MINUS,
		DIVIDE,
		MULTIPLY,
		POWER,
		EQUALS,
		EXMARK,
		LESS_THAN,
		LESS_THAN_EQ,
		GREATER_THAN,
		GREATER_THAN_EQ
	};

	class ASTNode {
	private:
		bool readOnly;
	public:
		ASTNode();
		~ASTNode();
		ASTNode* Left;
		ASTNode* Right;
		std::string type;
	};

	class ASTOperator : public ASTNode {
	public: 
		ASTOperator();
		~ASTOperator();
		OPERATOR value;
		std::string str_value;
	};	

	/**
	 * Number Values
	 */

	class ASTDouble : public ASTNode {
	public:
		ASTDouble();
		~ASTDouble();
		double value;
	};

	class ASTInt : public ASTNode {
	public:
		ASTInt();
		~ASTInt();
		int value;
	};

	class ASTFloat : public ASTNode { 
	public: 
		ASTFloat();
		~ASTFloat();
		float value;
	};

	class ASTBoolean : public ASTNode {
	public: 
		ASTBoolean();
		~ASTBoolean();
		bool value;
	};

	/**
	 * Strings
	 */
	class ASTString : public ASTNode
	{
	public:
		ASTString();
		~ASTString();
		std::string value;
	};

	/**
	 * Block
	 */
	class ASTBlock : public ASTNode {
	public:
		ASTBlock(){}
		~ASTBlock(){}
		std::map<std::string, ASTNode*> map;
		std::vector<ASTNode*> vector;
	};

	/**
	 * Functions
	 */
	class ASTFunctionArg: public ASTNode {
	public:
		ASTFunctionArg();
		~ASTFunctionArg();
		std::string name;
		ASTNode* value;
	};

	class ASTFunction : public ASTNode {
	public:
		ASTFunction();
		~ASTFunction();
		std::vector<ASTFunctionArg*> args;
		std::string name;
		bool isAnonymous;
		ASTBlock* block;
	};

	/**
	 * Objects
	 */
	
	class ASTObjectMember : public ASTNode {
	public:
		ASTObjectMember();
		~ASTObjectMember();
		std::string name;
		ASTNode* value;
		bool isReadOnly;
		bool isPrivate;
	};

	class ASTObject : public ASTNode {
	public:
		ASTObject();
		~ASTObject();
		std::map<std::string, ASTObjectMember> members;
		std::string name;
	};

	class ASTVar : public ASTNode {
	public:
		ASTVar();
		~ASTVar();
		std::string name;
		ASTNode* value;
	};

	namespace ASTFactory {
		ASTVar* CreateVar(std::string name, ASTNode* expr);
	}

}

#endif // AST_H_
