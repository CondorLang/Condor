#ifndef AST_H_
#define AST_H_

#include <string>
#include <map>
#include "../token/token.h"
#include "../scanner/scanner.h"
#include "scope.h"

namespace Cobra {

	class ASTNode
	{
	public:
		ASTNode();
		~ASTNode(){}
		std::string name;
	};
}

#endif // AST_H_
