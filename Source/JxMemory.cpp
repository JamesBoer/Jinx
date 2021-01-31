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
		if (params.allocFn || params.freeFn)
		{
			// If you're using one custom memory function, you must use them ALL
			assert(params.allocFn && params.freeFn);
			Impl::allocFn = params.allocFn;
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

