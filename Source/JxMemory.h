/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_MEMORY_H__
#define JX_MEMORY_H__


#ifdef _DEBUG
#define JINX_DEBUG_ALLOCATION
#endif // _DEBUG

#define JINX_ALLOC_REBIND_NOT_USED

#ifdef JINX_DEBUG_ALLOCATION
#define JinxAlloc(bytes) MemPoolAllocate(__FILE__, __FUNCTION__, __LINE__, bytes)
#define JinxRealloc(ptr, bytes) MemPoolReallocate(__FILE__, __FUNCTION__, __LINE__, ptr, bytes)
#define JinxNew(objType, ...) MemPoolAllocateObject<objType>(__FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#else
#define JinxAlloc(bytes)MemPoolAllocate(bytes)
#define JinxRealloc(ptr, bytes) MemPoolReallocate(ptr, bytes)
#define JinxNew(objType, ...) MemPoolAllocateObject<objType>(__VA_ARGS__)
#endif

#define JinxFree MemPoolFree
#define JinxDelete MemPoolFreeObject

namespace Jinx
{

	// Stand-alone global allocation functions (debug and release version)

#ifdef JINX_DEBUG_ALLOCATION

	void * MemPoolAllocate(const char * file, const char * function, uint32_t line, size_t bytes);

	void * MemPoolReallocate(const char * file, const char * function, uint32_t line, void * ptr, size_t bytes);

	template<typename T>
	T * MemPoolAllocateType(const char * file, const char * function, uint32_t line)
	{
		return (T *)MemPoolAllocate(file, function, line, sizeof(T));
	}

	template<typename T, typename... Args>
	T * MemPoolAllocateObject(const char * file, const char * function, uint32_t line, Args&&... args)
	{
		T * obj = MemPoolAllocateType<T>(file, function, line);
		new(obj)T(std::forward<Args>(args)...);
		return obj;
	}

#else

	void * MemPoolAllocate(size_t bytes);

	void * MemPoolReallocate(void * ptr, size_t bytes);

	template<typename T>
	T * MemPoolAllocateType()
	{
		return (T *)MemPoolAllocate(sizeof(T));
	}

	template<typename T>
	T * MemPoolAllocateObject()
	{
		T * obj = MemPoolAllocateType<T>();
		new(obj)T();
		return obj;
	}

	template<typename T, typename ... Args>
	T * MemPoolAllocateObject(Args&&... args)
	{
		T * obj = MemPoolAllocateType<T>();
		new(obj)T(std::forward<Args>(args)...);
		return obj;
	}

#endif

	void MemPoolFree(void * ptr);

	template<typename T>
	void MemPoolFreeObject(T * obj)
	{
		if (!obj)
			return;
		obj->~T();
		MemPoolFree(obj);
	}

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

#ifndef JINX_ALLOC_REBIND_NOT_USED
		template <typename U>
		struct rebind { using other = Allocator<U>; }
#endif // JINX_ALLOC_REBIND_NOT_USED

		pointer address(reference value) const { return &value; }
		const_pointer address(const_reference value) const { return &value; }

		pointer allocate(size_type n) { return static_cast<pointer> (JinxAlloc(n * sizeof(value_type))); }
		pointer allocate(size_type n, const void *) { return static_cast<pointer> (JinxAlloc(n * sizeof(value_type))); }
		void deallocate(void* ptr, size_type) { JinxFree(static_cast<T*> (ptr)); }

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

	template <typename T>
	class Deleter
	{
	public:
		void operator()(T * p) { JinxDelete<T>(p); }
	};

	struct MemoryStats
	{
		MemoryStats() :
			externalAllocCount(0),
			externalFreeCount(0),
			internalAllocCount(0),
			internalFreeCount(0),
			currentBlockCount(0),
			currentAllocatedMemory(0),
			currentUsedMemory(0)
		{}
		uint64_t externalAllocCount;
		uint64_t externalFreeCount;
		uint64_t internalAllocCount;
		uint64_t internalFreeCount;
		uint64_t currentBlockCount;
		uint64_t currentAllocatedMemory;
		uint64_t currentUsedMemory;
	};


	struct GlobalParams;
	void InitializeMemory(const GlobalParams & params);

	void ShutDownMemory();

	// Get memory stats
	MemoryStats GetMemoryStats();

	// Log all currently allocated memory (debug only)
	void LogAllocations();

	// Define a custom UTF-8 string using internal allocator
	using String = std::basic_string <char, std::char_traits<char>, Allocator<char>>;

	// Define a custom UTF-16 string using internal allocator
	using StringU16 = std::basic_string <char16_t, std::char_traits<char16_t>, Allocator<char16_t>>;

	// Define a custom wide character string using internal allocator
	using WString = std::basic_string <wchar_t, std::char_traits<wchar_t>, Allocator<wchar_t>>;

}

#endif // JX_MEMORY_H__
