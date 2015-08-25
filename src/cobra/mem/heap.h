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

	struct HeapObject {
		Address address;
		TOKEN type;
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
			bool heapInsertFlag = HEAP_INSERT;
			if (heapInsertFlag) printf("\tInserting: %d\t", size + 1);
			items[size] = val;
			return &items[size++];
		}
		bool Delete(int idx){
			HeapObject obj = items[idx];
			if (obj.address != NULL){
				bool heapInsertFlag = HEAP_DELETE;
				if (heapInsertFlag){
					Token* tok = new Token(obj.type);
					printf("\tdeleting:  %d\t\t%s\n", size, tok->String().c_str());
					delete tok;
				}
				delete obj.address;
			}

			for (int i = idx; i < size - 1; i++){
				items[idx] = items[idx + 1];
			}
			size = size - 1;
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
				if (heap == NULL || heap->Size() > MAX_HEAP_SIZE){
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
			for (int i = 0; i < heaps.size(); i++){
				for (int j = heaps[i].Size(); j != 0; j--){
					HeapObject* obj = heaps[i].Get(j);
					if (types.find(Token::ToString(obj->type)) != std::string::npos) {
					  heaps[i].Delete(j - 1);
					}
				}
			}
		}
		void FlushAll(){
			for (int i = 0; i < heaps.size(); i++){
				for (int j = heaps[i].Size(); j != 0; j--){
					heaps[i].Delete(j - 1);
				}
			}
		}
	};

} // namespace internal
} // namespace Cobra

#endif // HEAP_H_