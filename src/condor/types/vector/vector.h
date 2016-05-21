// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef VECTOR_H_
#define VECTOR_H_

#include <stddef.h>
#include <stdio.h>
#include <typeinfo>
#include "condor/global.h"

namespace Condor {
namespace internal{

	class Isolate;

	void* NewVectorItem(Isolate* isolate, size_t size);
	void EraseVectorItem(Isolate* isolate, size_t size, void* ptr);
	void InvalidVectorAccess(int idx, int size);
	void InternalVectorError(int idx, int size);
	void P(const char* msg);
	void P(int i);

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
		Vector(Isolate* iso){isolate = iso; kFirst = NULL; kLast = NULL; kCursor = NULL; kCount = 0; kCursorLoc = 0;}
		~Vector(){}
		void SetIsolate(Isolate* iso){isolate = iso;}
		int size(){return kCount;}
		bool empty(){return kCount == 0 && kFirst == NULL;}
		void push_back(T t){
			if (kFirst == NULL){
				void* pt = NewVectorItem(isolate, sizeof(VectorItem<T>));
				VectorItem<T>* p = new(pt) VectorItem<T>();
				kFirst = p;
				kFirst->data = t;
				kCursor = kFirst;
				kLast = kFirst;
				kCount = 1;
			}
			else{
				void* pt = NewVectorItem(isolate, sizeof(VectorItem<T>));
				VectorItem<T>* p = new(pt) VectorItem<T>();
				kLast->next = p;
				kLast->next->prev = kLast;
				kLast = kLast->next;
				kLast->data = t;
				kCount++;
			}
		}
		VectorItem<T>* begin(){return kFirst;}
		VectorItem<T>* end(){return kLast;}
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
			if (idx >= size()){
				InvalidVectorAccess(idx, size());
				return;
			}
			else if (idx == -1) {
				P("Cannot erase idx -1");
				return;
			}
			kCursor = kFirst;
			kCursorLoc = 0;
			while (kCursorLoc < idx){
				if (kCursor == NULL) return;
				kCursor = kCursor->next;
				kCursorLoc++;
			}
			VectorItem<T>* item = kCursor;

			if (kCursorLoc == 0 || kFirst == kCursor){
				if (kFirst->next != NULL){
					kFirst = kFirst->next;
					kFirst->prev = NULL;
					kCursor = kFirst;
				}
				else{
					kFirst = NULL;
					kCursor = NULL;
				}
			}
			if (kCursor && kCursor->next) kCursor->next->prev = kCursor->prev;
			if (kCursor && kCursor->prev) kCursor->prev->next = kCursor->next;
			kCount--;
			item->~VectorItem();
			EraseVectorItem(isolate, sizeof(VectorItem<T>), item);
		}
		void insert(VectorItem<T>* idx, T t){
			if (idx->prev == NULL){
				void* pt = NewVectorItem(isolate, sizeof(VectorItem<T>));
				VectorItem<T>* p = new(pt) VectorItem<T>();
				kCursor = p;
				kCursor->data = t;
				kCursor->next = kFirst;
				kFirst = kCursor;
			}
		}
		void insert(VectorItem<T>* spot, VectorItem<T>* start, VectorItem<T>* end){
			
		}
		T get(int idx){
			if (idx >= size()){
				InvalidVectorAccess(idx, size());
				return -1;
			}
			kCursor = kFirst;
			kCursorLoc = 0;
			for (int i = 0; i < idx; i++){
				if (kCursor == NULL){
					InvalidVectorAccess(idx, size());
					return -1;
				}
				kCursor = kCursor->next;
				kCursorLoc++;
			}
			if (kCursor == NULL){
				InternalVectorError(idx, size());
				return -1;
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
