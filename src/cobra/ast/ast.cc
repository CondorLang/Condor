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
		delete expr;
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
} // namespace Cobra