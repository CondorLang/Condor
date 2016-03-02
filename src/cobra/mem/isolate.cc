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

		const static size_t chunkSize = 80;
		const static size_t sizeToAllocate = DEFAULT_MEMORY_CHUNK_SIZE * 2;
		medium = new MemoryPool(poolSize, chunkSize, sizeToAllocate);
		medium->name = "medium";

		const static size_t chunkSize3 = 130;
		const static size_t sizeToAllocate3 = DEFAULT_MEMORY_CHUNK_SIZE * 2;
		large = new MemoryPool(poolSize, chunkSize3, sizeToAllocate3);
		large->name = "large";

		xl = new MemoryPool();
		xl->name = "xl";

		bool requested = MEMORY_REQUEST;
		if (requested){
			small->debug = true;
			medium->debug = true;
			large->debug = true;
			xl->debug = true;
		}
		gc = new GC();
	}

	Isolate::~Isolate(){
		delete small;
		delete medium;
		delete large;
		delete xl;
		delete factory;
	}

	void* Isolate::GetMemory(const size_t size){
		if (size < 35){
			return small->GetMemory(size);
		}
		else if (size < 80){
			return medium->GetMemory(size);
		}
		else if (size < 130){
			return large->GetMemory(size);
		}
		else{
			return xl->GetMemory(size);
		}
	}

	void Isolate::FreeMemory(void* ptr, const size_t size){
		if (size < 35) small->FreeMemory(ptr, size);
		else if (size < 80) medium->FreeMemory(ptr, size);
		else if (size < 130) large->FreeMemory(ptr, size);
		else xl->FreeMemory(ptr, size);
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

	void Isolate::RunGC(){
		gc->Run(small);
		gc->Run(medium);
		gc->Run(large);
		gc->Run(xl);
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