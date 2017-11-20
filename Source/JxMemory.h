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

	// Fix unreferenced variable warnings
	template<typename T>
	constexpr int ref(const T &) { return 0; }
	
	
	// Stand-alone global allocation functions (debug and release version)

#ifdef JINX_DEBUG_ALLOCATION

	void * MemPoolAllocate(const char * file, const char * function, uint32_t line, size_t bytes);

	void * MemPoolReallocate(const char * file, const char * function, uint32_t line, void * ptr, size_t bytes);

	template<typename T>
	T * MemPoolAllocateType(const char * file, const char * function, uint32_t line)
	{
		return (T *)MemPoolAllocate(file, function, line, sizeof(T));
	}

	template<typename T>
	T * MemPoolAllocateObject(const char * file, const char * function, uint32_t line)
	{
		T * obj = MemPoolAllocateType<T>(file, function, line);
		new(obj)T();
		return obj;
	}

	template<typename T, typename A1>
	T * MemPoolAllocateObject(const char * file, const char * function, uint32_t line, A1 && arg1)
	{
		T * obj = MemPoolAllocateType<T>(file, function, line);
		new(obj)T(std::forward<A1>(arg1));
		return obj;
	}

	template<typename T, typename A1, typename A2>
	T * MemPoolAllocateObject(const char * file, const char * function, uint32_t line, A1 && arg1, A2 && arg2)
	{
		T * obj = MemPoolAllocateType<T>(file, function, line);
		new(obj)T(std::forward<A1>(arg1), std::forward<A2>(arg2));
		return obj;
	}

	template<typename T, typename A1, typename A2, typename A3>
	T * MemPoolAllocateObject(const char * file, const char * function, uint32_t line, A1 && arg1, A2 && arg2, A3 && arg3)
	{
		T * obj = MemPoolAllocateType<T>(file, function, line);
		new(obj)T(std::forward<A1>(arg1), std::forward<A2>(arg2), std::forward<A3>(arg3));
		return obj;
	}

	template<typename T, typename A1, typename A2, typename A3, typename A4>
	T * MemPoolAllocateObject(const char * file, const char * function, uint32_t line, A1 && arg1, A2 && arg2, A3 && arg3, A4 && arg4)
	{
		T * obj = MemPoolAllocateType<T>(file, function, line);
		new(obj)T(std::forward<A1>(arg1), std::forward<A2>(arg2), std::forward<A3>(arg3), std::forward<A4>(arg4));
		return obj;
	}

	template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
	T * MemPoolAllocateObject(const char * file, const char * function, uint32_t line, A1 && arg1, A2 && arg2, A3 && arg3, A4 && arg4, A5 && arg5)
	{
		T * obj = MemPoolAllocateType<T>(file, function, line);
		new(obj)T(std::forward<A1>(arg1), std::forward<A2>(arg2), std::forward<A3>(arg3), std::forward<A4>(arg4), std::forward<A5>(arg5));
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

	template<typename T, typename A1>
	T * MemPoolAllocateObject(A1 && arg1)
	{
		T * obj = MemPoolAllocateType<T>();
		new(obj)T(std::forward<A1>(arg1));
		return obj;
	}

	template<typename T, typename A1, typename A2>
	T * MemPoolAllocateObject(A1 && arg1, A2 && arg2)
	{
		T * obj = MemPoolAllocateType<T>();
		new(obj)T(std::forward<A1>(arg1), std::forward<A2>(arg2));
		return obj;
	}

	template<typename T, typename A1, typename A2, typename A3>
	T * MemPoolAllocateObject(A1 && arg1, A2 && arg2, A3 && arg3)
	{
		T * obj = MemPoolAllocateType<T>();
		new(obj)T(std::forward<A1>(arg1), std::forward<A2>(arg2), std::forward<A3>(arg3));
		return obj;
	}

	template<typename T, typename A1, typename A2, typename A3, typename A4>
	T * MemPoolAllocateObject(A1 && arg1, A2 && arg2, A3 && arg3, A4 && arg4)
	{
		T * obj = MemPoolAllocateType<T>();
		new(obj)T(std::forward<A1>(arg1), std::forward<A2>(arg2), std::forward<A3>(arg3), std::forward<A4>(arg4));
		return obj;
	}

	template<typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
	T * MemPoolAllocateObject(A1 && arg1, A2 && arg2, A3 && arg3, A4 && arg4, A5 && arg5)
	{
		T * obj = MemPoolAllocateType<T>();
		new(obj)T(std::forward<A1>(arg1), std::forward<A2>(arg2), std::forward<A3>(arg3), std::forward<A4>(arg4), std::forward<A5>(arg5));
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
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T value_type;

		Allocator() throw() {};
		Allocator(const Allocator &) throw() { };

		template<typename U>
		Allocator(const Allocator<U>&) throw() { };

		template<typename U>
		Allocator & operator = (const Allocator<U> & other) { return *this; }
		Allocator & operator = (const Allocator & other) { return *this; }
		~Allocator() {}

		template <typename U>
		struct rebind { typedef Allocator<U> other; };

		pointer address(reference value) const { return &value; }
		const_pointer address(const_reference value) const { return &value; }

		pointer allocate(size_type n) { return static_cast<pointer> (Jinx::JinxAlloc(n * sizeof(value_type))); }
		pointer allocate(size_type n, const void *) { return static_cast<pointer> (Jinx::JinxAlloc(n * sizeof(value_type))); }
		void deallocate(void* ptr, size_type) { Jinx::JinxFree(static_cast<T*> (ptr)); }

		template<typename U, typename... Args>
		void construct(U* ptr, Args&&  ... args) { ::new ((void*)(ptr)) U(std::forward<Args>(args)...); }
		void construct(pointer ptr, const T& val) { new (static_cast<T*> (ptr)) T(val); }

		template<typename U>
		void destroy(U* ptr) { Jinx::ref(ptr); ptr->~U(); }
		void destroy(pointer ptr) { Jinx::ref(ptr); ptr->~T(); }

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
	typedef std::basic_string <char, std::char_traits<char>, Allocator<char>> String;

	// Define a custom UTF-16 string using internal allocator
	typedef std::basic_string <char16_t, std::char_traits<char16_t>, Allocator<char16_t>> StringU16;

	// Define a custom wide character string using internal allocator
	typedef std::basic_string <wchar_t, std::char_traits<wchar_t>, Allocator<wchar_t>> WString;

};

#endif // JX_MEMORY_H__