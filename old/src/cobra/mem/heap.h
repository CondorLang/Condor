#ifndef HEAP_H_
#define HEAP_H_

#define MAX_HEAP_SIZE 1000

#include <vector>
#include <string>
#include "cobra/globals.h"
#include "cobra/token/token.h"
#include "cobra/flags.h"

namespace Cobra {
namespace internal{

	class Allocate;

	struct HeapObject {
		Address address;
		TOKEN type;
		bool deleted;
	};

	class Heap
	{
	private:
		int size;
		HeapObject items[MAX_HEAP_SIZE];
	public:
		Heap(){size = 0;};
		~Heap(){}
		HeapObject* Get(int idx){
			return &items[idx];
		}
		int Size(){return size;}
		HeapObject* Insert(HeapObject val){
			val.deleted = false;
			bool heapInsertFlag = HEAP_INSERT;
			if (heapInsertFlag) printf("\tInserting: %d\t", size + 1);
			items[size] = val;
			return &items[size++];
		}
		bool IsValidAddress(Address addr){
			for (int i = 0; i < size - 1; i++){
				if (items[i].address == addr){return true;}
			}
			return false;
		}
		bool Delete(int idx){
			HeapObject obj = items[idx];
			if (obj.address != NULL && !obj.deleted){
				bool heapInsertFlag = HEAP_DELETE;
				if (heapInsertFlag){
					Token* tok = new Token(obj.type);
					printf("\tdeleting:  %d\t\t%s\n", size, tok->String().c_str());
					delete tok;
				}
				HeapObject* o = &items[idx];
				if (!o->deleted) delete obj.address;
				o->deleted = true;
			}
			if (idx == size - 1){
				size--;
			}
			else{
				for (int i = idx; i < size - 1; i++){
					items[i] = items[i + 1];
				}
				size--;
			}
			return true;
		}
	};

	class HeapStore
	{
	private:
		std::vector<Heap> heaps;
	public:
		HeapStore(){}
		~HeapStore(){
			heaps.erase(heaps.begin(), heaps.end());
		}
		bool IsValidAddress(Address addr){
			for (int i = 0; i < heaps.size(); i++){
				if (heaps[i].IsValidAddress(addr)){return true;}
			}
			return false;
		}
		HeapObject* Insert(HeapObject val){
			bool heapInsertFlag = HEAP_INSERT;
			if (heaps.empty()){
				Heap newHeap;
				heaps.push_back(newHeap);
				if (heapInsertFlag){
					printf("Heap #%lu", heaps.size());
				}
			}
			else{
				Heap* heap = &heaps.back();
				if (heap == NULL || heap->Size() == MAX_HEAP_SIZE){
					Heap newHeap;
					heaps.push_back(newHeap);
					if (heapInsertFlag){
						printf("Heap #%lu", heaps.size());
					}
				}
			}
			return heaps[heaps.size() - 1].Insert(val);
		}
		void FlushByTypeString(std::string types){
			// printf("d: %lu\n", heaps.size());
			// for (int i = 0; i < heaps.size(); i++){
			// 	for (int j = heaps[i].Size(); j != 0; j--){
			// 		HeapObject* obj = heaps[i].Get(j);
			// 		if (types.find(Token::ToString(obj->type)) != std::string::npos) {
			// 		  heaps[i].Delete(j - 1);
			// 		}
			// 	}
			// }
		}
		void FlushAll(){
			// for (int i = heaps.size() - 1; i != -1; i--){
			// 	for (int j = heaps[i].Size() - 1; j != -1; j = heaps[i].Size() - 1){
			// 		heaps[i].Delete(j);
			// 	}
			// 	heaps.pop_back();
			// }
		}
	};

} // namespace internal
} // namespace Cobra

#endif // HEAP_H_