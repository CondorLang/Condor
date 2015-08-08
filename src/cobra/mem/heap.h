#ifndef HEAP_H_
#define HEAP_H_

#define MAX_HEAP_SIZE 1000

#include <vector>
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
		int Size(){return size;}
		HeapObject* Insert(HeapObject val){
			items[size] = val;
			return &items[size++];
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
	};

} // namespace internal
} // namespace Cobra

#endif // HEAP_H_