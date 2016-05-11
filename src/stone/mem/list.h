// Copyright 2016 Chase Willden and The StoneLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef LIST_H_
#define LIST_H_

namespace Stone {
namespace internal{

	static const int DEFAULT_LIST_SIZE = 10;

	template<class T>
	class List
	{
	private:
		T* array[DEFAULT_LIST_SIZE];
		int spot;
		List* next;
		int size;
	public:
		List(){spot = 0;next = NULL;}
		~List(){}

		struct Chunk {
			T* t;
			int spot;
		};

		Chunk InsertDeep(T* t, int depth){
			if (spot == DEFAULT_LIST_SIZE){
				if (next == NULL) next = new List<T>;
				next->InsertDeep(t, depth + spot);
			}
			else{
				array[spot++] = t;
			}
		}

		Chunk Insert(T* t){
			size++;
			if (spot == DEFAULT_LIST_SIZE){
				if (next == NULL) next = new List<T>;
				return next->InsertDeep(t, spot);
			}
			else{
				array[spot++] = t;
				return &array[spot - 1];
			}
		}
	};

} // namespace internal
} // namespace Stone

#endif // LIST_H_