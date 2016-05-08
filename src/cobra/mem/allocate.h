// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef ALLOCATE_H_
#define ALLOCATE_H_

//http://www.codeproject.com/Articles/15527/C-Memory-Pool#HowDoesItWork

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <map>
#include <vector>
#include "cobra/flags.h"
#include "cobra/global.h"
#include "list.h"

namespace Cobra {
namespace internal{

	const static size_t KB = 1000;

	const static size_t DEFAULT_MEMORY_POOL_SIZE = KB * 8; // 8kb
	const static size_t DEFAULT_MEMORY_CHUNK_SIZE = 260;
	const static size_t DEFAULT_MEMORY_SIZE_TO_ALLOCATE = DEFAULT_MEMORY_CHUNK_SIZE * 2.2; // used in XL

	typedef struct Chunk {
		void* data;
		size_t used;
		size_t size;
		bool isAllocationChunk;
		bool isLastChunk;
		Chunk* next;
	} Chunk;	

	class MemoryPool
	{
	private: 
		size_t kChunkSize;
		size_t kTotalSize;
		size_t kUsedSize;
		size_t kFreeSize;
		size_t kMinSize;
		Chunk* kFirstChunk;
		Chunk* kLastChunk;
		Chunk* kCursorChunk;
		int kChunkCount;
		int counter;
		std::map<byte*, Chunk*> kChunkOrg;
		std::vector<Chunk*> kUnused;

	public:
		MemoryPool(const size_t poolSize = DEFAULT_MEMORY_POOL_SIZE,
							 const size_t chunkSize = DEFAULT_MEMORY_CHUNK_SIZE,
							 const size_t memorySize = DEFAULT_MEMORY_SIZE_TO_ALLOCATE);
		~MemoryPool();
		void AllocateMemory(const size_t size);
		size_t CalculateBestMemoryBlockSize(const size_t size);
		unsigned int CalculateNeededChunks(const size_t size);
		bool LinkChunksToData(Chunk* ptrNewChunk, unsigned int chunkCount, byte* ptrNewMemBlock);
		Chunk* SetChunkDefaults(Chunk* chunk);
		bool RecalcChunkMemorySize(Chunk* chunk, unsigned int size);
		void FreeAllAllocatedMemory();
		void DeallocateAllChunks();
		void* GetMemory(const size_t size);
		Chunk* FindChunkSuitableToHoldMemory(const size_t size);
		Chunk* SkipChunks(Chunk* chunk, unsigned int chunkToSkip);
		size_t MaxValue(const size_t a, const size_t b);
		void SetMemoryChunkValues(Chunk* chunk, const size_t size);
		size_t GetFreeSize(){return kFreeSize;}
		size_t GetUsedSize(){return kUsedSize;}
		size_t GetTotalSize(){return kTotalSize;}
		void FreeMemory(void* ptr, const size_t size);
		Chunk* FindChunkHoldingPointerTo(void* ptr);
		bool debug;
		std::string name;
	};

	class Allocate
	{
	public:
		static void* New(size_t size);
		static void Delete(void* ptr);
		static void OutOfMemory();
	};

} // namespace internal
} // namespace Cobra

#endif // ALLOCATE_H_