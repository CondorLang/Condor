#include "isolate.h"

namespace Cobra {
namespace internal{

	Isolate::Isolate(){
		heapstore = new HeapStore();
		factory = new Factory(this);
	}

	Isolate::~Isolate(){
		delete heapstore;
		delete factory;
	}

	void Isolate::_enter(){
		_currentIsolate = this;
	}

	void Isolate::_exit(){
		_currentIsolate = NULL;
	}

	HeapObject* Isolate::Insert(HeapObject obj){
		HeapObject* o = heapstore->Insert(obj);
		bool heapInsertFlag = HEAP_INSERT;
		if (heapInsertFlag){
			Token* tok = new Token(obj.type);
			printf("\t%s\n", tok->String().c_str());
			delete tok;
		}
		return o;
	}

	void Isolate::FlushAST(){

		std::string flushing = "kAstFile kAstFor kFunc kScope kAstInclude kAstImport kAstFunc kAstBlock";
		flushing += "kAstVar kAstUnaryExpr kAstBinaryExpr kAstObjectMemberChainExpr kAstIdent" + 
		flushing += "kAstFuncCallExpr kAstArrayMemberExpr kAstObject kAstIf kAstElse kAstWhile" + 
		flushing += "kAstFor kAstFile";
		heapstore->FlushByTypeString(flushing);
	}

	void Isolate::FlushAll(){
		heapstore->FlushAll();
	}

	void Isolate::SetContext(Context* context){
		this->context = context;
	}

	bool Isolate::IsAddressValid(Address addr){
		return heapstore->IsValidAddress(addr);
	}

} // namespace internal
} // namespace Cobra