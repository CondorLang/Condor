// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "allocate.h"

namespace Condor {
namespace internal{

	void* Allocate::New(size_t size){
		void* addr = calloc(1, size);
		if (addr == NULL) OutOfMemory();
		return addr;
	}

	void Allocate::Delete(void* ptr){
		free(ptr);
	}

	void Allocate::OutOfMemory(){
		printf("Out of Memory\n");
		exit(1);
	}

	MemoryPool::MemoryPool(const size_t poolSize, const size_t chunkSize, const size_t memorySize){
		kChunkSize = chunkSize;
		kFirstChunk = NULL;
		kLastChunk = NULL;
		kCursorChunk = NULL;
		kTotalSize = 0;
		kUsedSize = 0;
		kFreeSize = 0;
		kMinSize = memorySize;
		kChunkCount = 0;
		name = "Default";
		debug = false;
		AllocateMemory(poolSize);
	}

	MemoryPool::~MemoryPool(){
		bool memoryAudit = MEMORY_AUDIT;
		if (memoryAudit){
			printf("\nCondorLang (c) %c%c%c%c\n-----------------------------\n", BUILD_YEAR_CH0, BUILD_YEAR_CH1, BUILD_YEAR_CH2, BUILD_YEAR_CH3);
			printf("Name: %s\n", name.c_str());
			printf("Total Memory Used: \t%zu bytes\n", (kUsedSize));
			printf("Total Memory Unused: \t%lukb\n",  (kFreeSize / KB));
			printf("Total Memory Size: \t%lukb\n", (kTotalSize / KB));
			printf("Total Chunks Used: \t%d\n", kChunkCount);
			printf("Chunk Size: \t\t%lu bytes\n", kChunkSize);
		}
		FreeAllAllocatedMemory();
		DeallocateAllChunks();
	}

	void MemoryPool::DeallocateAllChunks(){
		Chunk* chunk = kFirstChunk;
	  	Chunk* chunkToDelete = NULL;
	  	while (chunk != NULL){
				if (chunk->isAllocationChunk){	
					if (chunkToDelete){
						Allocate::Delete((void*) chunkToDelete);
						chunkToDelete = NULL;
					}
					chunkToDelete = chunk;
				}
				chunk = chunk->next;
	  	}
	  	if (chunkToDelete) Allocate::Delete((void*) chunkToDelete); // catch the trailing chunk
	}

	void MemoryPool::FreeAllAllocatedMemory(){
		Chunk* chunk = kFirstChunk;
		while (chunk != NULL){
			if (chunk->isAllocationChunk && chunk->data != NULL){
				Allocate::Delete((void*) chunk->data);
			}
			chunk = chunk->next;
		}
	}

	void MemoryPool::FreeMemory(void* ptr, const size_t size){
		Chunk* chunk = FindChunkHoldingPointerTo(ptr);
		if (chunk != NULL){
			chunk->used = 0;
			chunk->size = kChunkSize;
			kUsedSize -= kChunkSize;
			kFreeSize += kChunkSize;
			kUnused.push_back(chunk);
		}
		kChunkCount--;
	}

	Chunk* MemoryPool::FindChunkHoldingPointerTo(void* ptr){
		if (kChunkOrg.find((byte*) ptr) != kChunkOrg.end()) return kChunkOrg[(byte*) ptr]; // quick lookup
		Chunk* chunk = kFirstChunk; // fail safe
		while (chunk != NULL){
			if (chunk->data == ((byte*) ptr)) break;
			chunk = chunk->next;
		}
		return chunk;
	}

	Chunk* MemoryPool::FindChunkSuitableToHoldMemory(const size_t size){
		unsigned int chunkToSkip = 0;
		Chunk* chunk = kCursorChunk;
		for (unsigned int i = 0; i < kChunkCount; i++){
			if (chunk != NULL){
				if (chunk == kLastChunk){
					chunk = kFirstChunk;
				}
				if (chunk->size >= size && chunk->used == 0){
					kCursorChunk = chunk;
					return chunk;
				}
				chunkToSkip = CalculateNeededChunks(chunk->used);
				if (chunkToSkip == 0) chunkToSkip = 1;
				chunk = SkipChunks(chunk, chunkToSkip);
			}
		}
		return NULL;
	}

	Chunk* MemoryPool::SkipChunks(Chunk* chunk, unsigned int chunkToSkip){
		Chunk* currentChunk = chunk;
		for (unsigned int i = 0; i < chunkToSkip; i++){
			if (currentChunk != NULL){
				currentChunk = currentChunk->next;
			}
			else{
				break;
			}
		}
		return currentChunk;
	}

	void* MemoryPool::GetMemory(const size_t size){
		if (debug){
			printf("%s - %lu\n", name.c_str(), size);
		}
		size_t bestBlockSize = CalculateBestMemoryBlockSize(size);

		if (kUnused.size() == 0){
			bestBlockSize = MaxValue(bestBlockSize, CalculateBestMemoryBlockSize(kMinSize));
			AllocateMemory(bestBlockSize);
			return GetMemory(size);
		}

		Chunk* chunk = kUnused.back();
		kUnused.pop_back();

		kUsedSize += bestBlockSize;
		kFreeSize -= bestBlockSize;
		SetMemoryChunkValues(chunk, bestBlockSize);
		kChunkOrg[(byte*) chunk->data] = chunk;
		return (void*) chunk->data;
	}

	void MemoryPool::SetMemoryChunkValues(Chunk* chunk, const size_t size){
		if (chunk != NULL){
			chunk->used = size;
		}
	}

	void MemoryPool::AllocateMemory(const size_t size){
		size_t bestBlockSize = CalculateBestMemoryBlockSize(size);
		unsigned int neededChunks = CalculateNeededChunks(size);
		byte* ptrNewMemBlock = (byte*) Allocate::New(bestBlockSize);
		Chunk* ptrNewChunks = (Chunk*) Allocate::New(neededChunks * sizeof(Chunk));
		if (ptrNewMemBlock == NULL) Allocate::OutOfMemory();
		if (ptrNewChunks == NULL) Allocate::OutOfMemory();
		kTotalSize += bestBlockSize;
		kFreeSize += bestBlockSize;
		kChunkCount += neededChunks;
		LinkChunksToData(ptrNewChunks, neededChunks, ptrNewMemBlock);
	}

	size_t MemoryPool::CalculateBestMemoryBlockSize(const size_t size){
		unsigned int needed = CalculateNeededChunks(size);
		return (size_t) (needed * kChunkSize);
	}

	unsigned int MemoryPool::CalculateNeededChunks(const size_t size){
		float f = (float) ((float) size / (float) kChunkSize);
		return ((unsigned int) ceil(f));
	}

	bool MemoryPool::LinkChunksToData(Chunk* ptrNewChunk, unsigned int chunkCount, byte* ptrNewMemBlock){
		Chunk* newChunk = NULL;
		unsigned int memoryOffset = 0;
		bool allocationChunkAssigned = false;
		for (unsigned int i = 0; i < chunkCount; i++){
			if (kFirstChunk == NULL){
				kFirstChunk = SetChunkDefaults(&ptrNewChunk[0]);
				kLastChunk = kFirstChunk;
				kCursorChunk = kFirstChunk;
			}
			else{
				newChunk = SetChunkDefaults(&ptrNewChunk[i]);
				kLastChunk->next = newChunk;
				kLastChunk = newChunk;
			}
			memoryOffset = (i * ((unsigned int) kChunkSize));
			kLastChunk->data = &(ptrNewMemBlock[memoryOffset]);

			if (!allocationChunkAssigned){
				kLastChunk->isAllocationChunk = true;
				allocationChunkAssigned = true;
			}
			if (i == chunkCount - 1) kLastChunk->isLastChunk = true;
		}
		return RecalcChunkMemorySize(kFirstChunk, kChunkCount);
	}

	Chunk* MemoryPool::SetChunkDefaults(Chunk* chunk){
		if (chunk != NULL){
			chunk->data = NULL;
	    chunk->size = 0;
	    chunk->used = 0;
			chunk->isAllocationChunk = false;
	    chunk->next = NULL;
	    chunk->isLastChunk = false;
	    kUnused.push_back(chunk);
		}
		return chunk;
	}

	bool MemoryPool::RecalcChunkMemorySize(Chunk* chunk, unsigned int size){
		unsigned int memoryOffset = 0;
		for (unsigned int i = 0; i < size; i++){
			if (chunk){
				memoryOffset = (i * ((unsigned int) kChunkSize));
				chunk->size = (((unsigned int) kTotalSize) - memoryOffset);
				chunk = chunk->next;
			}
			else{
				return false;
			}
		}
		return true;
	}

	size_t MemoryPool::MaxValue(const size_t a, const size_t b){
		return a > b ? a : b;
	}

	int MemoryPool::GetMemoryUsage(){
		return (int) kUsedSize;
	}

} // namespace internal
} // namespace Condor
