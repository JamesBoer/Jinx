/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

namespace Jinx
{
	// External allocation functions
	namespace Impl
	{
		static inline AllocFn allocFn = [](size_t size) { return malloc(size); };
		static inline ReallocFn reallocFn = [] (void * p, size_t s, size_t) { return realloc(p, s); };
		static inline FreeFn freeFn = [](void * p, size_t) { return free(p); };

		static inline std::atomic_uint64_t allocationCount = 0;
		static inline std::atomic_uint64_t freeCount = 0;
		static inline std::atomic_uint64_t allocatedMemory = 0;
	} 

	inline_t void * MemAllocate(size_t bytes)
	{
		Impl::allocationCount++;
		Impl::allocatedMemory += bytes;
		return reinterpret_cast<uint8_t *>(Impl::allocFn(bytes));
	}

	inline_t void * MemReallocate(void * ptr, size_t newBytes, size_t currBytes)
	{
		// With a size of zero, this acts like free()
		if (newBytes == 0)
		{
			MemFree(ptr, currBytes);
			return nullptr;
		}

		// If we have currently allocated memory, we track this as a free() as well as an alloc()
		if (ptr)
			Impl::freeCount++;

		// Normal realloc behaviorwith preserved data
		Impl::allocationCount++;
		Impl::allocatedMemory += (newBytes - currBytes);
		return reinterpret_cast<uint8_t *>(Impl::reallocFn(ptr, newBytes, currBytes));
	}

	inline_t void MemFree(void * ptr, size_t bytes)
	{
		if (!ptr)
			return;
		Impl::freeCount++;
		assert(Impl::allocatedMemory >= bytes);
		Impl::allocatedMemory -= bytes;
		Impl::freeFn(ptr, bytes);
	}

	inline_t void InitializeMemory(const GlobalParams & params)
	{
		if (params.allocFn || params.reallocFn || params.freeFn)
		{
			// If you're using one custom memory function, you must use them ALL
			assert(params.allocFn && params.reallocFn && params.freeFn);
			Impl::allocFn = params.allocFn;
			Impl::reallocFn = params.reallocFn;
			Impl::freeFn = params.freeFn;
		}
	}

	inline_t MemoryStats GetMemoryStats()
	{
		MemoryStats stats;
		stats.allocationCount = Impl::allocationCount;
		stats.freeCount = Impl::freeCount;
		stats.allocatedMemory = Impl::allocatedMemory;
		return stats;
	}

} // namespace Jinx

