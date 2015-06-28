#include "ast.h"

namespace Cobra {
	ASTFile::~ASTFile(){
		delete scope;
	}
	ASTFunc::~ASTFunc(){
		delete body;
		for (std::map<std::string, ASTNode*>::iterator it = args.begin(); it != args.end(); ++it){
			delete it->second;
		}
	}
	ASTExpr::~ASTExpr(){
		delete value;
	}
	ASTUnaryExpr::~ASTUnaryExpr(){
		delete value;
		delete op;
	}
	ASTBinaryExpr::~ASTBinaryExpr(){
		delete Left;
		delete Right;
		delete op;
	}
	ASTFuncCallExpr::~ASTFuncCallExpr(){
		for (int i = 0; i < params.size(); i++){
			delete params[0];
		}
	}
	ASTArrayMemberExpr::~ASTArrayMemberExpr(){
		delete member;
		delete value;
	}
	ASTObject::~ASTObject(){
		for (std::map<std::string, ASTExpr*>::iterator it = members.begin(); it != members.end(); ++it){
			delete it->second;
		}
	}
} // namespace Cobra