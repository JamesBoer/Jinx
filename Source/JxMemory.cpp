/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

using namespace Jinx;
using namespace std;

// Uncomment to skip the custom allocation and use plain old malloc/realloc/free for
// every allocation.  This might be useful for profiling tests or debugging
// suspected issues with the allocator.  Note that if this is defined, no
// customized allocator functions will be used at all.
//#define JINX_DEBUG_USE_STD_ALLOC

// An application can disable the pool allocator using this define.  Custom
// allocators will still be used, and if those are not provided, the library
// will fall back to the standard library allocators malloc/realloc/free.
//#define JINX_DISABLE_POOL_ALLOCATOR

// Overload new and delete with allocation to default pool.  Useful for tests to
// check if anything is using new/delete under the hood.
//#define JINX_OVERRIDE_NEW_AND_DELETE

// Whenever we are using debug allocation mode, also use memory guards.  However,
// we can also enable it independently if desired.  The memory guards put a protective
// band around each allocation in an attempt to detect memory overwrites.
#ifdef JINX_DEBUG_ALLOCATION
#define JINX_USE_MEMORY_GUARDS
#endif

// Global new and delete overloads
#ifdef JINX_OVERRIDE_NEW_AND_DELETE

void * operator new (size_t n)
{
#ifdef JINX_DEBUG_USE_STD_ALLOC
	return malloc(n);
#else
	return Jinx::JinxAlloc(n);
#endif
}

void * operator new [](size_t n)
{
#ifdef JINX_DEBUG_USE_STD_ALLOC
	return malloc(n);
#else
	return Jinx::JinxAlloc(n);
#endif	
}

void operator delete(void * p) throw()
{
#ifdef JINX_DEBUG_USE_STD_ALLOC
	free(p);
#else
	Jinx::JinxFree(p);
#endif
}

void operator delete[](void * p) throw()
{
#ifdef JINX_DEBUG_USE_STD_ALLOC
	free(p);
#else
	Jinx::JinxFree(p);
#endif	
}

#endif // JINX_OVERRIDE_NEW_AND_DELETE
// End new and delete overloads


namespace Jinx
{

	// External allocation functions
	AllocFn s_allocFn = [](size_t size) { return malloc(size); };
	ReallocFn s_reallocFn = [](void * p, size_t size) { return realloc(p, size); };
	FreeFn s_freeFn = [](void * p) { return free(p); };

#ifndef JINX_DISABLE_POOL_ALLOCATOR

#ifdef JINX_USE_MEMORY_GUARDS
	static const uint8_t MEMORY_GUARD_PATTERN = 0xA8;
	static const size_t MEMORY_GUARD_SIZE = 16;
#endif

	class BlockHeap;
	struct MemoryHeader;


	// This structure is placed at the beginning of each large system allocated block
	// of memory.  The memory pool makes smaller allocations out of the memory block on 
	// demand, and can also free allocations by simply reducing the count.  When the count 
	// reaches zero, we know that all allocations have been freed, and can either reclaim 
	// or reuse this memory.
	struct MemoryBlock
	{
#ifdef JINX_USE_MEMORY_GUARDS
		uint8_t memGuardHead[MEMORY_GUARD_SIZE];
#endif	
		uint8_t * data;
		size_t usedBytes;
		size_t allocatedBytes;
		size_t capacity;
		size_t count;
		MemoryBlock * prev;
		MemoryBlock * next;
#ifdef JINX_DEBUG_ALLOCATION
		MemoryHeader * head;
		MemoryHeader * tail;
#endif
#ifdef JINX_USE_MEMORY_GUARDS
		uint8_t memGuardTail[MEMORY_GUARD_SIZE];
#endif
	};

	// Run-time header for all memory allocations.  This header is placed
	// in front of the memory address that's returned from the allocation functions.
	struct MemoryHeader
	{
#ifdef JINX_USE_MEMORY_GUARDS
		uint8_t memGuardHead[MEMORY_GUARD_SIZE];
#endif
		BlockHeap * heap;
		MemoryBlock * memBlock;
		size_t bytes;
		size_t padding;
#ifdef JINX_DEBUG_ALLOCATION
		// Allows us to walk through all allocations in a block.
		MemoryHeader * prev;
		MemoryHeader * next;
		// Data to help track allocations
		const char * file;
		const char * function;
		int64_t line; // Note: this is a 64-bit value to assist with padding on 32-bit platforms
#endif
#ifdef JINX_USE_MEMORY_GUARDS
		uint8_t memGuardTail[MEMORY_GUARD_SIZE];
#endif
	};

	// Make sure our MemoryHeader struct is properly padded
	static_assert((sizeof(MemoryHeader) % std::alignment_of<max_align_t>::value) == 0, "MemoryHeader must be padded to proper allocation alignment");

	class BlockHeap
	{
	public:
		BlockHeap();
		~BlockHeap();

		void * Alloc(size_t bytes);
		void * Realloc(void * ptr, size_t bytes);
		void Free(void * ptr);
		void Free(MemoryHeader * header);
		void LogAllocations();
		inline BlockHeap * Next() const { return m_next; }
		void ShutDown();

	private:
		MemoryBlock * AllocBlock(size_t bytes);
		void FreeInternal(MemoryHeader * header);
		void FreeBlock(MemoryBlock * block);

	private:
		Mutex m_mutex;
		BlockHeap * m_prev;
		BlockHeap * m_next;
		MemoryBlock * m_allocHead;
		MemoryBlock * m_allocTail;
		MemoryBlock * m_spareHead;
		MemoryBlock * m_spareTail;
		size_t m_allocSpareBlocks;
	};

#else // JINX_DISABLE_POOL_ALLOCATOR

	class DefaultHeap
	{
	public:
		void * Alloc(size_t bytes) { return s_allocFn(bytes); }
		void * Realloc(void * ptr, size_t bytes) { return s_reallocFn(ptr, bytes); }
		void Free(void * ptr) { s_freeFn(ptr); }
		const MemoryStats & GetMemoryStats() const { return m_stats; }
		void LogAllocations() {}
		void ShutDown() {}

	private:
		AllocFn m_allocFn = [](size_t size) { return malloc(size); };
		ReallocFn m_reallocFn = [](void * p, size_t size) { return realloc(p, size); };
		FreeFn m_freeFn = [](void * p) { return free(p); };
		MemoryStats m_stats;
	};

#endif // #ifndef JINX_DISABLE_POOL_ALLOCATOR

#ifndef JINX_DISABLE_POOL_ALLOCATOR

#ifdef JINX_USE_MEMORY_GUARDS
	static uint8_t s_memoryGuardCheck[MEMORY_GUARD_SIZE] =
	{
		MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN,
		MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN,
		MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN,
		MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN,
	};
	static_assert(countof(s_memoryGuardCheck) == (size_t)MEMORY_GUARD_SIZE, "Memory guard array size mismatch");

#endif // JINX_USE_MEMORY_GUARDS

	// Each thread has its own local heap.  This helps to avoid thread contention
	// when scripts are executed on parallel threads.  Note that reallocs and frees
	// are allowed to occur from different threads (preventing this would be unweildy), 
	// so locks are still needed to ensure thread-safety inside the heap itself.
	static thread_local BlockHeap	s_heap;

	// List of all thread-local heaps
	static Mutex s_mutex;
	static BlockHeap * s_head;
	static BlockHeap * s_tail;

	// Allocation parameters
	static size_t s_allocBlockSize = (1024 * 32) - sizeof(MemoryBlock);
	static size_t s_maxAllocSpareBlocks = 2;

	// Track global memory statistics
	static std::atomic<uint64_t> s_externalAllocCount;
	static std::atomic<uint64_t> s_externalFreeCount;
	static std::atomic<uint64_t> s_internalAllocCount;
	static std::atomic<uint64_t> s_internalFreeCount;
	static std::atomic<uint64_t> s_currentBlockCount;
	static std::atomic<uint64_t> s_currentAllocatedMemory;
	static std::atomic<uint64_t> s_currentUsedMemory;


#else // JINX_DISABLE_POOL_ALLOCATOR

	static DefaultHeap s_heap;

#endif // JINX_DISABLE_POOL_ALLOCATOR

} // namespace Jinx


using namespace Jinx;

#ifndef JINX_DISABLE_POOL_ALLOCATOR

BlockHeap::BlockHeap() :
	m_allocHead(nullptr),
	m_allocTail(nullptr),
	m_spareHead(nullptr),
	m_spareTail(nullptr),
	m_next(nullptr),
	m_allocSpareBlocks(0)
{

	// Add block heap to the global list
	if (s_head == nullptr)
	{
		assert(s_tail == nullptr);
		s_head = this;
		m_prev = nullptr;
		s_tail = this;
	}
	else
	{
		// Ensure thread-safe access to the global heap list
		std::lock_guard<Mutex> lock(s_mutex);
		assert(s_tail);
		m_prev = s_tail;
		s_tail->m_next = this;
		s_tail = this;
	}
}

BlockHeap::~BlockHeap()
{
	ShutDown();
	assert(m_allocHead == nullptr && m_allocTail == nullptr);
	assert(m_spareHead == nullptr && m_spareTail == nullptr);
}

void * BlockHeap::Alloc(size_t bytes)
{
	if (!bytes)
		return nullptr;

	// Ensure thread-safe access to the allocator
	std::lock_guard<Mutex> lock(m_mutex);

	// The required allocation size is request size plus the size of the memory header
	size_t requestedBytes = bytes + sizeof(MemoryHeader);

	// Make sure allocations are always property aligned to the maximum 
	requestedBytes = NextHighestMultiple(requestedBytes, std::alignment_of<max_align_t>::value);

	// Make sure we have a valid memory block for the requested size
	if (!m_allocHead)
	{
		m_allocHead = AllocBlock(requestedBytes);
		m_allocTail = m_allocHead;
	}
	else
	{
		if (requestedBytes > (m_allocTail->capacity - m_allocTail->allocatedBytes))
		{
			MemoryBlock * newBlock = AllocBlock(requestedBytes);
			m_allocTail->next = newBlock;
			newBlock->prev = m_allocTail;
			m_allocTail = newBlock;
		}
	}

	// Return a memory pointer
	void * ptr = m_allocTail->data + m_allocTail->allocatedBytes;
	m_allocTail->allocatedBytes += requestedBytes;
	m_allocTail->usedBytes += requestedBytes;
	m_allocTail->count++;

	// Fill out memory header with required information
	MemoryHeader * header = static_cast<MemoryHeader *>(ptr);
	header->heap = this;
	header->memBlock = m_allocTail;
	header->bytes = requestedBytes;

#ifdef JINX_DEBUG_ALLOCATION

	header->file = nullptr;
	header->function = nullptr;
	header->line = 0;

	// Insert the memory header into the block's linked list for debug tracking
	if (header->memBlock->head == nullptr)
	{
		assert(header->memBlock->tail == nullptr);
		header->memBlock->head = header;
		header->prev = nullptr;
	}
	else
	{
		assert(header->memBlock->tail);
		header->prev = header->memBlock->tail;
		header->memBlock->tail->next = header;
	}
	header->memBlock->tail = header;
	header->next = nullptr;

#endif

#ifdef JINX_USE_MEMORY_GUARDS
	memset(header->memGuardHead, MEMORY_GUARD_PATTERN, MEMORY_GUARD_SIZE);
	memset(header->memGuardTail, MEMORY_GUARD_PATTERN, MEMORY_GUARD_SIZE);
#endif 

	// Update memory stats
	s_currentUsedMemory += requestedBytes;
	s_internalAllocCount++;

	// Return the allocated pointer advanced by the size of the memory header.  We'll
	// reverse the process when freeing the memory.
	return static_cast<char*>(ptr) + sizeof(MemoryHeader);
}

MemoryBlock * BlockHeap::AllocBlock(size_t bytes)
{
	size_t blockSize = s_allocBlockSize;
	if (bytes > blockSize)
		blockSize = NextHighestMultiple(bytes, std::alignment_of<max_align_t>::value);

	MemoryBlock * newBlock = nullptr;

	// Check the sparelist first
	MemoryBlock * curr = m_spareHead;
	while (curr)
	{
		if (curr->capacity >= bytes)
		{
			if (curr == m_spareHead)
				m_spareHead = curr->next;
			else
				curr->prev->next = curr->next;
			if (curr == m_spareTail)
				m_spareTail = curr->prev;
			else
				curr->next->prev = curr->prev;
			m_allocSpareBlocks--;
			newBlock = curr;
			break;
		}
		curr = curr->next;
	}

	// If none is available, allocate a block
	if (!newBlock)
	{
		newBlock = static_cast<MemoryBlock *>(s_allocFn(blockSize + sizeof(MemoryBlock)));
		newBlock->capacity = blockSize;
		newBlock->data = reinterpret_cast<uint8_t *>(newBlock) + sizeof(MemoryBlock);
		s_currentAllocatedMemory += (blockSize + sizeof(MemoryBlock));
		s_externalAllocCount++;
		s_currentBlockCount++;
	}
	newBlock->allocatedBytes = 0;
	newBlock->usedBytes = 0;
	newBlock->count = 0;
	newBlock->prev = nullptr;
	newBlock->next = nullptr;
#ifdef JINX_DEBUG_ALLOCATION
	newBlock->head = nullptr;
	newBlock->tail = nullptr;
#endif
#ifdef JINX_USE_MEMORY_GUARDS
	memset(newBlock->memGuardHead, MEMORY_GUARD_PATTERN, MEMORY_GUARD_SIZE);
	memset(newBlock->memGuardTail, MEMORY_GUARD_PATTERN, MEMORY_GUARD_SIZE);
#endif 
	return newBlock;
}

void BlockHeap::Free(void * ptr)
{
	// Ensure thread-safe access to the allocated blocks
	std::lock_guard<Mutex> lock(m_mutex);

	// Retrieve the memory header from the raw pointer
	MemoryHeader * header = reinterpret_cast<MemoryHeader*>(static_cast<char *>(ptr) - sizeof(MemoryHeader));

	// Free the memory from the allocated block
	FreeInternal(header);
}

void BlockHeap::Free(MemoryHeader * header)
{
	// Ensure thread-safe access to the allocated blocks
	std::lock_guard<Mutex> lock(m_mutex);

	// Free the memory from the allocated block
	FreeInternal(header);
}

void BlockHeap::FreeInternal(MemoryHeader * header)
{
	// Retrieve the memory block from the header
	MemoryBlock * memBlock = header->memBlock;

#ifdef JINX_USE_MEMORY_GUARDS
	assert(memcmp(header->memGuardHead, s_memoryGuardCheck, MEMORY_GUARD_SIZE) == 0);
	assert(memcmp(header->memGuardTail, s_memoryGuardCheck, MEMORY_GUARD_SIZE) == 0);
#endif 

	// Assert we're not freeing more than we've allocated
	assert(memBlock->usedBytes >= header->bytes);
	assert(memBlock->count > 0);

	// Subtract memory buffer size
	memBlock->usedBytes -= header->bytes;

	// Subtract an allocation count
	memBlock->count--;

	// Update memory stats
	s_currentUsedMemory -= header->bytes;
	s_internalFreeCount++;

#ifdef JINX_DEBUG_ALLOCATION

	// Update the memory blocks head or tail pointers if required
	if (memBlock->head == header)
		memBlock->head = header->next;
	if (memBlock->tail == header)
		memBlock->tail = header->prev;

	// Remove the header from the memory block's list of allocations by
	// linking the previous and next nodes, removing the current node
	// from the chain of links.
	if (header->prev)
		header->prev->next = header->next;
	if (header->next)
		header->next->prev = header->prev;

#endif

	// If the allocation count reaches zero, then we can free
	// the block.
	if (memBlock->count == 0)
		FreeBlock(memBlock);
}

void BlockHeap::FreeBlock(MemoryBlock * block)
{
	assert(block);
	assert(block->data);
	assert(block->count == 0);

#ifdef JINX_USE_MEMORY_GUARDS
	assert(memcmp(block->memGuardHead, s_memoryGuardCheck, MEMORY_GUARD_SIZE) == 0);
	assert(memcmp(block->memGuardTail, s_memoryGuardCheck, MEMORY_GUARD_SIZE) == 0);
#endif 

	// Remove the block from the double linked-list.
	if (block == m_allocHead)
		m_allocHead = block->next;
	else
		block->prev->next = block->next;
	if (block == m_allocTail)
		m_allocTail = block->prev;
	else
		block->next->prev = block->prev;

	// Check first to see if we can put this on the spare list
	if (m_allocSpareBlocks < s_maxAllocSpareBlocks)
	{
		block->allocatedBytes = 0;
		block->usedBytes = 0;
		block->count = 0;
		if (!m_spareTail)
		{
			m_spareHead = block;
			block->next = nullptr;
			block->prev = nullptr;
		}
		else
		{
			block->next = nullptr;
			block->prev = m_spareTail;
			m_spareTail->next = block;
		}
		m_spareTail = block;
		m_allocSpareBlocks++;
	}
	else
	{
		// Track allocation stats
		s_externalFreeCount++;
		s_currentAllocatedMemory -= (block->capacity + sizeof(MemoryBlock));
		s_currentBlockCount--;

		// Free the block of memory
		s_freeFn(block);
	}
}

void BlockHeap::LogAllocations()
{
	LogWriteLine("=== Memory Log Begin ===");

	// Log all memory blocks
	auto memBlock = m_allocHead;
	while (memBlock)
	{
		LogWriteLine("");
		LogWriteLine("--- Memory Block ---");
#ifdef JINX_USE_MEMORY_GUARDS
		bool memGuardsIntact = (memcmp(memBlock->memGuardHead, s_memoryGuardCheck, MEMORY_GUARD_SIZE) == 0) &&
			(memcmp(memBlock->memGuardTail, s_memoryGuardCheck, MEMORY_GUARD_SIZE) == 0) ? true : false;
		LogWriteLine("Memory guards intact: %s", memGuardsIntact ? "true" : "false");
#endif 		
		LogWriteLine("Data = %p", memBlock->data);
		LogWriteLine("Used bytes = %" PRIuPTR, memBlock->usedBytes);
		LogWriteLine("Allocated bytes = %" PRIuPTR, memBlock->allocatedBytes);
		LogWriteLine("Capacity = %" PRIuPTR, memBlock->capacity);
		LogWriteLine("Count = %" PRIuPTR, memBlock->count);
#ifdef JINX_DEBUG_ALLOCATION
		LogWriteLine("Memory allocations:");
		auto memHeader = memBlock->head;
		while (memHeader)
		{
			LogWriteLine("Allocation %p, size: %"  PRIuPTR ", File: %s, Function: %s", 
				memHeader->memBlock, memHeader->bytes, memHeader->file ? memHeader->file : "", memHeader->function ? memHeader->function : "");
			memHeader = memHeader->next;
		}
#endif
		memBlock = memBlock->next;
	}
	LogWriteLine("");

	// Log memory stats
	auto memStats = GetMemoryStats();
	LogWriteLine("--- Memory Stats ---");
	LogWriteLine("External alloc count:     %i", memStats.externalAllocCount);
	LogWriteLine("External free count:      %i", memStats.externalFreeCount);
	LogWriteLine("Internal alloc count:     %i", memStats.internalAllocCount);
	LogWriteLine("Internal free count:      %i", memStats.internalFreeCount);
	LogWriteLine("Current block count:      %i", memStats.currentBlockCount);
	LogWriteLine("Current allocated memory: %lli", memStats.currentAllocatedMemory);
	LogWriteLine("Current used memory:      %lli", memStats.currentAllocatedMemory);
	LogWriteLine("");
	LogWriteLine("=== Memory Log End ===");

}

void * BlockHeap::Realloc(void * ptr, size_t bytes)
{

	// Realloc acts like Alloc if ptr is null
	if (ptr == nullptr)
		return Alloc(bytes);

	// If bytes are zero, Realloc acts like Free
	if (bytes == 0)
	{
		Free(ptr);
		return nullptr;
	}

	// Retrieve the memory header from the raw pointer
	MemoryHeader * header = reinterpret_cast<MemoryHeader*>(static_cast<char *>(ptr) - sizeof(MemoryHeader));

	// If we're shrinking the allocation, simply do nothing and return the same pointer
	if (bytes < header->bytes - sizeof(MemoryHeader))
		return ptr;

	// It doesn't make a lot of sense to try to perform a true realloc with this allocation scheme, as the 
	// chances of being able to do so with a batch/pool allocator like we're using would be slim at best.

	// Alloc a new buffer
	void * p = Alloc(bytes);

	// Copy the old content to the new buffer
	memcpy(p, ptr, header->bytes - sizeof(MemoryHeader));

	// Free the old memory
	Free(header);

	// Return the new memory buffer
	return p;
}

void BlockHeap::ShutDown()
{
	// Free memory from allocation list
	MemoryBlock * curr = m_allocHead;
	MemoryBlock * next;
	while (curr)
	{
		next = curr->next;
		if (curr->usedBytes == 0)
		{
			s_externalFreeCount++;
			s_currentAllocatedMemory -= (curr->capacity + sizeof(MemoryBlock));
			s_freeFn(curr);
		}
		else
		{
			LogWriteLine("Could not free block at shutdown.  Memory still in use.");
		}
		curr = next;
	}
	m_allocHead = nullptr;
	m_allocTail = nullptr;

	// Free memory from spare list
	curr = m_spareHead;
	while (curr)
	{
		next = curr->next;
		if (curr->usedBytes == 0)
		{
			s_externalFreeCount++;
			s_currentAllocatedMemory -= (curr->capacity + sizeof(MemoryBlock));
			s_freeFn(curr);
		}
		else
		{
			LogWriteLine("Could not free block at shutdown.  Memory still in use.");
		}
		curr = next;
	}
	m_spareHead = nullptr;
	m_spareTail = nullptr;

	// Block heap has already been removed from the global list
	if (m_prev == nullptr && m_next == nullptr)
		return;

	// Ensure thread-safe access to the global heap list
	std::lock_guard<Mutex> lock(s_mutex);

	// Remove block heap from global heap list
	if (this == s_head)
		s_head = m_next;
	else
		m_prev->m_next = m_next;
	if (this == s_tail)
		s_tail = m_prev;
	else
		m_next->m_prev = m_prev;
	m_prev = nullptr;
	m_next = nullptr;
}

#endif // JINX_DISABLE_POOL_ALLOCATOR

#ifdef JINX_DEBUG_ALLOCATION

void * Jinx::MemPoolAllocate(const char * file, const char * function, uint32_t line, size_t bytes)
{
	void * p;
#if defined(JINX_DEBUG_USE_STD_ALLOC)
	Jinx::ref(file);
	Jinx::ref(function);
	Jinx::ref(line);
	p = malloc(bytes);
#elif defined(JINX_DISABLE_POOL_ALLOCATOR)
	Jinx::ref(file);
	Jinx::ref(function);
	Jinx::ref(line);
	p = s_heap.Alloc(bytes);
#else
	p = s_heap.Alloc(bytes);
	MemoryHeader * header = reinterpret_cast<MemoryHeader*>(static_cast<char *>(p) - sizeof(MemoryHeader));
	header->file = file;
	header->function = function;
	header->line = line;
#endif
	return p;
}

void * Jinx::MemPoolReallocate(const char * file, const char * function, uint32_t line, void * ptr, size_t bytes)
{
	void * p;
#ifdef JINX_DEBUG_USE_STD_ALLOC
	Jinx::ref(file);
	Jinx::ref(function);
	Jinx::ref(line);
	// The CRT debug library has a bug that prevents it from propertly detecting
	// memory freed with realloc.
#ifdef LAIR_DEBUG_ENABLE_STD_REALLOC_LEAK_FIX
	if (bytes == 0)
	{
		free(ptr);
		return nullptr;
	}
#endif
	p = realloc(ptr, bytes);
#elif defined(JINX_DISABLE_POOL_ALLOCATOR)
	Jinx::ref(file);
	Jinx::ref(function);
	Jinx::ref(line);
	p = s_heap.Realloc(ptr, bytes);
#else
	if (ptr)
	{
		MemoryHeader * header = reinterpret_cast<MemoryHeader*>(static_cast<char *>(ptr) - sizeof(MemoryHeader));
		p = header->heap->Realloc(ptr, bytes);
	}
	else
	{
		p = s_heap.Realloc(ptr, bytes);
	}
	if (p)
	{
		MemoryHeader * header = reinterpret_cast<MemoryHeader*>(static_cast<char *>(p) - sizeof(MemoryHeader));
		header->file = file;
		header->function = function;
		header->line = line;
	}
#endif
	return p;
}

#else

void * Jinx::MemPoolAllocate(size_t bytes)
{
	void * p;
#ifdef JINX_DEBUG_USE_STD_ALLOC
	p = malloc(bytes);
#else
	p = s_heap.Alloc(bytes);
#endif
	return p;
}

void * Jinx::MemPoolReallocate(void * ptr, size_t bytes)
{
	void * p;
#ifdef JINX_DEBUG_USE_STD_ALLOC
	p = realloc(ptr, bytes);
#elif defined(JINX_DISABLE_POOL_ALLOCATOR)
	p = s_heap.Realloc(ptr, bytes);
#else
	if (ptr)
	{
		MemoryHeader * header = reinterpret_cast<MemoryHeader*>(static_cast<char *>(ptr) - sizeof(MemoryHeader));
		p = header->heap->Realloc(ptr, bytes);
	}
	else
	{
		p = s_heap.Realloc(ptr, bytes);
	}
#endif
	return p;
}

#endif

void Jinx::MemPoolFree(void * ptr)
{
	if (!ptr)
		return;
#ifdef JINX_DEBUG_USE_STD_ALLOC
	free(ptr);
#elif defined(JINX_DISABLE_POOL_ALLOCATOR)
	s_heap.Free(ptr);
#else
	MemoryHeader * header = reinterpret_cast<MemoryHeader*>(static_cast<char *>(ptr) - sizeof(MemoryHeader));
	header->heap->Free(ptr);
#endif
}

void Jinx::InitializeMemory(const GlobalParams & params)
{
	if (params.allocFn || params.reallocFn || params.freeFn)
	{
		// If you're using one custom memory function, you must use them ALL
		assert(params.allocFn && params.reallocFn && params.freeFn);
		s_allocFn = params.allocFn;
		s_reallocFn = params.reallocFn;
		s_freeFn = params.freeFn;
	}

#ifndef JINX_DISABLE_POOL_ALLOCATOR
	// Alloc block size must be at least 4K (otherwise, what's the point of a block allocator?)
	assert(params.allocBlockSize >= (1024 * 4));
	s_allocBlockSize = params.allocBlockSize - sizeof(MemoryBlock);
	s_maxAllocSpareBlocks = params.allocSpareBlocks;
#endif

}

void Jinx::ShutDownMemory()
{
#ifndef JINX_DEBUG_USE_STD_ALLOC
	std::lock_guard<Mutex> lock(s_mutex);
	BlockHeap * heap = s_head;
	while (heap)
	{
		heap->ShutDown();
		heap = heap->Next();
	}
#endif
}

MemoryStats Jinx::GetMemoryStats()
{
	MemoryStats stats;
#ifndef JINX_DISABLE_POOL_ALLOCATOR
	stats.externalAllocCount = s_externalAllocCount;
	stats.externalFreeCount = s_externalFreeCount;
	stats.internalAllocCount = s_internalAllocCount;
	stats.internalFreeCount = s_internalFreeCount;
	stats.currentBlockCount = s_currentBlockCount;
	stats.currentAllocatedMemory = s_currentAllocatedMemory;
	stats.currentUsedMemory = s_currentUsedMemory;
#endif
	return stats;
}

void Jinx::LogAllocations()
{
#ifndef JINX_DEBUG_USE_STD_ALLOC
	std::lock_guard<Mutex> lock(s_mutex);
	BlockHeap * heap = s_head;
	while (heap)
	{
		heap->LogAllocations();
		heap = heap->Next();
	}
#endif
}

