#include "isolate.h"

namespace Cobra {
namespace internal{

	Isolate::Isolate(){
		factory = new Factory(this);
		const static size_t poolSize = KB * 8; // 8kb
		const static size_t chunkSize2 = 35;
		const static size_t sizeToAllocate2 = DEFAULT_MEMORY_CHUNK_SIZE * 2;
		small = new MemoryPool(poolSize, chunkSize2, sizeToAllocate2);
		small->name = "small";

		const static size_t chunkSize = 121;
		const static size_t sizeToAllocate = DEFAULT_MEMORY_CHUNK_SIZE * 2;
		medium = new MemoryPool(poolSize, chunkSize, sizeToAllocate);
		medium->name = "medium";

		large = new MemoryPool();
		large->name = "large";
	}

	Isolate::~Isolate(){
		delete small;
		delete medium;
		delete large;
		delete factory;
	}

	void* Isolate::GetMemorySmall(const size_t size){
		return small->GetMemory(size);
	}

	void* Isolate::GetMemory(const size_t size){
		return medium->GetMemory(size);
	}

	void* Isolate::GetMemoryLarge(const size_t size){
		return large->GetMemory(size);
	}

	void Isolate::FreeMemory(void* ptr, const size_t size, std::string whichOne){
		if (whichOne == "small") small->FreeMemory(ptr, size);
		if (whichOne == "medium") medium->FreeMemory(ptr, size);
		if (whichOne == "large") large->FreeMemory(ptr, size);
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