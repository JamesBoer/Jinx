/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_MEMORY_H__
#define JX_MEMORY_H__

namespace Jinx
{
	// Stand-alone global allocation functions
	void * MemAllocate(size_t bytes);
	void * MemReallocate(void * ptr, size_t newBytes, size_t oldBytes);
	void MemFree(void * ptr, size_t bytes);

	// Jinx allocator for use in STL containers
	template <typename T>
	class Allocator
	{
	public:
		using value_type = T;

		Allocator() throw() {};
		Allocator(const Allocator &) throw() { }

		template<typename U>
		explicit Allocator(const Allocator<U>&) throw() { }

		T* allocate(size_t n) { return static_cast<T*> (Jinx::MemAllocate(n * sizeof(value_type))); }
		void deallocate(void* ptr, size_t n) { Jinx::MemFree(static_cast<T*> (ptr), n * sizeof(value_type)); }
	};

	template <typename T>
	bool operator == (const Allocator<T> &, const Allocator<T> &) { return true; }
	template <typename T>
	bool operator != (const Allocator<T> &, const Allocator<T> &) { return false; }


	// A fixed-size static memory arena used by StaticAllocator for fast, one-time allocations.
	// Based on Howard Hinnant's short_alloc, which is MIT licensed code.
	template <std::size_t N, std::size_t alignment = alignof(std::max_align_t)>
	class StaticArena
	{
		alignas(alignment) char m_buffer[N];
		char * m_ptr;

	public:
		~StaticArena() { m_ptr = nullptr; }
		StaticArena() noexcept : m_ptr(m_buffer) {}
		StaticArena(const StaticArena &) = delete;
		StaticArena & operator=(const StaticArena &) = delete;

		template <std::size_t ReqAlign> char * allocate(std::size_t n);
		void deallocate(char * p, std::size_t n) noexcept;

		static constexpr std::size_t size() noexcept { return N; }
		std::size_t used() const noexcept { return static_cast<std::size_t>(m_ptr - m_buffer); }
		void reset() noexcept { m_ptr = m_buffer; }

	private:
		static std::size_t align_up(std::size_t n) noexcept
		{
			return (n + (alignment - 1)) & ~(alignment - 1);
		}

		bool pointer_in_buffer(char * p) noexcept
		{
			return std::uintptr_t(m_buffer) <= std::uintptr_t(p) &&
				std::uintptr_t(p) <= std::uintptr_t(m_buffer) + N;
		}
	};

	template <std::size_t N, std::size_t alignment>
	template <std::size_t ReqAlign>
	char * StaticArena<N, alignment>::allocate(std::size_t n)
	{
		static_assert(ReqAlign <= alignment, "alignment is too small for this arena");
		assert(pointer_in_buffer(m_ptr) && "short_alloc has outlived arena");
		auto const aligned_n = align_up(n);
		if (static_cast<decltype(aligned_n)>(m_buffer + N - m_ptr) >= aligned_n)
		{
			char * r = m_ptr;
			m_ptr += aligned_n;
			return r;
		}

		static_assert(alignment <= alignof(std::max_align_t), "you've chosen an "
			"alignment that is larger than alignof(std::max_align_t), and "
			"cannot be guaranteed by normal operator new");
		return static_cast<char *>(Jinx::MemAllocate(n));
	}

	template <std::size_t N, std::size_t alignment>
	void StaticArena<N, alignment>::deallocate(char * p, std::size_t n) noexcept
	{
		assert(pointer_in_buffer(m_ptr) && "short_alloc has outlived arena");
		if (pointer_in_buffer(p))
		{
			n = align_up(n);
			if (p + n == m_ptr)
				m_ptr = p;
		}
		else
			Jinx::MemFree(p, n);
	}

	template <class T, std::size_t N, std::size_t Align = alignof(std::max_align_t)>
	class StaticAllocator
	{
	public:
		using value_type = T;
		static auto constexpr alignment = Align;
		static auto constexpr size = N;
		using arena_type = StaticArena<size, alignment>;

	private:
		arena_type & m_arena;

	public:
		StaticAllocator(const StaticAllocator &) = default;
		StaticAllocator & operator=(const StaticAllocator &) = delete;

		StaticAllocator(arena_type & a) noexcept : m_arena(a)
		{
			static_assert(size % alignment == 0,
				"size N needs to be a multiple of alignment Align");
		}
		template <class U>
		StaticAllocator(const StaticAllocator<U, N, alignment> & a) noexcept
			: m_arena(a.m_arena)
		{
		}

		template <class _Up> struct rebind { using other = StaticAllocator<_Up, N, alignment>; };

		T * allocate(std::size_t n)
		{
			return reinterpret_cast<T *>(m_arena.template allocate<alignof(T)>(n * sizeof(T)));
		}
		void deallocate(T * p, std::size_t n) noexcept
		{
			m_arena.deallocate(reinterpret_cast<char *>(p), n * sizeof(T));
		}

		template <class T1, std::size_t N1, std::size_t A1, class U, std::size_t M, std::size_t A2>
		friend bool operator == (const StaticAllocator<T1, N1, A1> & x, const StaticAllocator<U, M, A2> & y) noexcept;

		template <class U, std::size_t M, std::size_t A> friend class StaticAllocator;
	};

	template <class T, std::size_t N, std::size_t A1, class U, std::size_t M, std::size_t A2>
	inline bool operator == (const StaticAllocator<T, N, A1> & x, const StaticAllocator<U, M, A2> & y) noexcept
	{
		return N == M && A1 == A2 && &x.m_arena == &y.m_arena;
	}

	template <class T, std::size_t N, std::size_t A1, class U, std::size_t M, std::size_t A2>
	inline bool operator != (const StaticAllocator<T, N, A1> & x, const StaticAllocator<U, M, A2> & y) noexcept
	{
		return !(x == y);
	}


	struct GlobalParams;
	void InitializeMemory(const GlobalParams & params);

	struct MemoryStats
	{
		uint64_t allocationCount = 0;
		uint64_t freeCount = 0;
		uint64_t allocatedMemory = 0;
	};

	MemoryStats GetMemoryStats();
}

#endif // JX_MEMORY_H__
