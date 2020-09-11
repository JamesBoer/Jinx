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
		static inline ReallocFn reallocFn = [](void * p, size_t size) { return realloc(p, size); };
		static inline FreeFn freeFn = [](void * p) { return free(p); };

		static inline std::atomic_uint64_t allocationCount = 0;
		static inline std::atomic_uint64_t freeCount = 0;
		static inline std::atomic_uint64_t allocatedMemory = 0;

		struct MemoryHeader
		{
			size_t allocSize;
		};
	} 

	inline_t void * MemAllocate(size_t bytes)
	{
		Impl::MemoryHeader * hdr = static_cast<Impl::MemoryHeader *>(Impl::allocFn(bytes + sizeof(Impl::MemoryHeader)));
		hdr->allocSize = bytes;
		Impl::allocationCount++;
		Impl::allocatedMemory += bytes;
		return reinterpret_cast<uint8_t *>(hdr) + sizeof(Impl::MemoryHeader);
	}

	inline_t void * MemReallocate(void * ptr, size_t bytes)
	{
		if (!ptr)
			return MemAllocate(bytes);
		if (!bytes)
		{
			MemFree(ptr);
			return nullptr;
		}
		Impl::MemoryHeader * hdr = reinterpret_cast<Impl::MemoryHeader *>(reinterpret_cast<uint8_t *>(ptr) - sizeof(Impl::MemoryHeader));
		size_t oldSize = hdr->allocSize;
		ptr = Impl::reallocFn(hdr, bytes + sizeof(Impl::MemoryHeader));
		Impl::allocatedMemory += (bytes - oldSize);
		return reinterpret_cast<uint8_t *>(ptr) + sizeof(Impl::MemoryHeader);
	}

	inline_t void MemFree(void * ptr)
	{
		if (!ptr)
			return;
		Impl::MemoryHeader * hdr = reinterpret_cast<Impl::MemoryHeader *>(reinterpret_cast<uint8_t *>(ptr) - sizeof(Impl::MemoryHeader));
		Impl::freeCount++;
		assert(Impl::allocatedMemory >= hdr->allocSize);
		Impl::allocatedMemory -= hdr->allocSize;
		Impl::freeFn(hdr);
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

