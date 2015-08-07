#ifndef TYPE_H_
#define TYPE_H_

#include "cobra/ast/ast.h"
#include "cobra/token/token.h"

namespace Cobra {
namespace internal{

	class ASTLiterary;

	class Type
	{
	public:
		Type(){typeOf = ILLEGAL;}
		~Type(){}
		std::string name;
		TOKEN typeOf;
		static ASTLiterary* ToASTLiterary(Type* type);
	};

	class Int : public Type
	{
	public:
		Int(){typeOf = INT;}
		~Int(){}
		void SetAST(ASTLiterary* lit);
		int value;
	};

	class Double : public Type
	{
	public:
		Double(){typeOf = DOUBLE;}
		~Double(){}
		void SetAST(ASTLiterary* lit);
		double value;
	};
} // namespace internal
}

#endif // TYPE_H_
