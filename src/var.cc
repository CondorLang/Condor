#include "var.h"

using namespace Cobra;

namespace Cobra{

	Var::Var(){

	}

	Var::~Var(){
		delete expr;
	}

	void Var::setExpr(Expression* expression){
		expr = expression;
	}
	
}