#ifndef VAR_H_
#define VAR_H_

#include <string>
#include "expression.h"

namespace Cobra {

	enum VAR_TYPE{
		VAR_INT,
		VAR_DOUBLE,
		VAR_FLOAT,
		VAR_SHORT,
		VAR_BOOL,
		VAR_STRING
	};

	class Var
	{
	private: 
		VAR_TYPE type;
		Expression* expr;

	public:
		Var();
		~Var();		
		void setExpr(Expression* expression);
	};
}

#endif // VAR_H_	
