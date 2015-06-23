#include "ast.h"

using namespace Cobra;

ASTNode::ASTNode(){
	readOnly = false;
	Left = NULL;
	Right = NULL;
	type = "undefined";
}

ASTNode::~ASTNode(){
	delete Left;
	delete Right;
}

ASTOperator::ASTOperator(){
	type = "operator";
}
ASTOperator::~ASTOperator(){}

ASTInt::ASTInt(){
	type = "int";
}
ASTInt::~ASTInt(){}

ASTDouble::ASTDouble(){
	type = "double";
}
ASTDouble::~ASTDouble(){}

ASTFloat::ASTFloat(){
	type = "float";
}
ASTFloat::~ASTFloat(){}

ASTBoolean::ASTBoolean(){
	type = "boolean";
}
ASTBoolean::~ASTBoolean(){}

ASTString::ASTString(){
	type = "string";
}
ASTString::~ASTString(){}

ASTFunctionArg::ASTFunctionArg(){
	type = "functionArg";
}
ASTFunctionArg::~ASTFunctionArg(){
	delete value;
}

ASTFunction::ASTFunction(){
	type = "function";
}
ASTFunction::~ASTFunction(){}

ASTObject::ASTObject(){
	type = "object";
}
ASTObject::~ASTObject(){}

ASTObjectMember::ASTObjectMember(){
	isReadOnly = false;
	isPrivate = false;
	type = "objectMember";
}
ASTObjectMember::~ASTObjectMember(){
	delete value;
}

ASTVar::ASTVar(){
	type = "var";
}
ASTVar::~ASTVar(){}

ASTVar* ASTFactory::CreateVar(std::string name, ASTNode* expr){
	ASTVar* var = new ASTVar;
	var->name = name;
	var->value = expr;
	return var;
}