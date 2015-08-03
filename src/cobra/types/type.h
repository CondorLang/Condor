#ifndef TYPE_H_
#define TYPE_H_

#include "../ast/ast.h"
#include "../token/token.h"

namespace Cobra {

	class ASTLiterary;

	class Type
	{
	public:
		Type(){}
		~Type(){}
		TOKEN typeof;
		std::string name;
		static ASTLiterary* ToASTLiterary(Type* type);
	};

	class Int : public Type
	{
	public:
		Int(){typeof = INT;}
		~Int(){}
		void SetAST(ASTLiterary* lit);
		int value;
	};
}

#endif // TYPE_H_
