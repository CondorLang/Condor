#ifndef VECTOR_H_
#define VECTOR_H_

#include <stddef.h>

namespace Cobra {
namespace internal{

	class Isolate;

	void* NewVectorItem(Isolate* isolate, size_t size);

	template<class T>
	class VectorItem
	{
	public:
		VectorItem(T t){data = t; next = NULL;prev = NULL;}
		VectorItem(){next = NULL; prev = NULL;}
		~VectorItem(){}
		T data;
		VectorItem<T>* next;
		VectorItem<T>* prev;
	};

	template<class T>
	class Vector
	{
	private:
		VectorItem<T>* kCursor;
		VectorItem<T>* kFirst;
		VectorItem<T>* kLast;
		int kCount;
		int kCursorLoc;

		Isolate* isolate;
	public:
		Vector(){isolate = NULL; kFirst = NULL; kLast = NULL; kCursor = NULL; kCount = 0; kCursorLoc = 0;}
		~Vector(){}
		void SetIsolate(Isolate* iso){isolate = iso;}
		int size(){return kCount;}
		bool empty(){return kCount == 0;}
		void push_back(T t){
			if (kFirst == NULL){
				kFirst = (VectorItem<T>*) NewVectorItem(isolate, sizeof(VectorItem<T>));
				kFirst->data = t;
				kCursor = kFirst;
				kCount++;
			}
			else{
				kCursor->next = (VectorItem<T>*) NewVectorItem(isolate, sizeof(VectorItem<T>));
				kCursor->next->prev = kCursor;
				kCursor = kCursor->next;
				kCursor->data = t;
				kCount++;
			}
		}
		VectorItem<T>* begin(){return kFirst;}
		int find(T t){
			kCursor = kFirst;
			for (int i = 0; i < size(); i++){
				if (kCursor == NULL) return -1;
				if (kCursor->data == t) return i;
				kCursor = kCursor->next;
				kCursorLoc++;
			}
			return -1;
		}
		void erase(int idx){
			if (idx >= kCount) return;
			else if (idx == -1) return;
			kCursor = kFirst;
			kCursorLoc = 0;
			while (kCursorLoc != idx){
				if (kCursor == NULL) return;
				kCursor = kCursor->next;
				kCursorLoc++;
			}
			if (kCursor->prev != NULL) kCursor->prev->next = kCursor->next;
			if (kCursor->next != NULL) kCursor->next->prev = kCursor->prev;
			kCount--;
		}
		void insert(VectorItem<T>* idx, T t){
			if (idx->prev == NULL){
				kCursor = (VectorItem<T>*) NewVectorItem(isolate, sizeof(VectorItem<T>));
				kCursor->data = t;
				kCursor->next = kFirst;
				kFirst = kCursor;
			}
		}
		T get(int idx){
			if (idx >= kCount) return NULL;
			kCursor = kFirst;
			for (int i = 0; i < idx; i++){
				if (kCursor == NULL) return NULL;
				kCursor = kCursor->next;
				kCursorLoc++;
			}
			return kCursor->data;
		}
		T operator[](int idx){
			return get(idx);
		}
	};

} // namespace internal
}

#endif // VECTOR_H_
