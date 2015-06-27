#include "ast.h"

namespace Cobra {
	ASTNode::ASTNode(){}
	ASTFile::~ASTFile(){}
	ASTFunc::~ASTFunc(){
		delete body;
		for (std::map<std::string, ASTNode*>::iterator it = args.begin(); it != args.end(); ++it){
			delete it->second;
		}
	}
} // namespace Cobra