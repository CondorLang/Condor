#include "isolate.h"

namespace Cobra {
namespace internal{

	Isolate::Isolate(){
		factory = new Factory(this);
		mp = new MemoryPool();
	}

	Isolate::~Isolate(){
		delete mp;
		delete factory;
	}

	void* Isolate::GetMemory(const size_t size){
		return mp->GetMemory(size);
	}

	void Isolate::_enter(){
		_currentIsolate = this;
	}

	void Isolate::_exit(){
		bool en = EXPORTED_NODES;
		if (en){
			this->context->PrintExported(this);
		}
		_currentIsolate = NULL;
	}

	HeapObject* Isolate::Insert(HeapObject obj){
		HeapObject* o = heapstore.Insert(obj);
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
		heapstore.FlushByTypeString(flushing);
	}

	void Isolate::FlushAll(){
		heapstore.FlushAll();
	}

	void Isolate::SetContext(Context* context){
		this->context = context;
	}

	bool Isolate::IsAddressValid(Address addr){
		return heapstore.IsValidAddress(addr);
	}

} // namespace internal
} // namespace Cobra