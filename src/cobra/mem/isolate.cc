#include "isolate.h"

namespace Cobra {
namespace internal{

	Isolate::~Isolate(){

	}

	void Isolate::_enter(){
		_currentIsolate = this;
	}

	void Isolate::_exit(){
		_currentIsolate = NULL;
	}

	HeapObject* Isolate::Insert(HeapObject obj){
		return heapstore->Insert(obj);
	}

	void Isolate::FlushAST(){

		std::string flushing = "kAstFile kAstFor kFunc kScope kAstInclude kAstImport kAstFunc kAstBlock";
		flushing += "kAstVar kAstUnaryExpr kAstBinaryExpr kAstObjectMemberChainExpr kAstIdent" + 
		flushing += "kAstFuncCallExpr kAstArrayMemberExpr kAstObject kAstIf kAstElse kAstWhile" + 
		flushing += "kAstFor kAstFile";
		heapstore->FlushByTypeString(flushing);
	}

} // namespace internal
} // namespace Cobra