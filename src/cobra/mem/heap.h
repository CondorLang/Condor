#ifndef HEAP_H_
#define HEAP_H_

#define MAX_HEAP_SIZE 1000

#include <vector>
#include <string>
#include "cobra/globals.h"
#include "cobra/token/token.h"

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
			items[size] = val;
			return &items[size++];
		}
		bool Delete(int idx){
			HeapObject obj = items[idx];
			delete obj.address;

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
			if (heaps.empty()){
				Heap newHeap;
				heaps.push_back(newHeap);
			}
			else{
				Heap* heap = &heaps.back();
				if (heap == NULL || heap->Size() > MAX_HEAP_SIZE){
					Heap newHeap;
					heaps.push_back(newHeap);
				}
			}
			return heaps[heaps.size() - 1].Insert(val);
		}
		void FlushByTypeString(std::string types){
			for (int i = 0; i < heaps.size(); i++){
				for (int j = 0; j < heaps[i].Size(); j++){
					HeapObject* obj = heaps[i].Get(j);
					if (types.find(Token::ToString(obj->type)) != std::string::npos) {
					  
					}
				}
			}
		}
	};

} // namespace internal
} // namespace Cobra

#endif // HEAP_H_