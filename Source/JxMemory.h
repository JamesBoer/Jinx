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
	void * MemReallocate(void * ptr, size_t bytes);
	void MemFree(void * ptr);

	// Jinx allocator for use in STL containers
	template <typename T>
	class Allocator
	{
	public:
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using value_type = T;

		Allocator() throw() {};
		Allocator(const Allocator &) throw() { }

		template<typename U>
		explicit Allocator(const Allocator<U>&) throw() { }

		template<typename U>
		Allocator & operator = ([[maybe_unused]] const Allocator<U> & other) { other; return *this; }
		Allocator & operator = ([[maybe_unused]] const Allocator & other) { other; return *this; }
		~Allocator() {}

		pointer address(reference value) const { return &value; }
		const_pointer address(const_reference value) const { return &value; }

		pointer allocate(size_type n) { return static_cast<pointer> (Jinx::MemAllocate(n * sizeof(value_type))); }
		pointer allocate(size_type n, const void *) { return static_cast<pointer> (Jinx::MemAllocate(n * sizeof(value_type))); }
		void deallocate(void* ptr, size_type) { Jinx::MemFree(static_cast<T*> (ptr)); }

		template<typename U, typename... Args>
		void construct(U* ptr, Args&&  ... args) { ::new ((void*)(ptr)) U(std::forward<Args>(args)...); }
		void construct(pointer ptr, const T& val) { new (static_cast<T*> (ptr)) T(val); }

		template<typename U>
		void destroy([[maybe_unused]] U* ptr) { ptr->~U(); }
		void destroy([[maybe_unused]] pointer ptr) { ptr->~T(); }

		size_type max_size() const { return std::numeric_limits<std::size_t>::max() / sizeof(T); }
	};

	template <typename T>
	bool operator == (const Allocator<T> &, const Allocator<T> &) { return true; }
	template <typename T>
	bool operator != (const Allocator<T> &, const Allocator<T> &) { return false; }

	struct GlobalParams;
	void InitializeMemory(const GlobalParams & params);

	struct MemoryStats
	{
		uint64_t allocationCount = 0;
		uint64_t freeCount = 0;
		uint64_t allocatedMemory = 0;
	};

	MemoryStats GetMemoryStats();

	// Define a custom UTF-8 string using internal allocator
	using String = std::basic_string <char, std::char_traits<char>, Allocator<char>>;

	// Define a custom UTF-16 string using internal allocator
	using StringU16 = std::basic_string <char16_t, std::char_traits<char16_t>, Allocator<char16_t>>;

	// Define a custom wide character string using internal allocator
	using WString = std::basic_string <wchar_t, std::char_traits<wchar_t>, Allocator<wchar_t>>;
}

#endif // JX_MEMORY_H__
