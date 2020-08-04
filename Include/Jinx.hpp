

// begin --- JxBuffer.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

// begin --- JxInternal.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_INTERNAL_H__
#define JX_INTERNAL_H__

// begin --- Jinx.h --- 

/*
The MIT License (MIT)

Copyright (c) 2016 James Boer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once
#ifndef JINX_H__
#define JINX_H__

/*! \file */


/*! 
\mainpage Jinx Documentation Index Page

\section intro_sec Introduction

Jinx is an embedded scripting language designed for use in real-time applications
like videogames.  It features a simple, intuitive scripting syntax that looks 
remarkably like pseudo-code, and a clean, modern C++ interface for ease of use 
and integration.

The Jinx source code, documentation, and all related files are copyrighted (c) 2016
by James Boer, and distributed under the MIT license.
*/

// Platform definitions
#if defined(_WIN32) || defined(_WIN64)
#define JINX_WINDOWS
#define JINX_USE_FROM_CHARS
#pragma warning(push)
#pragma warning(disable : 4530) // Silence warnings if exceptions are disabled
#endif

#if defined(__linux__) || defined(__linux)
#define JINX_LINUX
#endif

#ifdef __APPLE__
#ifdef __MACH__
#define JINX_MACOS
#endif
#endif

/*
On macOS, use of std::any is restricted to applications targeting versions 10.14 and up (Mohave) due to
limitations in std::any_cast.  As such, Jinx provides optional void * aliases in place of std::any in
case a project wishes to target macOS clients earlier than 10.14.
*/

#define JINX_USE_ANY

#include <memory>
#include <functional>
#include <vector>
#include <map>
#include <string>
#include <cstddef>
#include <limits>
#include <cstring>
#ifdef JINX_USE_ANY
#include <any>
#endif

#ifdef JINX_WINDOWS
#pragma warning(pop)
#endif

// begin --- JxMemory.h --- 

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


// end --- JxMemory.h --- 



// begin --- JxBuffer.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_BUFFER_H__
#define JX_BUFFER_H__

/*! \file */

/*! \namespace */
namespace Jinx
{
	class Buffer;
	using BufferPtr = std::shared_ptr<Buffer>;

	class Buffer 
	{
	public:
		Buffer();
		~Buffer();
		size_t Capacity() const;
		void Clear();
		void Read(size_t * pos, void * data, size_t bytes);
		void Read(size_t * pos, BufferPtr & buffer, size_t bytes);
		void Reserve(size_t size);
		inline uint8_t * Ptr() const { return m_data; }
		inline size_t Size() const { return m_size; }
		void Write(const void * data, size_t bytes);
		void Write(size_t * pos, const void * data, size_t bytes);

	private:
		uint8_t * m_data;
		size_t m_size;
		size_t m_capacity;
	};


	BufferPtr CreateBuffer();
}

#endif // JX_BUFFER_H__


// end --- JxBuffer.h --- 



// begin --- JxCollection.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_COLLECTION_H__
#define JX_COLLECTION_H__

/*! \file */

/*! \namespace */
namespace Jinx
{
	class Variant;
	using Collection = std::map<Variant, Variant, std::less<Variant>, Allocator<std::pair<const Variant, Variant>>>;
	using CollectionPtr = std::shared_ptr<Collection>;
	using CollectionItr = Collection::iterator;
	using CollectionItrPair = std::pair<CollectionItr, CollectionPtr>;
	CollectionPtr CreateCollection();
}

#endif // JX_COLLECTION_H__



// end --- JxCollection.h --- 



// begin --- JxGuid.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_GUID_H__
#define JX_GUID_H__

namespace Jinx
{

	struct Guid
	{
		uint32_t data1;
		uint16_t  data2;
		uint16_t  data3;
		uint8_t  data4[8];
	};

	const Guid NullGuid = { 0, 0, 0,{ 0, 0, 0, 0, 0, 0, 0, 0 } };

	inline bool operator < (const Guid & left, const Guid & right)
	{
		return (memcmp(&left, &right, sizeof(Guid)) < 0) ? true : false;
	}

	inline bool operator == (const Guid & left, const Guid & right)
	{
		return (memcmp(&left, &right, sizeof(Guid)) == 0) ? true : false;
	}

	inline bool operator != (const Guid & left, const Guid & right)
	{
		return !(left == right);
	}

}




#endif // JX_GUID_H__


// end --- JxGuid.h --- 



// begin --- JxVariant.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_VARIANT_H__
#define JX_VARIANT_H__

/*! \file */

/*! \namespace */
namespace Jinx
{

	class BinaryReader;
	class BinaryWriter;

	/// Interface for user objects in scripts
	class IUserObject
	{
	public:
		virtual ~IUserObject() {};
	};

	using UserObjectPtr = std::shared_ptr<IUserObject>;

	/// ValueType represents the type of value contained in a Variant object
	enum class ValueType
	{
		Null,
		Number,
		Integer,
		Boolean,
		String,
		Collection,
		CollectionItr,
		UserObject,
		Buffer,
		Guid,
		ValType,
		NumValueTypes,
		Any = NumValueTypes, // Internal use only
	};

	/// Variant stores value/type pairs in a convenient class.
	/**
	The Variant class is both used to store and manipulate values internally.  It can convert
	between different value types dynamically.  It also provides a convenient mechanism to pass 
	and retrieve values from the native Jinx API in a type-agnostic fashion.
	*/
	class Variant
	{
	public:

		// Constructor overloads
		Variant() :
			m_type(ValueType::Null)
		{}
		Variant(const Variant & copy);
		Variant(std::nullptr_t) : m_type(ValueType::Null) { SetNull(); }
		Variant(bool value) : m_type(ValueType::Null) { SetBoolean(value); }
		Variant(int32_t value) : m_type(ValueType::Null) { SetInteger(value); }
		Variant(int64_t value) : m_type(ValueType::Null) { SetInteger(value); }
		Variant(double value) : m_type(ValueType::Null) { SetNumber(value); }
		Variant(const char * value) : m_type(ValueType::Null) { SetString(value); }
		Variant(const char16_t * value) : m_type(ValueType::Null) { SetString(value); }
		Variant(const wchar_t * value) : m_type(ValueType::Null) { SetString(value); }
		Variant(const String & value) : m_type(ValueType::Null) { SetString(value); }
		Variant(const StringU16 & value) : m_type(ValueType::Null) { SetString(value); }
		Variant(const WString & value) : m_type(ValueType::Null) { SetString(value); }
		Variant(const CollectionPtr & value) : m_type(ValueType::Null) { SetCollection(value); }
		Variant(const CollectionItrPair & value) : m_type(ValueType::Null) { SetCollectionItr(value); }
		Variant(const UserObjectPtr & value) : m_type(ValueType::Null) { SetUserObject(value); }
		Variant(const BufferPtr & value) : m_type(ValueType::Null) { SetBuffer(value); }
		Variant(const Guid & value) : m_type(ValueType::Null) { SetGuid(value); }
		Variant(ValueType value) : m_type(ValueType::Null) { SetValType(value); }

		// Destructor
		~Variant();

		// Assignment operator overloads
		Variant & operator= (const Variant & copy);

		// Increment operators
		Variant & operator++();
		Variant operator++(int) {
			Variant tmp(*this);
			operator++();
			return tmp;
		}
		Variant & operator += (const Variant & right);

		// Decrement operators
		Variant & operator--();
		Variant operator--(int) {
			Variant tmp(*this);
			operator--();
			return tmp;
		}
		Variant & operator -= (const Variant & right);

		// Value getters
		bool GetBoolean() const;
		int64_t GetInteger() const;
		double GetNumber() const;
		String GetString() const;
		StringU16 GetStringU16() const;
		WString GetWString() const;
		CollectionPtr GetCollection() const;
		CollectionItrPair GetCollectionItr() const;
		UserObjectPtr GetUserObject() const;
		BufferPtr GetBuffer() const;
		Guid GetGuid() const;
		ValueType GetValType() const;

		// Type getter
		ValueType GetType() const { return m_type; }

		// Is this a valid collection key?
		bool IsKeyType() const;

		// Is this an integer or number type?
		bool IsNumericType() const;

		// Type checks
		bool IsType(ValueType type) { return m_type == type ? true : false; }
		bool IsNull() const { return m_type == ValueType::Null ? true : false; }
		bool IsBoolean() const { return m_type == ValueType::Boolean ? true : false; }
		bool IsInteger() const { return m_type == ValueType::Integer ? true : false; }
		bool IsNumber() const { return m_type == ValueType::Number ? true : false; }
		bool IsString() const { return m_type == ValueType::String ? true : false; }
		bool IsCollection() const { return m_type == ValueType::Collection ? true : false; }
		bool IsCollectionItr() const { return m_type == ValueType::CollectionItr ? true : false; }
		bool IsUserObject() const { return m_type == ValueType::UserObject ? true : false; }
		bool IsBuffer() const { return m_type == ValueType::Buffer ? true : false; }
		bool IsGuid() const { return m_type == ValueType::Guid ? true : false; }
		bool IsValType() const { return m_type == ValueType::ValType ? true : false; }

		// Value setters
		void SetNull();
		void SetBoolean(bool value);
		void SetInteger(int64_t value);
		void SetNumber(double value);
		void SetString(const String & value);
		void SetString(const StringU16 & value);
		void SetString(const WString & value);
		void SetCollection(const CollectionPtr & value);
		void SetCollectionItr(const CollectionItrPair & value);
		void SetUserObject(const UserObjectPtr & value);
		void SetBuffer(const BufferPtr & value);
		void SetGuid(const Guid & value);
		void SetValType(ValueType type);

		// Check to see if a successful type conversion can be made
		bool CanConvertTo(ValueType type) const;

		// Conversion casts.  True if successful, false if invalid cast
		bool ConvertTo(ValueType type);

		// Serialization
		void Read(BinaryReader & reader);
		void Write(BinaryWriter & writer) const;

	private:

		void Destroy();

		ValueType m_type;
		union
		{
			bool m_boolean;
			double m_number;
			int64_t m_integer;
			ValueType m_valType;
			String m_string;
			CollectionPtr m_collection;
			CollectionItrPair m_collectionItrPair;
			UserObjectPtr m_userObject;
			BufferPtr m_buffer;
			Guid m_guid;
		};
	};

	// Arithmetic operators
	Variant operator + (const Variant & left, const Variant & right);
	Variant operator - (const Variant & left, const Variant & right);
	Variant operator * (const Variant & left, const Variant & right);
	Variant operator / (const Variant & left, const Variant & right);
	Variant operator % (const Variant & left, const Variant & right);

	// Comparison operators
	bool operator == (const Variant & left, const Variant & right);
	bool operator < (const Variant & left, const Variant & right);
	bool operator <= (const Variant & left, const Variant & right);
	inline bool operator != (const Variant & left, const Variant & right) { return (left == right) ? false : true; }
	inline bool operator > (const Variant & left, const Variant & right) { return (left <= right) ? false : true; }
	inline bool operator >= (const Variant & left, const Variant & right) { return (left < right) ? false : true; }

	// Variant operator <, <=, >, >= type validation
	bool ValidateValueComparison(const Variant & left, const Variant & right);

	/// Helper function convertion to utf-8 string from other types using a Variant object
	inline String Str(const char16_t * str) { return Variant(str).GetString(); }
	inline String Str(const wchar_t * str) { return Variant(str).GetString(); }
	inline String Str(const StringU16 & str) { return Variant(str).GetString(); }
	inline String Str(const WString & str) { return Variant(str).GetString(); }

}

#endif // JX_VARIANT_H__

// end --- JxVariant.h --- 



#ifndef inline_t
#define inline_t
#endif

/*! \namespace */
namespace Jinx
{
	/// Major version number
	const uint32_t MajorVersion = 1;

	/// Minor version number
	const uint32_t MinorVersion = 1;

	/// Patch number
	const uint32_t PatchNumber = 9;

	// Forward declaration
	class IScript;

	// Shared pointer to script object
	using ScriptPtr = std::shared_ptr<IScript>;

	// Function parameters
	using Parameters = std::vector<Variant, Allocator<Variant>>;

	// Signature for native function callback
	using FunctionCallback = std::function<Variant(ScriptPtr, const Parameters &)>;

	using RuntimeID = uint64_t;
	const RuntimeID InvalidID = 0;

#ifdef JINX_USE_ANY
	using Any = std::any;
#define JinxAny std::any
#define JinxAnyCast std::any_cast
#else
	using Any = void *;
#define JinxAny Jinx::Any
#define JinxAnyCast reinterpret_cast
#endif

	/// Determines visibility when registering a function
	enum class Visibility
	{
		/// The registered function will be visible outside the library
		Public,
		/// The registered function will only be visible to library functions
		Private
	};

	/// Determines the access type of a registered property
	enum class Access
	{
		/// The registered property can both be read and written to
		ReadWrite,
		/// The registered property can only be read from
		ReadOnly,
	};

	/// Indicates the logging level of Jinx output logs
	enum class LogLevel
	{
		/// This message is purely for informational or debugging purposes
		Info,
		/// This is a warning message which indicates a potential issue
		Warning,
		/// This message is a compile-time or run-time error condition
		Error,
	};

	/// ILibrary represents a single module of script code.
	/** 
	Multiple scripts can be compiled as part of a single library.  This interface allows the creation of
	native functions that script code can execute via RegisterFunction() and properties through the use
	of the RegisterProperty() function.  You can also get or set property values using the GetProperty()
	and SetProperty() functions.
	*/
	class ILibrary
	{
	public:

		/// Register a native function for use by scripts
		/**
		This method registers a native function for use by script code.
		\param visibility Indicates whether function is public or private.
		\param name String containing all function nameparts and parameters.  Parameters are indicated with "{}",
		while names are expected to conform to standard Jinx identifier naming rules.
		\param function The callback function executed by the script.
		\return Returns true on success or false on failure.
		*/
		virtual bool RegisterFunction(Visibility visibility, const String & name, FunctionCallback function) = 0;

		/// Register a property for use by scripts
		/**
		This method registers a library property.
		\param visibility Indicates whether property is public or private. 
		\param access Indicates whether the property is read-only or read/write.
		\param name Property name
		\param value Default property value
		\return Returns true on success or false on failure.
		\sa FunctionCallback, Visibility, Access
		*/
		virtual bool RegisterProperty(Visibility visibility, Access access, const String & name, const Variant & value) = 0;

		/// Get property value
		/**
		This method retrieves a property value by name.
		\param name Property name to retrieve
		\return Returns the indicated property value.  Returns a null Variant on failure to find property.
		*/
		virtual Variant GetProperty(const String & name) const = 0;

		/// Set property value
		/**
		This method sets a property value by name.
		\param name Property name to set
		\param value Variant value to set
		*/
		virtual void SetProperty(const String & name, const Variant & value) = 0;

	protected:
		virtual ~ILibrary() {}
	};

	/// Shared pointer to library object
	using LibraryPtr = std::shared_ptr<ILibrary>;
	

	/// Script interface.
	/** 
	IScript represents a single script executing as compiled bytecode.  Each script
	has its own execution stack, and so can run for an indeterminate amount of time.
	The user is expected to run the script by calling Execute() each update tick
	until it's finished executing.  Check to see if the script is finished executing 
	using the IsFinished() function.
	*/
	class IScript
	{
	public:

		/// Execute the script
		/**
		The client is responsible for calling Execute() once per simulation tick until the
		script is finished executing, which is checked with the IsFinished() function.
		\return true if executed successfully and false if an error has occured.
		*/
		virtual bool Execute() = 0;

		/// Checks if the script is finished executing
		/**
		Determines if the script has finished executed or hit a fatal error
		\return true if finished, false if still executing.
		*/
		virtual bool IsFinished() const = 0;

		/// Get a variable by name
		/**
		\param name The name of the variable to retrieve
		\return Variant representing the variable to retrieve.  A Variant of type ValueType::Null is
		returned if the variable is not found.
		*/
		virtual Variant GetVariable(const String & name) const = 0;

		/// Set a variable by name
		/**
		\param name The name of the variable to set
		\param value The value of the variable to set
		*/
		virtual void SetVariable(const String & name, const Variant & value) = 0;

		/// Find the ID of a library function
		/**
		\param library Pointer to library containing function to call
		\param name String containing all function nameparts and parameters.  Parameters are indicated with "{}",
		while names are expected to conform to standard Jinx identifier naming rules.
		\return Returns a valid RuntimeID on success, InvalidID on failure.
		*/
		virtual RuntimeID FindFunction(LibraryPtr library, const String & name) = 0;

		/// Call a library function
		/**
		\param id RuntimeID of the function to call
		\param params Vector of Variants to act as function parameters
		\return Returns the Variant containing the function return value, or null for no value.
		*/
		virtual Variant CallFunction(RuntimeID id, Parameters params) = 0;

		/// Get the script name
		/**
		\return This returns the name of the script designated by the user when compiling from source.
		\sa IRuntime::CompileScript(), IRuntime::ExecuteScript()
		*/
		virtual const String & GetName() const = 0;

		/// Get a user context pointer
		/**
		\return void pointer optionally passed at script creation.  This is intended to be
		used by native library functions to retrieve user-specific data or objects.
		*/
		virtual Any GetUserContext() const = 0;

		/// Return the library to which this script belongs
		/**
		\return This function returns a shared pointer to the library which this script belongs to. 
		*/
		virtual LibraryPtr GetLibrary() const = 0;

	protected:
		virtual ~IScript() {}
	};

	/// Performance stats
	/**
	Struct that contains several performance stats of scripts since the beginning of the 
	runtime object creation or the last time IRuntime::GetScriptPerformanceStats() was 
	called with the cleared flag set.
	\sa IRuntime::GetScriptPerformanceStats()
	*/
	struct PerformanceStats
	{
		PerformanceStats() :
			compilationTimeNs(0),
			executionTimeNs(0),
			perfTimeNs(0),
			scriptCompilationCount(0),
			scriptExecutionCount(0),
			scriptCompletionCount(0),
			instructionCount(0)
		{}
		/// Total compilation time of all scripts in nanoseconds
		uint64_t compilationTimeNs;
		/// Total execution time of all scripts in nanoseconds
		uint64_t executionTimeNs;
		/// Performance sample time in nanoseconds
		uint64_t perfTimeNs;
		/// Number of scripts compiled
		uint64_t scriptCompilationCount;
		/// Number of scripts executed
		uint64_t scriptExecutionCount;
		/// Number of scripts completed
		uint64_t scriptCompletionCount;
		/// Number of instructions executed
		uint64_t instructionCount;
	};

	/// Jinx runtime interface
	/** 
	The Jinx runtime interface represents the entire runtime scripting environment.
	*/
	class IRuntime
	{
	public:

		/// Compile text string into bytecode.
		/**
		Pass a C-string filled with text to compile it to executable bytecode.  This
		bytecode buffer is then passed to the CreateScript() function to create a
		script ready for execution.
		\param scriptText A C string containing text to compile to bytecode
		\param name The name of the script, typically the filename, used for debugging
		and diagnostic purposes.
		\param libraries A list of libraries to import by default.
		\return A BufferPtr containing compiled bytecode on success or a nullptr on failure.
		\sa CreateScript()
		*/
		virtual BufferPtr Compile(const char * scriptText, String name = String(), std::initializer_list<String> libraries = {}) = 0;

		/// Create a script from bytecode
		/**
		Create script from compiled bytecode retrieved from the Compile() function.
		\param bytecode A BufferPtr containing the compiled bytecode generated by
		the output of the Compile() function.
		\param userContext A void pointer containing per-script user-defined data or object.
		\return A ScriptPtr ready for execution.
		\sa Compile(), IScript
		*/
		virtual ScriptPtr CreateScript(BufferPtr bytecode, Any userContext = nullptr) = 0;

		/// Compile and create script from text
		/**
		Compiles script text to bytecode, then creates and returns a script if successful.
		\param scriptText A C string containing text to compile to bytecode
		\param userContext A void pointer containing per-script user-defined data or object.
		\param name The name of the script, typically the filename, used for debugging
		and diagnostic purposes.
		\param libraries A list of libraries to import by default.
		\return A ScriptPtr containing compiled bytecode on success or a nullptr on failure.
		*/
		virtual ScriptPtr CreateScript(const char * scriptText, Any userContext = nullptr, String name = String(), std::initializer_list<String> libraries = {}) = 0;

		/// Compile, create, and execute a script
		/**
		Compiles script text to bytecode, then creates and execute script, and returns a 
		script if successful.
		\param scriptText A C string containing text to compile to bytecode
		\param userContext A void pointer containing per-script user-defined data or object.
		\param name The name of the script, typically the filename, used for debugging
		and diagnostic purposes.
		\param libraries A list of libraries to import by default.
		\return A ScriptPtr containing compiled bytecode on success or a nullptr on failure.
		*/
		virtual ScriptPtr ExecuteScript(const char * scriptText, Any userContext = nullptr, String name = String(), std::initializer_list<String> libraries = {}) = 0;

		/// Retrieve library by name or create empty library if not found
		/**
		\param name Name of the library to create or retrieve
		\return A LibraryPtr pointer to an ILibrary interface.
		\sa Compile(), ILibrary
		*/
		virtual LibraryPtr GetLibrary(const String & name) = 0;

		/// Get script performance stats
		/**
		Retrieve all script performance stats, optionally resetting the stat counters.
		\param resetStats A true value reset the stats, and false allows them to continue accumulating
		\return A PerfStats structure containing various performance statistics
		\sa PerformanceStats
		*/
		virtual PerformanceStats GetScriptPerformanceStats(bool resetStats = true) = 0;

		// Strip debug info from bytecode
		/**
		Strip bytecode of debug info and return in a new buffer
		\param bytecode A buffer containing compiled bytecode
		\return A new buffer containing bytecode without debug info, or the same buffer if already stripped.  Returns a
		null pointer on error.
		\sa PerformanceStats
		*/
		virtual BufferPtr StripDebugInfo(BufferPtr bytecode) const = 0;

	protected:
		virtual ~IRuntime() {}
	};

	/// Shared pointer to runtime object
	using RuntimePtr = std::shared_ptr<IRuntime>;


	/// Create a runtime object
	/**
	\sa IRuntime
	*/
	RuntimePtr CreateRuntime();


	/// Prototype for global memory allocation function callback
	using AllocFn = std::function<void *(size_t)>;

	/// Prototype for global memory re-allocation function callback
	using ReallocFn = std::function<void *(void *, size_t)>;

	/// Prototype for global memory free function callback
	using FreeFn = std::function<void(void *)>;

	/// Prototype for global logging function callback
	using LogFn = std::function<void(LogLevel level, const char *)>;


	/// Initializes global Jinx parameters
	/**
	A struct containing a number of global parameters affecting memory, logging, and
	general runtime behavior.
	\sa Initialize()
	*/
	struct GlobalParams
	{
		GlobalParams() :
			enableLogging(true), 
			logSymbols(false),
			logBytecode(false),
			enableDebugInfo(true),
			allocBlockSize(8192),
			allocSpareBlocks(4),
			maxInstructions(2000),
			errorOnMaxInstrunctions(true)
		{}
		/// Logging function 
		LogFn logFn;
		/// Enable logging
		bool enableLogging;
		/// Log a detailed list of symbols after initial lexing phase
		bool logSymbols;
		/// Log a detailed list of bytecode after compilation
		bool logBytecode;
		/// Embed debug info in bytecode
		bool enableDebugInfo;
		/// Alloc memory function
		AllocFn allocFn;
		/// Realloc memory function
		ReallocFn reallocFn;
		/// Free memory function
		FreeFn freeFn;
		/// Size of each individual block allocation in bytes
		size_t allocBlockSize;
		/// Number of spare allocation blocks to retain
		size_t allocSpareBlocks;
		/// Maximum number of instructions per script per Execute() function
		uint32_t maxInstructions;
		/// Maximum total script instrunctions
		bool errorOnMaxInstrunctions;
	};

	/// Get Jinx version in string form for easier display
	String GetVersionString();

	/// Initializes global Jinx parameters
	/**
	This function should be called before any other functions are called.  It allows the client
	to set up custom logging options and memory allocation if desired.
	\param params A GlobalParams struct containing a number of parameters that affect any
	created runtime objects.
	\sa GlobalParams
	*/
	void Initialize(const GlobalParams & params);

	/// Shut down global Jinx systems
	/**
	This optional-use function shuts down the memory system, allowing leak-checking to validate 
	all memory has been freed.
	\sa Initialize
	*/
	void ShutDown();

}

#endif // JINX_H__


// end --- Jinx.h --- 



#ifdef JINX_WINDOWS
#pragma warning(push)
#pragma warning(disable : 4530) // Silence warnings if exceptions are disabled
#endif

#include <cassert>
#include <cstdarg>
#include <mutex>
#include <algorithm>
#include <memory>
#include <string>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <vector>
#include <random>
#include <cctype>
#include <cinttypes>
#include <string.h>
#include <cstddef>
#include <atomic>
#include <locale>
#include <codecvt>
#include <climits>
#ifdef JINX_USE_FROM_CHARS
#include <charconv>
#else
#include <sstream>
#endif

#ifdef JINX_WINDOWS
#pragma warning(pop)
#endif

// begin --- JxLogging.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_LOGGING_H__
#define JX_LOGGING_H__


namespace Jinx::Impl
{

	void InitializeLogging(const GlobalParams & params);

	bool IsLogSymbolsEnabled();
	bool IsLogBytecodeEnabled();

	void LogWrite(LogLevel level, const char * format, ...);
	void LogWriteLine(LogLevel level, const char * format, ...);

} // namespace Jinx::Impl

#endif // JX_LOGGING_H__


// end --- JxLogging.h --- 



// begin --- JxCommon.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_COMMON_H__
#define JX_COMMON_H__

namespace Jinx::Impl
{

	const uint32_t LogTabWidth = 4;

	// All script opcodes
	enum class Opcode
	{
		Add,
		And,
		CallFunc,
		Cast,
		Decrement,
		Divide,
		Equals,
		EraseItr,
		ErasePropKeyVal,
		EraseVarKeyVal,
		Exit,
		Function,
		Greater,
		GreaterEq,
		Increment,
		Jump,
		JumpFalse,
		JumpFalseCheck,
		JumpTrue,
		JumpTrueCheck,
		Less,
		LessEq,
		Library,
		LoopCount,
		LoopOver,
		Mod,
		Multiply,
		Negate,
		Not,
		NotEquals,
		Or,
		Pop,
		PopCount,
		Property,
		PushColl,
		PushItr,
		PushKeyVal,
		PushList,
		PushProp,
		PushTop,
		PushVar,
		PushVal,
		Return,
		ScopeBegin,
		ScopeEnd,
		SetIndex,
		SetProp,
		SetPropKeyVal,
		SetVar,
		SetVarKeyVal,
		Subtract,
		Type,
		Wait,
		NumOpcodes,
	};

	const char * GetOpcodeText(Opcode opcode);

	// Symbols
	enum class SymbolType
	{
		None,
		Invalid,
		NewLine,
		NameValue,			// Value types begin
		StringValue,
		NumberValue,
		IntegerValue,
		BooleanValue,
		ForwardSlash,		// Operator begin
		Asterisk,
		Plus,
		Minus,
		Equals,
		NotEquals,
		Percent,
		Comma,
		ParenOpen,
		ParenClose,
		CurlyOpen,
		CurlyClose,
		SquareOpen,
		SquareClose,
		Ellipse,
		SingleQuote,
		LessThan,
		LessThanEquals,
		GreaterThan,
		GreaterThanEquals,
		And,				// Keyword begin.  
		As,
		Begin,
		Boolean,
		Break,
		By,
		Collection,
		Decrement,
		Else,
		End,
		Erase,
		External,
		From,
		Function,
		Guid,
		If,
		Import,
		Increment,
		Integer,
		Is,
		Library,
		Loop,
		Not,
		Null,
		Number,
		Object,
		Or,
		Over,
		Private,
		Public,
		Readonly,
		Return,
		Set,
		String,
		To,
		Type,
		Until,
		Wait,
		While,
		NumSymbols,
	};

	const char * GetSymbolTypeText(SymbolType symbol);

	// Check to see if a symbol is a particular category
	bool IsConstant(SymbolType symbol);
	bool IsOperator(SymbolType symbol);
	bool IsKeyword(SymbolType symbol);

	// Get the name of a value type
	const char * GetValueTypeName(ValueType valueType);

	// Visibility type 
	enum class VisibilityType
	{
		Local,
		Private,
		Public,
	};

	constexpr uint32_t MakeFourCC(char ch0, char ch1, char ch2, char ch3)
	{
		return ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8 |
			((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24)));
	}

	const uint32_t BytecodeSignature = MakeFourCC('J', 'I', 'N', 'X');
	const uint32_t BytecodeVersion = 1;

	struct BytecodeHeader
	{
		BytecodeHeader() :
			signature(BytecodeSignature),
			version(BytecodeVersion),
			dataSize(0)
		{}
		uint32_t signature;
		uint32_t version;
		uint32_t dataSize;
	};

	static_assert(sizeof(BytecodeHeader) == 12, "BytecodeHeader struct is not properly aligned on this platform");

	const uint32_t DebugSignature = MakeFourCC('J', 'D', 'B', 'G');

	struct DebugHeader
	{
		DebugHeader() :
			signature(DebugSignature),
			lineEntryCount(0),
			dataSize(0)
		{}
		uint32_t signature;
		uint32_t lineEntryCount;
		uint32_t dataSize;
	};

	static_assert(sizeof(DebugHeader) == 12, "DebugHeader struct is not properly aligned on this platform");

	struct DebugLineEntry
	{
		uint32_t opcodePosition;
		uint32_t lineNumber;
	};

	template <typename T>
	inline T NextHighestMultiple(T val, T multiple)
	{
		return val + ((multiple - (val % multiple)) % multiple);
	}

	template<typename T, size_t s>
	constexpr size_t countof(T(&)[s])
	{
		return s;
	}

	inline const char * StrCopy(char * dest, size_t destBufferSize, const char * source)
	{
#if defined(JINX_WINDOWS)
		strncpy_s(dest, destBufferSize, source, ((size_t)-1));
		return dest;
#else
		strncpy(dest, source, destBufferSize);
		return dest;
#endif
	}

	// Get number of parts in name
	size_t GetNamePartCount(const String & name);

	// Generate runtime id from unique name information
	RuntimeID GetVariableId(const char * name, size_t nameLen, size_t stackDepth);

	RuntimeID GetRandomId();
	uint32_t MaxInstructions();
	bool ErrorOnMaxInstrunction();
	bool EnableDebugInfo();

	// Forward declarations
	class Runtime;
	using RuntimeIPtr = std::shared_ptr<Runtime>;
	using RuntimeWPtr = std::weak_ptr<Runtime>;

	// Shared aliases
	using SymbolTypeMap = std::map<String, SymbolType, std::less<String>, Allocator<std::pair<const String, SymbolType>>>;

} // namespace Jinx::Impl




#endif // JX_COMMON_H__


// end --- JxCommon.h --- 



// begin --- JxUnicode.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_UNICODE_H__
#define JX_UNICODE_H__


namespace Jinx::Impl
{

	size_t GetUtf8CharSize(const char * utf8Str);

	StringU16 ConvertUtf8ToUtf16(const String & utf8Str);
	String ConvertUtf16ToUtf8(const StringU16 & utf16Str);

	WString ConvertUtf8ToWString(const String & utf8Str);
	String ConvertWStringToUtf8(const WString & wStr);

	bool IsCaseFolded(const String & source);
	String FoldCase(const String & source);

} // namespace Jinx::Impl

#endif // JX_UNICODE_H__


// end --- JxUnicode.h --- 



// begin --- JxUnicodeCaseFolding.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_UNICODE_CASE_FOLDING_H__
#define JX_UNICODE_CASE_FOLDING_H__


namespace Jinx::Impl
{

	struct CaseFoldingData
	{
		char32_t sourceCodePoint;
		char32_t destCodePoint1;
		char32_t destCodePoint2;
	};

	bool FindCaseFoldingData(char32_t sourceCodePoint, char32_t * destCodePoint1, char32_t * destCodePoint2);

} // namespace Jinx::Impl

#endif // JX_UNICODE_CASE_FOLDING_H__


// end --- JxUnicodeCaseFolding.h --- 



// begin --- JxConversion.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_CONVERSION_H__
#define JX_CONVERSION_H__


namespace Jinx::Impl
{

	// Convert value type to byte and back
	ValueType ByteToValueType(uint8_t byte);
	uint8_t ValueTypeToByte(ValueType type);

	// Null conversions
	constexpr inline double NullToNumber() { return 0.0; }
	constexpr inline int64_t NullToInteger() { return 0ll; }
	constexpr inline bool NullToBoolean() { return false; }
	inline String NullToString() { return String("null"); }

	// Number conversions
	inline int64_t NumberToInteger(double value) { return static_cast<int64_t>(value); }
	inline bool NumberToBoolean(double value) { return value != 0.0f ? true : false; }
	inline String NumberToString(double value)
	{
		char buffer[32];
		snprintf(buffer, 32, "%.6f", value);
		return String(buffer);
	}

	// Integer conversions
	inline double IntegerToNumber(int64_t value) { return static_cast<double>(value); }
	inline bool IntegerToBoolean(int64_t value) { return value != 0 ? true : false; }
	inline String IntegerToString(int64_t value)
	{
		char buffer[32];
		snprintf(buffer, 32, "%" PRId64, value);
		return String(buffer);
	}

	// Boolean conversions
	inline double BooleanToNumber(bool value) { return value ? 1.0 : 0.0; }
	inline int64_t BooleanToInteger(bool value) { return value ? 1ll : 0ll; }
	inline String BooleanToString(bool value) { return value ? String("true") : String("false"); }

	enum class NumericFormat
	{
		International, // Uses dot as decimal indicator
		Continental    // Uses comma as decimal indicator
	};

	// String conversions
	bool StringToBoolean(const String & inValue, bool * outValue);
	bool StringToNumber(const String & value, double * outValue, NumericFormat format = NumericFormat::International);
	bool StringToInteger(const String & value, int64_t * outValue);
	bool StringToValueType(const String & value, ValueType * outValue);
	bool StringToGuid(const String & value, Guid * outValue);
	bool StringToCollection(const String & value, CollectionPtr * outValue);

	// GUID conversions
	String GuidToString(const Guid & value);

} // namespace Jinx::Impl

#endif // JX_CONVERSION_H__



// end --- JxConversion.h --- 



// begin --- JxSerialize.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_SERIALIZE_H__
#define JX_SERIALIZE_H__


namespace Jinx
{

	class BinaryReader
	{
	public:
		explicit BinaryReader(BufferPtr buffer) :
			m_buffer(buffer),
			m_pos(0)
		{}
		BinaryReader(const BinaryReader & other) :
			m_buffer(other.m_buffer),
			m_pos(other.m_pos)
		{}

		inline void Read(bool * val) { uint8_t b = false; m_buffer->Read(&m_pos, &b, sizeof(uint8_t)); *val = b ? true : false; }
		inline void Read(char * val) { m_buffer->Read(&m_pos, val, sizeof(char)); }
		inline void Read(int8_t * val) { m_buffer->Read(&m_pos, val, sizeof(int8_t)); }
		inline void Read(int16_t * val) { m_buffer->Read(&m_pos, val, sizeof(int16_t)); }
		inline void Read(int32_t * val) { m_buffer->Read(&m_pos, val, sizeof(int32_t)); }
		inline void Read(int64_t * val) { m_buffer->Read(&m_pos, val, sizeof(int64_t)); }
		inline void Read(uint8_t * val) { m_buffer->Read(&m_pos, val, sizeof(uint8_t)); }
		inline void Read(uint16_t * val) { m_buffer->Read(&m_pos, val, sizeof(uint16_t)); }
		inline void Read(uint32_t * val) { m_buffer->Read(&m_pos, val, sizeof(uint32_t)); }
		inline void Read(uint64_t * val) { m_buffer->Read(&m_pos, val, sizeof(uint64_t)); }
		inline void Read(float * val) { m_buffer->Read(&m_pos, val, sizeof(float)); }
		inline void Read(double * val) { m_buffer->Read(&m_pos, val, sizeof(double)); }

		void Read(String * val);
		void Read(BufferPtr & val);

		void Read(void * val, size_t bytes) { m_buffer->Read(&m_pos, val, bytes); }

		// Useful for reading generic data (like enums) whose type is different than the raw serialized data
		template<typename T0, typename T1>
		void Read(T0 * val)
		{
			T1 v;
			Read(&v);
			*val = static_cast<T0>(v);
		}

		size_t Tell() const { return m_pos; }
		void Seek(size_t pos) { m_pos = pos; }
		size_t Size() const { return m_buffer->Size(); }

	private:
		friend class BinaryWriter;
		BufferPtr m_buffer;
		size_t m_pos;
	};

	class BinaryWriter
	{
	public:
		BinaryWriter(BufferPtr buffer) :
			m_buffer(buffer),
			m_pos(0)
		{}

		inline void Write(bool val) { uint8_t b = val ? 1 : 0; m_buffer->Write(&m_pos, &b, sizeof(uint8_t)); }
		inline void Write(char val) { m_buffer->Write(&m_pos, &val, sizeof(char)); }
		inline void Write(int8_t val) { m_buffer->Write(&m_pos, &val, sizeof(int8_t)); }
		inline void Write(int16_t val) { m_buffer->Write(&m_pos, &val, sizeof(int16_t)); }
		inline void Write(int32_t val) { m_buffer->Write(&m_pos, &val, sizeof(int32_t)); }
		inline void Write(int64_t val) { m_buffer->Write(&m_pos, &val, sizeof(int64_t)); }
		inline void Write(uint8_t val) { m_buffer->Write(&m_pos, &val, sizeof(uint8_t)); }
		inline void Write(uint16_t val) { m_buffer->Write(&m_pos, &val, sizeof(uint16_t)); }
		inline void Write(uint32_t val) { m_buffer->Write(&m_pos, &val, sizeof(uint32_t)); }
		inline void Write(uint64_t val) { m_buffer->Write(&m_pos, &val, sizeof(uint64_t)); }
		inline void Write(float val) { m_buffer->Write(&m_pos, &val, sizeof(float)); }
		inline void Write(double val) { m_buffer->Write(&m_pos, &val, sizeof(double)); }

		void Write(const String & val);
		void Write(const BufferPtr & val);
		void Write(BinaryReader & reader, size_t bytes);
		void Write(const void * val, size_t bytes) { m_buffer->Write(&m_pos, val, bytes); }

		// Useful for writing generic data (like enums) whose type is different than the raw serialized data
		template<typename T0, typename T1>
		void Write(T0 val)
		{
			T1 v = static_cast<T1>(val);
			Write(v);
		}

		size_t Tell() const { return m_pos; }
		void Seek(size_t pos) { m_pos = pos; }
		size_t Size() const { return m_buffer->Size(); }

	private:

		BufferPtr m_buffer;
		size_t m_pos;
	};

} // namespace Jinx

#endif // JX_SERIALIZE_H__


// end --- JxSerialize.h --- 



// begin --- JxPropertyName.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_PROPERTY_H__
#define JX_PROPERTY_H__


namespace Jinx::Impl
{

	class PropertyName
	{
	public:
		PropertyName();
		PropertyName(VisibilityType visibility, bool readOnly, const String & moduleName, const String & propertyName, const Variant & defaultValue = nullptr);

		bool IsValid() const { return m_visibility != VisibilityType::Local; }
		bool IsReadOnly() const { return m_readOnly; }
		RuntimeID GetId() const { return m_id; }
		const String & GetName() const { return m_name; }
		VisibilityType GetVisibility() const { return m_visibility; }
		size_t GetPartCount() const { return m_partCount; }
		const Variant & GetDefaultValue() const { return m_defaultValue; }

		// Serialization
		void Read(BinaryReader & reader);
		void Write(BinaryWriter & writer) const;

	private:

		RuntimeID m_id = 0;
		VisibilityType m_visibility = VisibilityType::Local;
		bool m_readOnly = false;
		String m_name;
		Variant m_defaultValue;
		size_t m_partCount = 0;
	};

} // namespace Jinx::Impl

#endif // JX_PROPERTY_H__



// end --- JxPropertyName.h --- 



// begin --- JxLexer.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_LEXER_H__
#define JX_LEXER_H__


namespace Jinx::Impl
{

	struct Symbol
	{
		Symbol() :
			Symbol(SymbolType::None, 0, 0)
		{}
		Symbol(SymbolType t, int32_t ln, int32_t cn) :
			type(t),
			numVal(0),
			lineNumber(ln),
			columnNumber(cn)
		{}
		SymbolType type;
		String text;
		union
		{
			double numVal;
			int64_t intVal;
			bool boolVal;
		};
		uint32_t lineNumber;
		uint32_t columnNumber;
	};

	using SymbolList = std::list<Symbol, Allocator<Symbol>>;
	using SymbolListCItr = SymbolList::const_iterator;

	class Lexer
	{
	public:
		// Lex the script text
		Lexer(const SymbolTypeMap & symbolTypeMap, const char * start, const char * end, const String & name);

		// Do lexing pass to create token list
		bool Execute();

		// Retrieve the finished symbol list
		const SymbolList & GetSymbolList() const { return m_symbolList; }

	private:

		// Log an error
		template<class... Args>
		void Error(const char * format, Args&&... args)
		{
			if (m_error)
				return;
			if (m_name.empty())
				LogWrite(LogLevel::Error, "Error at line %i, column %i:", m_lineNumber, m_columnNumber);
			else
				LogWrite(LogLevel::Error, "Error in '%s' at line %i, column %i: ", m_name.c_str(), m_lineNumber, m_columnNumber);
			LogWriteLine(LogLevel::Error, format, std::forward<Args>(args)...);
			m_error = true;
		}

		// Create a new symbol
		void CreateSymbol(SymbolType type);
		void CreateSymbol(double number);
		void CreateSymbol(int64_t integer);
		void CreateSymbol(const String & name);
		void CreateSymbolString(String && text);

		// Character queries
		inline bool IsEndOfText() const { return (!(*m_current) || m_current > m_end) ? true : false; }
		bool IsName(const char * ptr) const;
		bool IsNameStart(const char * ptr) const;
		inline bool IsNewline(unsigned char c) const { return (c == '\r' || c == '\n') ? true : false; }
		inline bool IsWhitespace(unsigned char c) const { return (c == ' ' || c == '\t') ? true : false; }
		bool IsNextCharacter(unsigned char c) const;
		bool IsNextDigit() const;
		bool IsNumber(unsigned char c) const { return std::isdigit(c) != 0 || c == '.'; }
		bool IsNumberStart(unsigned char c) const { return std::isdigit(c) != 0 || c == '.' || c == '-'; }

		// Text parsing functions
		void AdvanceCurrent();
		void ParseEndOfLine();
		void ParseComment();
		void ParseEllipse();
		void ParseName();
		void ParseNumber();
		void ParseString();
		void ParseWhitespace();
		void ParseWhitespaceAndNewlines();

	private:
		String m_name;
		SymbolList m_symbolList;
		const char * m_start;
		const char * m_end;
		const char * m_current;
		uint32_t m_columnNumber;
		uint32_t m_columnMarker;
		uint32_t m_lineNumber;
		bool m_error;
		const SymbolTypeMap & m_symbolTypeMap;
	};

} // namespace Jinx::Impl

#endif // JX_LEXER_H__


// end --- JxLexer.h --- 



// begin --- JxHash.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_HASH_H__
#define JX_HASH_H__


namespace Jinx::Impl
{

	uint64_t GetHash(const void * data, size_t len);

} // namespace Jinx::Impl

#endif // JX_HASH_H__




// end --- JxHash.h --- 



// begin --- JxFunctionSignature.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_FUNCTION_SIGNATURE_H__
#define JX_FUNCTION_SIGNATURE_H__


namespace Jinx::Impl
{

	enum class FunctionSignaturePartType
	{
		Name,
		Parameter,
	};

	struct FunctionSignaturePart
	{
		FunctionSignaturePart() :
			partType(FunctionSignaturePartType::Name),
			optional(false),
			valueType(ValueType::Any)
		{}
		FunctionSignaturePartType partType;
		bool optional;
		ValueType valueType;
		std::vector<String, Allocator<String>> names;
	};

	using FunctionSignatureParts = std::vector<FunctionSignaturePart, Allocator<FunctionSignaturePart>>;

	// Function and member function signature object.
	class FunctionSignature
	{
	public:
		FunctionSignature();
		FunctionSignature(VisibilityType visibility, const String & libraryName, const FunctionSignatureParts & parts);

		// Get unique function id
		RuntimeID GetId() const { return m_id; }

		// Get human-readable name for debug purposes
		String GetName() const;

		// Get signature length
		size_t GetLength() const { return m_parts.size(); }

		// Get visibility level
		VisibilityType GetVisibility() const { return m_visibility; }

		// Get signature parts
		const FunctionSignatureParts & GetParts() const { return m_parts; }

		// Is this a valid signature?
		inline bool IsValid() const { return !m_parts.empty(); }

		// Get a list of parameter parts
		FunctionSignatureParts GetParameters() const;

		// Get number of parameters
		size_t GetParameterCount() const;

		// Serialization
		void Read(BinaryReader & reader);
		void Write(BinaryWriter & writer) const;

	private:

		friend bool operator == (const FunctionSignature & left, const FunctionSignature & right);

	private:

		// Unique id
		RuntimeID m_id = 0;

		// Visibility level
		VisibilityType m_visibility = VisibilityType::Local;

		// Library name
		String m_libraryName;

		// Each signature is made up of any number of parts representing either part
		// of the function name or a variable placeholder.
		FunctionSignatureParts m_parts;

	};

	bool operator == (const FunctionSignaturePart & left, const FunctionSignaturePart & right);
	bool operator == (const FunctionSignature & left, const FunctionSignature & right);

	using FunctionList = std::list<FunctionSignature, Allocator<FunctionSignature>>;
	using  FunctionPtrList = std::vector<const FunctionSignature*, Allocator<const FunctionSignature*>>;

} // namespace Jinx::Impl

#endif // JX_FUNCTION_SIGNATURE_H__



// end --- JxFunctionSignature.h --- 



// begin --- JxFunctionDefinition.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_FUNCTION_DEFINITION_H__
#define JX_FUNCTION_DEFINITION_H__


namespace Jinx::Impl
{

	class FunctionDefinition
	{
	public:
		FunctionDefinition(const FunctionSignature & signature, BufferPtr bytecode, size_t offset) :
			m_id(signature.GetId()),
			m_parameterCount(signature.GetParameterCount()),
            m_bytecode(bytecode),
            m_offset(offset),
            m_name(signature.GetName())
		{}
		FunctionDefinition(const FunctionSignature & signature, FunctionCallback callback) :
			m_id(signature.GetId()),
			m_parameterCount(signature.GetParameterCount()),
			m_bytecode(nullptr),
			m_offset(0),
			m_name(signature.GetName()),
			m_callback(callback)
		{}
		size_t GetParameterCount() const { return m_parameterCount; }
		RuntimeID GetId() const { return m_id; }
		const BufferPtr & GetBytecode() const { return m_bytecode; }
		size_t GetOffset() const { return m_offset; }
		FunctionCallback GetCallback() const { return m_callback; }
		const char * GetName() const { return m_name.c_str(); }
		friend class FunctionTable;

	private:
		RuntimeID m_id;
		size_t m_parameterCount;
		BufferPtr m_bytecode;
		size_t m_offset;
		String m_name;
		FunctionCallback m_callback;
	};
	
	using FunctionDefinitionPtr = std::shared_ptr<FunctionDefinition>;

} // Jinx::Impl

#endif // JX_FUNCTION_DEFINITION_H__



// end --- JxFunctionDefinition.h --- 



// begin --- JxLibrary.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_LIBRARY_H__
#define JX_LIBRARY_H__


namespace Jinx::Impl
{

	class Library : public ILibrary
	{
	public:
		Library(RuntimeWPtr runtime, const String & name);

		// ILibrary interface
		bool RegisterFunction(Visibility visibility, const String & name, FunctionCallback function) override;
		bool RegisterProperty(Visibility visibility, Access access, const String & name, const Variant & value) override;
		Variant GetProperty(const String & name) const override;
		void SetProperty(const String & name, const Variant & value) override;

		// Internal name getter
		const String & GetName() const { return m_name; }

		// Internal property functions
		bool RegisterPropertyName(const PropertyName & propertyName, bool checkForDuplicates);
		bool PropertyNameExists(const String & name) const;
		PropertyName GetPropertyName(const String & name);
		size_t GetMaxPropertyParts() const { return m_maxPropertyParts; }

		// Internal function signature functions
		void RegisterFunctionSignature(const FunctionSignature & signature);
		bool FunctionSignatureExists(const FunctionSignature & signature) const;
		FunctionSignature FindFunctionSignature(Visibility visibility, const String & name) const;
		const FunctionPtrList Functions() const;

	private:

		// Create a function signature from a string
		FunctionSignature CreateFunctionSignature(Visibility visibility, const String & name) const;

		// Private internal functions
		bool RegisterPropertyNameInternal(const PropertyName & propertyName, bool checkForDuplicates);

		using PropertyNameTable = std::map <String, PropertyName, std::less<String>, Allocator<std::pair<const String, PropertyName>>>;

		// Library name
		String m_name;

		// Track function definitions
		mutable std::mutex m_functionMutex;
		FunctionList m_functionList;

		// Properties
		mutable std::mutex m_propertyMutex;
		PropertyNameTable m_propertyNameTable;
		size_t m_maxPropertyParts;

		// Weak ptr to runtime system
		RuntimeWPtr m_runtime;
	};

	using LibraryIPtr = std::shared_ptr<Library>;

} // namespace Jinx::Impl

#endif // JX_LIBRARY_H__


// end --- JxLibrary.h --- 



// begin --- JxVariableStackFrame.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_VARIABLE_STACK_FRAME_H__
#define JX_VARIABLE_STACK_FRAME_H__

namespace Jinx::Impl
{

	class VariableStackFrame
	{
	public:
		VariableStackFrame();

		// Retrieve the max variable parts for the current frame
		size_t GetMaxVariableParts() const;

		// Set max variable parts for the current frame
		void SetMaxVariableParts(size_t varParts);

		// Assign a variable or check that it exists
		bool VariableAssign(const String & name);
		bool VariableExists(const String & name) const;

		// Get the stack depth given a variable name
		size_t GetStackDepthFromName(const String & name) const;

		// Begin or end a new execution frame, like with a function
		void FrameBegin();
		bool FrameEnd();

		// Begin or end current execution scope
		bool ScopeBegin();
		bool ScopeEnd();

		// Are we at the lowest frame level within the current frame (i.e. not in a scoped execution block)?
		bool IsRootScope() const;

		// Are we in the original execution frame (i.e. not in a function)?
		bool IsRootFrame() const;

		// Get last error message
		const char * GetErrorMessage() const { return m_errorMessage.c_str(); }

	private:

		void CalculateMaxVariableParts();

		using VariableSet = std::set<String, std::less<String>, Allocator<String>>;
		using VariableStack = std::vector<VariableSet, Allocator<VariableSet>>;
		struct FrameData
		{
			FrameData() : maxVariableParts(0) {}
			VariableStack stack;
			size_t maxVariableParts;
		};
		using VariableFrames = std::vector<FrameData, Allocator<FrameData>>;
		VariableFrames m_frames;
		String m_errorMessage;
	};

} // namespace Jinx::Impl

#endif // JX_VARIABLE_STACK_FRAME_H__


// end --- JxVariableStackFrame.h --- 



// begin --- JxParser.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_PARSER_H__
#define JX_PARSER_H__


namespace Jinx::Impl
{

	class Parser
	{
	public:
		Parser(RuntimeIPtr runtime, const SymbolList &symbolList, const String & name, std::initializer_list<String> libraries);
		Parser(RuntimeIPtr runtime, const SymbolList &symbolList, const String & name);

		// Convert the symbol list into bytecode
		bool Execute();

		// Parse and create a function signature from symbol list and library name
		FunctionSignature ParseFunctionSignature(VisibilityType access, const String & libraryName);

		// Retrieve the generated bytecode
		BufferPtr GetBytecode() const { return m_bytecode; }

		// Retrieve a debug-friendly name from an ID.
		String GetNameFromID(RuntimeID id) const;

	private:

		using FunctionPartData = std::vector<std::tuple<FunctionSignaturePartType, size_t, bool>, Allocator<std::tuple<FunctionSignaturePartType, size_t, bool>>>;

		// Structure returned from function finding algorithm
		struct FunctionMatch
		{
			FunctionMatch() : signature(nullptr) {}
			const FunctionSignature * signature;
			FunctionPartData partData;
		};

		// Log an error
		template<class... Args>
		void Error(const char * format, Args&&... args)
		{
			if (m_error)
				return;
			m_error = true;
			if (m_name.empty())
				LogWrite(LogLevel::Error, "Error at ");
			else
				LogWrite(LogLevel::Error, "Error in %s at ", m_name.c_str());
			if (m_currentSymbol == m_symbolList.end())
				LogWrite(LogLevel::Error, "end of script: ");
			else
				LogWrite(LogLevel::Error, "line %i, column %i: ", m_currentSymbol->lineNumber, m_currentSymbol->columnNumber);
			LogWriteLine(LogLevel::Error, format, std::forward<Args>(args)...);
		}

		// Hash and register variable or property name and ID mapping
		RuntimeID VariableNameToRuntimeID(const String & name);

		// Assign a variable or check that it exists
		void VariableAssign(const String & name);
		bool VariableExists(const String & name) const;

		// Begin or end a new execution frame, like with a function
		void FrameBegin();
		void FrameEnd();

		// Begin or end current execution scope
		void ScopeBegin();
		void ScopeEnd();

		// Retrieve a precedence value for the specified operator
		uint32_t GetOperatorPrecedence(Opcode opcode) const;

		// Check to see if the symbol is a newline or at the end of the list
		bool IsSymbolValid(SymbolListCItr symbol) const;

		// Check a string to see if it's a library name
		bool IsLibraryName(const String & name) const;

		// Check to see if this is a property name
		bool IsPropertyName(const String & libraryName, const String & propertyName) const;

		// Emit functions write to internal bytecode buffer
		void EmitAddress(size_t address);
		size_t EmitAddressPlaceholder();
		void EmitAddressBackfill(size_t address);
		void EmitCount(uint32_t count);
		void EmitName(const String & name);
		void EmitOpcode(Opcode opcode);
		void EmitValue(const Variant & value);
		void EmitId(RuntimeID id);
		void EmitIndex(int32_t index);
		void EmitValueType(ValueType type);

		// Finalize bytecode header with final size
		void WriteBytecodeHeader();

		// Write optional debug info
		void WriteDebugInfo();

		// Advance to next sumbol
		void NextSymbol();

		// Returns false and flags an error if the current symbol does not match param.  NextSymbol() is
		// called and true is returned on success.
		bool Expect(SymbolType symbol);

		// If the current symbol matches the parameter, NextSymbol() is called and the function returns true.
		bool Accept(SymbolType symbol);

		// Check for existance of the specified symbol type or content at the current position.  
		// The state of the parser is guaranteed not to be altered.  Returns true or non-null on success.
		bool Check(SymbolType symbol) const;
		bool CheckBinaryOperator() const;
		bool CheckName() const;
		bool CheckValue() const;
		bool CheckValueType() const;
		bool CheckFunctionNamePart() const;
		bool CheckVariable(SymbolListCItr currSym, size_t * symCount = nullptr) const;
		bool CheckVariable() const;
		bool CheckProperty(SymbolListCItr currSym, size_t * symCount = nullptr) const;
		bool CheckProperty(size_t * symCount = nullptr) const;
		bool CheckPropertyName(LibraryIPtr library, SymbolListCItr currSym, size_t * symCount) const;
		String CheckLibraryName() const;
		bool CheckFunctionCallPart(const FunctionSignatureParts & parts, size_t partsIndex, SymbolListCItr currSym, SymbolListCItr endSym, FunctionMatch & match) const;
		FunctionMatch CheckFunctionCall(const FunctionSignature & signature, SymbolListCItr currSym, SymbolListCItr endSym, bool skipInitialParam) const;
		FunctionMatch CheckFunctionCall(const FunctionList & functionList, SymbolListCItr currSym, SymbolListCItr endSym, bool skipInitialParam) const;
		FunctionMatch CheckFunctionCall(LibraryIPtr library, SymbolListCItr currSym, SymbolListCItr endSym, bool skipInitialParam) const;
		FunctionMatch CheckFunctionCall(bool skipInitialParam, SymbolListCItr currSym, SymbolListCItr endSym) const;
		FunctionMatch CheckFunctionCall(bool skipInitialParam, SymbolListCItr endSym) const;
		FunctionMatch CheckFunctionCall() const;

		// Parsing functions advance the current symbol, looking for a pattern of symbols
		// and injecting the compiled results into the bytecode buffer.
		VisibilityType ParseScope();
		Opcode ParseBinaryOperator();
		Variant ParseValue();
		ValueType ParseValueType();
		String ParseName();
		String ParseMultiName(std::initializer_list<SymbolType> symbols);
		String ParseVariable();
		void ParseSubscriptGet();
		uint32_t ParseSubscriptSet();
		void ParsePropertyDeclaration(VisibilityType scope, bool readOnly);
		PropertyName ParsePropertyName();
		PropertyName ParsePropertyNameParts(LibraryIPtr library);
		String ParseFunctionNamePart();
		FunctionSignature ParseFunctionSignature(VisibilityType access, bool signatureOnly = true);
		void ParseFunctionDefinition(VisibilityType scope);
		void ParseFunctionCall(const FunctionMatch & match);
		void ParseCast();
		void ParseSubexpressionOperand(bool required, SymbolListCItr endSymbol);
		void ParseSubexpression(SymbolListCItr endSymbol);
		void ParseSubexpression();
		void ParseExpression(SymbolListCItr endSymbol);
		void ParseExpression();
		void ParseErase();
		void ParseIncDec();
		void ParseIfElse();
		void ParseLoop();
		bool ParseStatement();
		void ParseBlock();
		void ParseLibraryImports();
		void ParseLibraryDeclaration();
		void ParseScript();

	private:
		using IDNameMap = std::map <RuntimeID, String, std::less<RuntimeID>, Allocator<std::pair<const RuntimeID, String>>>;

		// Runtime object
		RuntimeIPtr m_runtime;

		// Unique name
		String m_name;

		// Symbol list to parse
		const SymbolList & m_symbolList;

		// Current symbol being parsed
		SymbolListCItr m_currentSymbol;

		// Last parsed line
		uint32_t m_lastLine;

		// Signal an error
		bool m_error;

		// Break jump backfill address
		size_t m_breakAddress;

		// Bytecode data buffer
		BufferPtr m_bytecode;

		// Writes data to an output buffer
		BinaryWriter m_writer;

		// Write opcode debug data
		std::vector<DebugLineEntry, Allocator<DebugLineEntry>> m_debugLines;

		// Current library;
		LibraryIPtr m_library;

		// Local function definitions
		FunctionList m_localFunctions;

		// Library import list
		std::list<String, Allocator<String>> m_importList;

		// Keep track of variables currently in scope
		VariableStackFrame m_variableStackFrame;

		// ID to name mapping for debug output
		IDNameMap m_idNameMap;
	};

} // namespace Jinx::Impl

#endif // JX_PARSER_H__


// end --- JxParser.h --- 



// begin --- JxScript.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_SCRIPT_H__
#define JX_SCRIPT_H__


namespace Jinx::Impl
{

	class Script : public IScript, public std::enable_shared_from_this<Script>
	{
	public:
		Script(RuntimeIPtr runtime, BufferPtr bytecode, Any userContext);
		virtual ~Script();

		RuntimeID FindFunction(LibraryPtr library, const String & name) override;
		Variant CallFunction(RuntimeID id, Parameters params) override;

		bool Execute() override;
		bool IsFinished() const override;

		Variant GetVariable(const String & name) const override;
		void SetVariable(const String & name, const Variant & value) override;

		const String & GetName() const override { return m_name; }
		Any GetUserContext() const override { return m_userContext; }
		LibraryPtr GetLibrary() const override { return m_library; }

		std::vector<String, Allocator<String>> GetCallStack() const;

	private:
		void Error(const char * message);

		Variant GetVariable(RuntimeID id) const;
		Variant Pop();
		void Push(const Variant & value);
		void SetVariableAtIndex(RuntimeID id, size_t index);
		void SetVariable(RuntimeID id, const Variant & value);

		std::pair<CollectionPtr, Variant> WalkSubscripts(uint32_t subscripts, CollectionPtr collection);

		Variant CallFunction(RuntimeID id);

	private:
		using IdIndexMap = std::map<RuntimeID, size_t, std::less<RuntimeID>, Allocator<std::pair<const RuntimeID, size_t>>>;
		using ScopeStack = std::vector<size_t, Allocator<size_t>>;
		using FunctionMap = std::map<RuntimeID, FunctionDefinitionPtr, std::less<RuntimeID>, Allocator<std::pair<const RuntimeID, FunctionDefinitionPtr>>>;

		// Pointer to runtime object
		RuntimeIPtr m_runtime;

		// Execution frame allows jumping to remote code (function calls) and returning
		struct ExecutionFrame
		{
			ExecutionFrame(BufferPtr b, const char * n) : bytecode(b), reader(b), name(n)
			{
				scopeStack.reserve(32);
			}
			explicit ExecutionFrame(FunctionDefinitionPtr fn) : ExecutionFrame(fn->GetBytecode(), fn->GetName()) {}

			// Buffer containing script bytecode
			BufferPtr bytecode;

			// Binary reader - sequentially extracts data from bytecode buffer.  The reader's
			// current internal position acts as the current frame's instruction pointer.
			BinaryReader reader;

			// Function definition name.  Note that storing a raw string pointer is reasonably safe, 
			// because shared pointers to other objects are referenced in this struct, and there is 
			// no other way to modify the containing string.  I don't want to incur the expense of a 
			// safer string copy, which would cause an allocation cost for each function call.
			const char * name;

			// Variable id lookup map
			IdIndexMap ids;

			// Track top of stack for each level of scope
			ScopeStack scopeStack;

			// Top of the stack to clear to when this frame is popped
			size_t stackTop = 0;

			// Stop execution at the end of this frame
			bool waitOnReturn = false;
		};

		// Execution frame stack
		std::vector<ExecutionFrame, Allocator<ExecutionFrame>> m_execution;

		// Runtime stack
		std::vector<Variant, Allocator<Variant>> m_stack;

		// Current library
		LibraryIPtr m_library;

		// User context pointer
		Any m_userContext;

		// Initial position of bytecode for this script
		size_t m_bytecodeStart;

		// Is finished executing
		bool m_finished;

		// Runtime error
		bool m_error;

		// Script name
		String m_name;
	};

	using ScriptIPtr = std::shared_ptr<Script>;

} // namespace Jinx::Impl

#endif // JX_SCRIPT_H__


// end --- JxScript.h --- 



// begin --- JxRuntime.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_RUNTIME_H__
#define JX_RUNTIME_H__


namespace Jinx::Impl
{

	class Runtime : public IRuntime, public std::enable_shared_from_this<Runtime>
	{
	public:
		Runtime();
		virtual ~Runtime();

		// IRuntime interface
		BufferPtr Compile(const char * scriptText, String name, std::initializer_list<String> libraries) override;
		ScriptPtr CreateScript(BufferPtr bytecode, Any userContext) override;
		ScriptPtr CreateScript(const char * scriptText, Any userContext, String name, std::initializer_list<String> libraries) override;
		ScriptPtr ExecuteScript(const char * scriptText, Any userContext, String name, std::initializer_list<String> libraries) override;
		LibraryPtr GetLibrary(const String & name) override;
		PerformanceStats GetScriptPerformanceStats(bool resetStats = true) override;
		BufferPtr StripDebugInfo(BufferPtr bytecode) const override;

		// Internal interface
		BufferPtr Compile(BufferPtr scriptBuffer, String name, std::initializer_list<String> libraries);
		inline LibraryIPtr GetLibraryInternal(const String & name) { return std::static_pointer_cast<Library>(GetLibrary(name)); }
		FunctionDefinitionPtr FindFunction(RuntimeID id) const;
		bool LibraryExists(const String & name) const;
		void RegisterFunction(const FunctionSignature & signature, BufferPtr bytecode, size_t offset);
		void RegisterFunction(const FunctionSignature & signature, FunctionCallback function);
		Variant GetProperty(RuntimeID id) const;
		bool PropertyExists(RuntimeID id) const;
		void SetProperty(RuntimeID id, std::function<void(Variant &)> fn);
		void SetProperty(RuntimeID id, const Variant & value);
		void AddPerformanceParams(bool finished, uint64_t timeNs, uint64_t instCount);
		const SymbolTypeMap & GetSymbolTypeMap() const { return m_symbolTypeMap; }

	private:

		using LibraryMap = std::map<String, LibraryIPtr, std::less<String>, Allocator<std::pair<const String, LibraryIPtr>>>;
		using FunctionMap = std::map<RuntimeID, FunctionDefinitionPtr, std::less<RuntimeID>, Allocator<std::pair<const RuntimeID, FunctionDefinitionPtr>>>;
		using PropertyMap = std::map<RuntimeID, Variant, std::less<RuntimeID>, Allocator<std::pair<const RuntimeID, Variant>>>;
		void LogBytecode(const Parser & parser) const;
		void LogSymbols(const SymbolList & symbolList) const;

	private:

		static const size_t NumMutexes = 8;
		mutable std::mutex m_libraryMutex;
		LibraryMap m_libraryMap;
		mutable std::mutex m_functionMutex[NumMutexes];
		FunctionMap m_functionMap;
		mutable std::mutex m_propertyMutex[NumMutexes];
		PropertyMap m_propertyMap;
		std::mutex m_perfMutex;
		PerformanceStats m_perfStats;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_perfStartTime;
		SymbolTypeMap m_symbolTypeMap;
	};

} // namespace Jinx::Impl

#endif // JX_RUNTIME_H__


// end --- JxRuntime.h --- 



// begin --- JxLibCore.h --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_LIB_CORE_H__
#define JX_LIB_CORE_H__

namespace Jinx::Impl
{
	void RegisterLibCore(RuntimePtr runtime);

} // namespace Jinx::Impl




#endif // JX_LIB_CORE_H__



// end --- JxLibCore.h --- 



#endif // JX_INTERNAL_H__


// end --- JxInternal.h --- 



namespace Jinx
{
	inline Buffer::Buffer()
		: m_data(nullptr), m_size(0), m_capacity(0)
	{
	}

	inline Buffer::~Buffer()
	{
		JinxFree(m_data);
	}

	inline size_t Buffer::Capacity() const
	{
		return m_capacity;
	}

	inline void Buffer::Clear()
	{
		m_size = 0;
	}

	inline void Buffer::Read(size_t * pos, void * data, size_t bytes)
	{
		assert(*pos < m_size);
		assert(bytes <= (m_size - *pos));
		size_t bytesToCopy = std::min(bytes, m_size - *pos);
		assert(bytesToCopy);
		assert(bytesToCopy == bytes);
		memcpy(data, m_data + *pos, bytesToCopy);
		*pos += bytes;
	}

	inline void Buffer::Read(size_t * pos, BufferPtr & buffer, size_t bytes)
	{
		assert(*pos < m_size);
		assert(bytes <= (m_size - *pos));
		size_t bytesToCopy = std::min(bytes, m_size - *pos);
		assert(bytesToCopy);
		assert(bytesToCopy == bytes);
		buffer->Reserve(bytesToCopy);
		memcpy(buffer->m_data, m_data + *pos, bytesToCopy);
		*pos += bytes;
		buffer->m_size = bytesToCopy;
	}

	inline void Buffer::Reserve(size_t size)
	{
		if (m_data)
		{
			if (size <= m_capacity)
				return;
			uint8_t * newData = (uint8_t *)JinxAlloc(size);
			memcpy(newData, m_data, m_size);
			JinxFree(m_data);
			m_data = newData;
			m_capacity = size;
		}
		else
		{
			m_data = (uint8_t *)JinxRealloc(m_data, size);
			m_capacity = size;
		}
	}

	inline void Buffer::Write(const void * data, size_t bytes)
	{
		assert(data && bytes);
		if (m_capacity < bytes)
			Reserve((m_capacity + bytes) + (m_capacity / 2));
		memcpy(m_data, data, bytes);
		m_size = bytes;
	}

	inline void Buffer::Write(size_t * pos, const void * data, size_t bytes)
	{
		assert(*pos <= m_size);
		assert(data && bytes);
		if (m_capacity < (*pos + bytes))
			Reserve((m_capacity + bytes) + (m_capacity / 2));
		memcpy(m_data + *pos, data, bytes);
		*pos += bytes;
		if (m_size < *pos)
			m_size = *pos;
	}

	inline BufferPtr CreateBuffer()
	{
		return std::allocate_shared<Buffer>(Allocator<Buffer>());
	}

} // namespace Jinx


// end --- JxBuffer.cpp --- 



// begin --- JxCollection.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/


namespace Jinx
{
	inline CollectionPtr CreateCollection()
	{
		return std::allocate_shared<Collection>(Allocator<Collection>());
	}

} // namespace Jinx



// end --- JxCollection.cpp --- 



// begin --- JxCommon.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

namespace Jinx
{

	namespace Impl
	{
		struct HashData
		{
			char hs[16];
			uint64_t h1;
			uint64_t h2;
		};

		struct CommonData
		{
			static inline GlobalParams globalParams;
			static inline std::atomic<uint64_t> uniqueId = { 0 };
			static inline const char * opcodeName[] =
			{
				"add",
				"and",
				"callfunc",
				"cast",
				"decrement",
				"divide",
				"equals",
				"eraseitr",
				"erasepropkeyval",
				"erasevarkeyval",
				"exit",
				"function",
				"greater",
				"greatereq",
				"increment",
				"jump",
				"jumpfalse",
				"jumpfalsecheck",
				"jumptrue",
				"jumptruecheck",
				"less",
				"lesseq",
				"library",
				"loopcount",
				"loopover",
				"mod",
				"multiply",
				"negate",
				"not",
				"notequals",
				"or",
				"pop",
				"popcount",
				"property",
				"pushcoll",
				"pushitr",
				"pushkeyval",
				"pushlist",
				"pushprop",
				"pushtop",
				"pushvar",
				"pushval",
				"return",
				"scopebegin",
				"scopeend",
				"setindex",
				"setprop",
				"setpropkeyval",
				"setvar",
				"setvarkeyval",
				"subtract",
				"type",
				"wait",
			};
			
			static_assert(countof(opcodeName) == static_cast<size_t>(Opcode::NumOpcodes), "Opcode descriptions don't match enum count");

			static inline const char * symbolTypeName[] =
			{
				"none",
				"invalid",
				"newline",
				"name value",
				"string value",
				"number value",
				"integer value",
				"boolean value",
				"/",
				"*",
				"+",
				"-",
				"=",
				"!=",
				"%",
				",",
				"(",
				")",
				"{",
				"}",
				"[",
				"]",
				"...",
				"'",
				"<",
				"<=",
				">",
				">=",
				"and",
				"as",
				"begin",
				"boolean",
				"break",
				"by",
				"collection",
				"decrement",
				"else",
				"end",
				"erase",
				"external",
				"from",
				"function",
				"guid",
				"if",
				"import",
				"increment",
				"integer",
				"is",
				"library",
				"loop",
				"not",
				"null",
				"number",
				"object",
				"or",
				"over",
				"private",
				"public",
				"readonly",
				"return",
				"set",
				"string",
				"to",
				"type",
				"until",
				"wait",
				"while",
			};

			static_assert(countof(symbolTypeName) == static_cast<size_t>(SymbolType::NumSymbols), "SymbolType descriptions don't match enum count");

			static inline const char * valueTypeName[] =
			{
				"null",
				"number",
				"integer",
				"boolean",
				"string",
				"collection",
				"collectionitr",
				"userobject",
				"buffer",
				"guid",
				"valtype",
				"any",
			};

			static_assert(countof(valueTypeName) == (static_cast<size_t>(ValueType::NumValueTypes) + 1), "ValueType names don't match enum count");
		};

		inline const char * GetOpcodeText(Opcode opcode)
		{
			return CommonData::opcodeName[static_cast<size_t>(opcode)];
		}

		inline const char * GetSymbolTypeText(SymbolType symbol)
		{
			assert(static_cast<int>(symbol) < static_cast<int>(SymbolType::NumSymbols));
			return CommonData::symbolTypeName[static_cast<size_t>(symbol)];
		}

		inline bool IsConstant(SymbolType symbol)
		{
			assert(static_cast<int>(symbol) < static_cast<int>(SymbolType::NumSymbols));
			return (static_cast<int>(symbol) > static_cast<int>(SymbolType::NameValue)) && (static_cast<int>(symbol) < static_cast<int>(SymbolType::ForwardSlash));
		}

		inline bool IsOperator(SymbolType symbol)
		{
			assert(static_cast<int>(symbol) < static_cast<int>(SymbolType::NumSymbols));
			return (static_cast<int>(symbol) >= static_cast<int>(SymbolType::ForwardSlash)) && (static_cast<int>(symbol) < static_cast<int>(SymbolType::And));
		}

		inline const char * GetValueTypeName(ValueType valueType)
		{
			assert(static_cast<int>(valueType) <= static_cast<int>(ValueType::NumValueTypes));
			return CommonData::valueTypeName[static_cast<size_t>(valueType)];
		}

		inline bool IsKeyword(SymbolType symbol)
		{
			assert(static_cast<int>(symbol) < static_cast<int>(SymbolType::NumSymbols));
			return static_cast<int>(symbol) >= static_cast<int>(SymbolType::And);
		}

		inline size_t GetNamePartCount(const String & name)
		{
			size_t parts = 1;
			for (auto itr = name.begin(); itr != name.end(); ++itr)
			{
				if (*itr == ' ')
					++parts;
			}
			return parts;
		}

		inline RuntimeID GetVariableId(const char * name, size_t nameLen, size_t stackDepth)
		{
			RuntimeID id = GetHash(name, nameLen);
			id += static_cast<RuntimeID>(stackDepth);
			return id;
		}

		inline RuntimeID GetRandomId()
		{
			// Create hash source of current time, a unique id, and a string
			HashData hd;
			memset(&hd, 0, sizeof(hd));
			StrCopy(hd.hs, 16, "0@@@@UniqueName");
			hd.h1 = std::chrono::high_resolution_clock::time_point().time_since_epoch().count();
			hd.h2 = CommonData::uniqueId++;

			// Return a new random Id from unique hash source
			return GetHash(&hd, sizeof(hd));
		}

		inline uint32_t MaxInstructions()
		{
			return CommonData::globalParams.maxInstructions;
		}

		inline bool ErrorOnMaxInstrunction()
		{
			return CommonData::globalParams.errorOnMaxInstrunctions;
		}

		inline bool EnableDebugInfo()
		{
			return CommonData::globalParams.enableDebugInfo;
		}

	} // namespace Impl

	inline String GetVersionString()
	{
		char buffer[32];
		snprintf(buffer, Impl::countof(buffer), "%i.%i.%i", Jinx::MajorVersion, Jinx::MinorVersion, Jinx::PatchNumber);
		return buffer;
	}

	inline void Initialize(const GlobalParams & params)
	{
		Impl::CommonData::globalParams = params;
		InitializeMemory(params);
		Impl::InitializeLogging(params);
	}

	inline void ShutDown()
	{
		ShutDownMemory();
	}


} // namespace Jinx



// end --- JxCommon.cpp --- 



// begin --- JxConversion.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

namespace Jinx::Impl
{

	struct ConvTable
	{
		static inline uint8_t valueTypeToByte[] =
		{
			0,  // Null,
			1,  // Number,
			2,  // Integer,
			3,  // Boolean,
			4,  // String,
			5,  // Collection,
			6,  // CollectionItr,
			7,  // UserData,
			8,  // Buffer,
			9,  // Guid,
			10, // ValType,
			11, // Any
		};

		static_assert(countof(valueTypeToByte) == (static_cast<size_t>(ValueType::NumValueTypes) + 1), "ValueType names don't match enum count");

		static inline ValueType byteToValueType[] =
		{
			ValueType::Null,
			ValueType::Number,
			ValueType::Integer,
			ValueType::Boolean,
			ValueType::String,
			ValueType::Collection,
			ValueType::CollectionItr,
			ValueType::UserObject,
			ValueType::Buffer,
			ValueType::Guid,
			ValueType::ValType,
			ValueType::Any,
		};

		static_assert(countof(byteToValueType) == (static_cast<size_t>(ValueType::NumValueTypes) + 1), "ValueType names don't match enum count");
	};

	inline ValueType ByteToValueType(uint8_t byte)
	{
		assert(byte <= static_cast<size_t>(ValueType::NumValueTypes));
		return ConvTable::byteToValueType[byte];
	}

	inline uint8_t ValueTypeToByte(ValueType type)
	{
		assert(static_cast<size_t>(type) <= static_cast<size_t>(ValueType::NumValueTypes));
		return ConvTable::valueTypeToByte[static_cast<size_t>(type)];
	}

	inline bool StringToBoolean(const String & inValue, bool * outValue)
	{
		assert(outValue);
		if (strcmp(inValue.c_str(), "true") == 0)
		{
			*outValue = true;
			return true;
		}
		else if (strcmp(inValue.c_str(), "false") == 0)
		{
			*outValue = false;
			return true;
		}
		return false;
	}

	inline bool StringToNumber(const String & value, double * outValue, NumericFormat format)
	{
		assert(outValue);
#ifdef JINX_USE_FROM_CHARS
		// In case contintental format is used, replace commas with decimal point
		if (format == NumericFormat::Continental)
		{
			String s = value;
			std::replace(s.begin(), s.end(), ',', '.');
			auto result = std::from_chars(s.data(), s.data() + s.size(), *outValue);
			if (result.ptr == s.data() + s.size())
				return true;
		}
		else
		{
			auto result = std::from_chars(value.data(), value.data() + value.size(), *outValue);
			if (result.ptr == value.data() + value.size())
				return true;
		}
		return false;
#else
		// In case contintental format is used, replace commas with decimal point
		if (format == NumericFormat::Continental)
		{
			String s = value;
			std::replace(s.begin(), s.end(), ',', '.');
			std::istringstream istr(s.c_str());
			istr.imbue(std::locale::classic());
			istr >> *outValue;
			if (istr.fail())
				return false;
		}
		else
		{
			std::istringstream istr(value.c_str());
			istr.imbue(std::locale::classic());
			istr >> *outValue;
			if (istr.fail())
				return false;
		}
		return true;
#endif
	}

	inline bool StringToInteger(const String & value, int64_t * outValue)
	{
		assert(outValue);
#ifdef JINX_USE_FROM_CHARS
		auto result = std::from_chars(value.data(), value.data() + value.size(), *outValue);
		if (result.ptr == value.data() + value.size())
			return true;
		return false;
#else
		char * endPtr;
		*outValue = strtoll(value.data(), &endPtr, 10);
		if (endPtr != (value.data() + value.size()))
			return false;
		return true;
#endif
	}

	inline bool StringToValueType(const String & value, ValueType * outValue)
	{
		if (value == "null")
		{
			*outValue = ValueType::Null;
			return true;
		}
		else if (value == "number")
		{
			*outValue = ValueType::Number;
			return true;
		}
		else if (value == "integer")
		{
			*outValue = ValueType::Integer;
			return true;
		}
		else if (value == "boolean")
		{
			*outValue = ValueType::Boolean;
			return true;
		}
		else if (value == "string")
		{
			*outValue = ValueType::String;
			return true;
		}
		else if (value == "collection")
		{
			*outValue = ValueType::Collection;
			return true;
		}
		else if (value == "collectionitr")
		{
			*outValue = ValueType::CollectionItr;
			return true;
		}
		else if (value == "buffer")
		{
			*outValue = ValueType::Buffer;
			return true;
		}
		else if (value == "guid")
		{
			*outValue = ValueType::Guid;
			return true;
		}
		else if (value == "valtype")
		{
			*outValue = ValueType::ValType;
			return true;
		}
		return false;
	}

	inline bool StringToGuid(const String & value, Guid * outValue)
	{
		assert(outValue);
		Guid guid;
		unsigned long p0;
		unsigned int p1, p2, p3, p4, p5, p6, p7, p8, p9, p10;
#ifdef JINX_WINDOWS
		int err = sscanf_s(value.c_str(), "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			&p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10);
#else
		int err = sscanf(value.c_str(), "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			&p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10);
#endif
		if (err != 11)
			return false;
		guid.data1 = static_cast<uint32_t>(p0);
		guid.data2 = static_cast<uint16_t>(p1);
		guid.data3 = static_cast<uint16_t>(p2);
		guid.data4[0] = static_cast<uint8_t>(p3);
		guid.data4[1] = static_cast<uint8_t>(p4);
		guid.data4[2] = static_cast<uint8_t>(p5);
		guid.data4[3] = static_cast<uint8_t>(p6);
		guid.data4[4] = static_cast<uint8_t>(p7);
		guid.data4[5] = static_cast<uint8_t>(p8);
		guid.data4[6] = static_cast<uint8_t>(p9);
		guid.data4[7] = static_cast<uint8_t>(p10);
		*outValue = guid;
		return true;
	}

	inline bool GetDelimiterAndFormat(const String & value, char & delimiter, NumericFormat & format)
	{
		size_t tabCount = 0;
		size_t commaCount = 0;
		size_t semicolonCount = 0;
		for (size_t i = 0; i < value.size(); ++i)
		{
			char c = value[i];
			if (c == '\t')
				++tabCount;
			else if (c == ',')
				++commaCount;
			else if (c == ';')
				++semicolonCount;
			else if (c == '\n' && i != 0)
				break;
		}
		if (tabCount == 0 && commaCount == 0 && semicolonCount == 0)
			return false;
		delimiter = (commaCount >= tabCount) ? ((commaCount > semicolonCount) ? ',' : ';') : '\t';
		format = delimiter == ';' ? NumericFormat::Continental : NumericFormat::International;
		return true;
	}

	inline bool IsLineEnd(const char current)
	{
		return current == '\n' || current == '\r';
	}

	inline void ParseWhitespace(const char ** current, const char * end)
	{
		while (**current != *end)
		{
			char c = **current;
			if (c != '\t' && c != ' ' && c != '\r' && c != '\n')
				break;
			++(*current);
		}
	}

	inline Variant ParseValue(const String & value, NumericFormat format)
	{
		Guid guid;
		if (StringToGuid(value, &guid))
			return guid;
		int64_t integer;
		if (StringToInteger(value, &integer))
			return integer;
		double number;
		if (StringToNumber(value, &number, format))
			return number;
		bool boolean;
		if (StringToBoolean(value, &boolean))
			return boolean;
		return value;
	}

	inline String ParseCell(char delimiter, const char ** current, const char * end)
	{
		if (*current == end)
			return String();
		bool isQuoted = false;
		if (**current == '"')
		{
			isQuoted = true;
			++(*current);
		}
		String value;
		while (*current != end)
		{
			const char c = **current;
			if (isQuoted)
			{
				// Since this cell is double-quote delimited, proceed without checking delimiters until
				// we see another double quote character.
				if (c == '"')
				{
					// Advance the iterator and check to see if it's followed by the end of file or
					// delimiters.  If so, we're done parsing.  If not, a second double-quote should
					// follow.
					++(*current);
					if (*current == end || **current == delimiter || IsLineEnd(**current))
						break;
					// If this assert hits, your data is malformed, since an interior double-quote was not
					// followed by a second quote
					assert(**current == '"');
				}
			}
			else
			{
				// This isn't a quote-escaped cell, so check for normal delimiters
				if (c == delimiter || IsLineEnd(c))
					break;
			}
			value += c;
			++(*current);
		}
		return value;
	}

	inline std::vector<Variant, Jinx::Allocator<Variant>> ParseRow(char delimiter, NumericFormat format, const char ** current, const char * end)
	{
		std::vector<Variant, Jinx::Allocator<Variant>> variants;

		while (*current != end)
		{
			auto str = ParseCell(delimiter, current, end);
			auto val = ParseValue(str, format);
			variants.push_back(val);
			if (*current != end)
			{
				if (**current == delimiter)
				{
					++(*current);
				}
				else if (IsLineEnd(**current))
				{
					if (*current != end)
					{
						const char nc = *((*current) + 1);
						if (IsLineEnd(nc))
							++(*current);
					}
					++(*current);
					break;
				}
			}
		}

		return variants;
	}

	inline bool StringToCollection(const String & value, CollectionPtr * outValue)
	{

		// First check what type of delimiter is used, tabs or semicolons
		char delimiter;
		NumericFormat format;
		if (!GetDelimiterAndFormat(value, delimiter, format))
			return false;

		// Parse first row, which we'll uses as index values into each subsequent row
		const char * current = value.data();
		const char * end = current + value.size();
		ParseWhitespace(&current, end);
		auto header = ParseRow(delimiter, format, &current, end);
		if (header.empty())
			return false;

		*outValue = CreateCollection();
		CollectionPtr coll = *outValue;

		// Parse and create a collection for each row, and assign column ids from header
		while (true)
		{
			const auto & row = ParseRow(delimiter, format, &current, end);
			if (row.empty())
				break;
			if (row.size() != header.size())
			{
				*outValue = nullptr;
				return false;
			}
			const auto & rowName = row[0];
			auto rowColl = CreateCollection();
			coll->insert({rowName, rowColl });
			for (size_t i = 0; i < row.size(); ++i)
			{
				const auto & headerName = header[i];
				const auto & rowVal = row[i];
				rowColl->insert({ headerName, rowVal });
			}
		}

		return true;
	}


	inline String GuidToString(const Guid & value)
	{
		char buffer[64];
		snprintf(
			buffer,
			countof(buffer),
			"%.*X-%.*X-%.*X-%.*X%.*X-%.*X%.*X%.*X%.*X%.*X%.*X",
			8, value.data1,
			4, value.data2,
			4, value.data3,
			2, value.data4[0],
			2, value.data4[1],
			2, value.data4[2],
			2, value.data4[3],
			2, value.data4[4],
			2, value.data4[5],
			2, value.data4[6],
			2, value.data4[7]
		);
		return String(buffer);
	}

} // namespace Jinx::Impl





// end --- JxConversion.cpp --- 



// begin --- JxFunctionSignature.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

namespace Jinx::Impl
{

	inline FunctionSignature::FunctionSignature()
	{
	}

	inline FunctionSignature::FunctionSignature(VisibilityType visibility, const String & libraryName, const FunctionSignatureParts & parts) :
		m_visibility(visibility),
		m_libraryName(libraryName),
		m_parts(parts)
	{
		if (m_visibility == VisibilityType::Local)
		{
			// Local functions use a randomly generated ID to avoid collisions with any other name.
			// We don't use a predictable hash algorithm because when calling a library function, it
			// could accidentally call the wrong local function if it has the same signature as a local
			// function in the original calling script.  Because it's a local function, there's also 
			// no real need to access it outside of the script itself or through the API either.
			m_id = GetRandomId();
		}
		else
		{
			// Library functions require a predictable ID.
			// Create a unique id based on a hash of the library name, signature text, and parameters
			String hashString = m_libraryName;
			hashString.reserve(64);
			for (auto itr = m_parts.begin(); itr != m_parts.end();)
			{
				if (itr->partType == FunctionSignaturePartType::Name)
				{
					for (const auto & name : itr->names)
						hashString += name;
				}
				else if (itr->partType == FunctionSignaturePartType::Parameter)
				{
					hashString += "{}";
				}
				else
				{
					hashString += "{";
					hashString += itr->names.front();
					hashString += "}";
				}
				++itr;
				if (itr != m_parts.end())
					hashString += " ";
			}
			m_id = GetHash(hashString.c_str(), hashString.length());
		}
	}

	inline size_t FunctionSignature::GetParameterCount() const
	{
		size_t count = 0;
		for (auto & part : m_parts)
		{
			if (part.partType == FunctionSignaturePartType::Parameter)
				++count;
		}
		return count;
	}

	inline FunctionSignatureParts FunctionSignature::GetParameters() const
	{
		FunctionSignatureParts parameters;
		for (auto & part : m_parts)
		{
			if (part.partType == FunctionSignaturePartType::Parameter)
				parameters.push_back(part);
		}
		return parameters;
	}

	inline String FunctionSignature::GetName() const
	{
		String fnName;
		fnName.reserve(32);
		if (!m_libraryName.empty())
		{
			fnName += m_libraryName;
			fnName += " ";
		}
		for (auto partItr = m_parts.begin(); partItr != m_parts.end();)
		{
			if (partItr->partType == FunctionSignaturePartType::Name)
			{
				if (partItr->optional)
					fnName += "(";
				for (auto nameItr = partItr->names.begin(); nameItr != partItr->names.end();)
				{
					fnName += *nameItr;
					++nameItr;
					if (nameItr != partItr->names.end() && partItr->names.size() > 1)
						fnName += "/";
				}
				if (partItr->optional)
					fnName += ")";
			}
			else
			{
				fnName += "{";
				if (partItr->valueType != ValueType::Any)
					fnName += GetValueTypeName(partItr->valueType);
				fnName += "}";
			}

			++partItr;
			if (partItr != m_parts.end())
				fnName += " ";
		}
		return fnName;
	}

	inline void FunctionSignature::Read(BinaryReader & reader)
	{
		// Read this object from a memory buffer
		reader.Read(&m_id);
		reader.Read<VisibilityType, uint8_t>(&m_visibility);
		reader.Read(&m_libraryName);
		uint8_t partSize;
		reader.Read(&partSize);
		for (uint8_t i = 0; i < partSize; ++i)
		{
			FunctionSignaturePart part;
			reader.Read<FunctionSignaturePartType, uint8_t>(&part.partType);
			reader.Read(&part.optional);
			reader.Read<ValueType, uint8_t>(&part.valueType);
			uint8_t nameSize;
			reader.Read(&nameSize);
			for (uint8_t j = 0; j < nameSize; ++j)
			{
				String name;
				reader.Read(&name);
				part.names.push_back(name);
			}
			m_parts.push_back(part);
		}
	}

	inline void FunctionSignature::Write(BinaryWriter & writer) const
	{
		// Write this object to a memory buffer
		writer.Write(m_id);
		writer.Write<VisibilityType, uint8_t>(m_visibility);
		writer.Write(m_libraryName);
		writer.Write(static_cast<uint8_t>(m_parts.size()));
		for (const auto & part : m_parts)
		{
			writer.Write<FunctionSignaturePartType, uint8_t>(part.partType);
			writer.Write(part.optional);
			writer.Write<ValueType, uint8_t>(part.valueType);
			writer.Write(static_cast<uint8_t>(part.names.size()));
			for (const auto & name : part.names)
			{
				writer.Write(name);
			}
		}
	}

	// FunctionSignature part comparison operator overloads
	inline bool operator == (const FunctionSignaturePart & left, const FunctionSignaturePart & right)
	{
		// If the types are different, the parts aren't equal
		if (left.partType != right.partType)
			return false;

		// Check for any matches between the left and right sets of names.  We consider
		// the signature parts equal if there are any matches.
		if (left.partType == FunctionSignaturePartType::Name)
		{
			for (auto & leftName : left.names)
			{
				for (auto & rightName : right.names)
				{
					if (leftName == rightName)
						return true;
				}
			}
			return false;
		}
		return true;
	}

	// FunctionSignature class comparison operator overload
	inline bool operator == (const FunctionSignature & left, const FunctionSignature & right)
	{
		// If there's any discrepency in size, then the signatures aren't equal
		if (left.m_parts.size() != right.m_parts.size())
			return false;

		// Compare all parts between left and right operands.  If any are different,
		// then the signature is not equivalant.
		auto leftPart = left.m_parts.begin();
		auto rightPart = right.m_parts.begin();
		while (leftPart != left.m_parts.end())
		{
			if (!(*leftPart == *rightPart))
				return false;
			++leftPart;
			++rightPart;
		}
		return true;
	}

} // namespace Jinx::Impl



// end --- JxFunctionSignature.cpp --- 



// begin --- JxHash.cpp --- 

// MurmurHash2 was written by Austin Appleby, and is placed in the public
// domain. Appleby disclaims copyright to this source code.
// https://en.wikipedia.org/wiki/MurmurHash

// All code in this source file is compatible with the MIT license.

namespace Jinx::Impl
{

	// This hash variant is supposedly platform and endian independent.
	inline uint32_t MurmurHashNeutral2(const void * key, int len, uint32_t seed)
	{
		const uint32_t m = 0x5bd1e995;
		const int r = 24;

		uint32_t h = seed ^ len;

		const unsigned char * data = (const unsigned char *)key;

		while (len >= 4)
		{
			uint32_t k;

			k = data[0];
			k |= data[1] << 8;
			k |= data[2] << 16;
			k |= data[3] << 24;

			k *= m;
			k ^= k >> r;
			k *= m;

			h *= m;
			h ^= k;

			data += 4;
			len -= 4;
		}

		switch (len)
		{
		case 3: h ^= data[2] << 16;
		[[fallthrough]];
		case 2: h ^= data[1] << 8;
		[[fallthrough]];
		case 1: h ^= data[0];
			h *= m;
		};

		h ^= h >> 13;
		h *= m;
		h ^= h >> 15;

		return h;
	}

	// Use two endian-neutral 32-bit hashes combined to a 64-bit hash to reduce odds of random collisions
	inline uint64_t GetHash(const void * data, size_t len)
	{
		uint32_t hash1 = MurmurHashNeutral2(data, static_cast<int>(len), 0xF835E195);
		uint32_t hash2 = MurmurHashNeutral2(data, static_cast<int>(len), 0x5C285D21);
		uint64_t hash = (static_cast<uint64_t>(hash2) << 32) | static_cast<uint64_t>(hash1);
		assert(hash != InvalidID);
		return hash;
	}

} // namespace Jinx::Impl




// end --- JxHash.cpp --- 



// begin --- JxLexer.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

namespace Jinx::Impl
{

	inline Lexer::Lexer(const SymbolTypeMap & symbolTypeMap, const char * start, const char * end, const String & name) :
		m_name(name),
		m_start(start),
		m_end(end),
		m_current(nullptr),
		m_columnNumber(1),
		m_columnMarker(1),
		m_lineNumber(1),
		m_error(false),
		m_symbolTypeMap(symbolTypeMap)
	{
	}

	inline void Lexer::AdvanceCurrent()
	{
		m_current += GetUtf8CharSize(m_current);
		++m_columnNumber;
	}

	inline void Lexer::CreateSymbol(SymbolType type)
	{
		Symbol symbol(type, m_lineNumber, m_columnMarker);
		m_symbolList.push_back(symbol);
		m_columnMarker = m_columnNumber;
	}

	inline void Lexer::CreateSymbol(double number)
	{
		Symbol symbol(SymbolType::NumberValue, m_lineNumber, m_columnMarker);
		symbol.numVal = number;
		m_symbolList.push_back(symbol);
		m_columnMarker = m_columnNumber;
	}

	inline void Lexer::CreateSymbol(int64_t integer)
	{
		Symbol symbol(SymbolType::IntegerValue, m_lineNumber, m_columnMarker);
		symbol.intVal = integer;
		m_symbolList.push_back(symbol);
		m_columnMarker = m_columnNumber;
	}

	inline void Lexer::CreateSymbol(const String & name)
	{
		Symbol symbol(SymbolType::NameValue, m_lineNumber, m_columnMarker);
		symbol.text = FoldCase(name);
		auto itr = m_symbolTypeMap.find(symbol.text);
		if (itr != m_symbolTypeMap.end())
		{
			symbol.type = itr->second;
		}
		else
		{
			// Special case detection of boolean 'true' and 'false' values.  We don't
			// want to make these symbol types.  Instead, they need to be a BooleanValue
			// type.
			if (symbol.text == "true")
			{
				symbol.type = SymbolType::BooleanValue;
				symbol.boolVal = true;
			}
			else if (symbol.text == "false")
			{
				symbol.type = SymbolType::BooleanValue;
				symbol.boolVal = false;
			}
		}
		m_symbolList.push_back(symbol);
		m_columnMarker = m_columnNumber;
	}

	inline void Lexer::CreateSymbolString(String && text)
	{
		Symbol symbol(SymbolType::StringValue, m_lineNumber, m_columnMarker);
		symbol.text = std::move(text);
		m_symbolList.push_back(symbol);
		m_columnMarker = m_columnNumber;
	}

	inline bool Lexer::Execute()
	{
		m_current = m_start;

		// Create a list of tokens for the parser to analyze
		while (!IsEndOfText())
		{
			// Advance past any whitespace
			ParseWhitespace();

			// Check for end of line character
			if (IsNewline(*m_current))
			{
				// Advance past end of line
				ParseEndOfLine();
				continue;
			}

			// May have reached end of text after parsing whitespace or end of line
			if (IsEndOfText())
				break;

			// Check for operators first
			unsigned char c = *m_current;
			switch (c)
			{
			case '-':
				if (IsNextCharacter('-'))
				{
					ParseComment();
					continue;
				}
				else if (IsNextDigit())
				{
					ParseNumber();
					continue;
				}
				else
					CreateSymbol(SymbolType::Minus);
				break;
			case '<':
				if (IsNextCharacter('='))
				{
					CreateSymbol(SymbolType::LessThanEquals);
					AdvanceCurrent();
				}
				else
				{
					CreateSymbol(SymbolType::LessThan);
				}
				break;
			case '>':
				if (IsNextCharacter('='))
				{
					CreateSymbol(SymbolType::GreaterThanEquals);
					AdvanceCurrent();
				}
				else
				{
					CreateSymbol(SymbolType::GreaterThan);
				}
				break;
			case '"':
				ParseString();
				break;
			case '*':
				CreateSymbol(SymbolType::Asterisk);
				break;
			case '/':
				CreateSymbol(SymbolType::ForwardSlash);
				break;
			case '+':
				if (IsNextDigit())
				{
					ParseNumber();
					continue;
				}
				else
					CreateSymbol(SymbolType::Plus);
				break;
			case '=':
				CreateSymbol(SymbolType::Equals);
				break;
			case '!':
				if (IsNextCharacter('='))
				{
					CreateSymbol(SymbolType::NotEquals);
					AdvanceCurrent();
				}
				else
				{
					Error("Invalid symbol '!'");
					break;
				}
				break;
			case '%':
				CreateSymbol(SymbolType::Percent);
				break;
			case ',':
				CreateSymbol(SymbolType::Comma);
				break;
			case '(':
				CreateSymbol(SymbolType::ParenOpen);
				break;
			case ')':
				CreateSymbol(SymbolType::ParenClose);
				break;
			case '{':
				CreateSymbol(SymbolType::CurlyOpen);
				break;
			case '}':
				CreateSymbol(SymbolType::CurlyClose);
				break;
			case '[':
				CreateSymbol(SymbolType::SquareOpen);
				break;
			case ']':
				CreateSymbol(SymbolType::SquareClose);
				break;
			default:
			{
				if (c == '.' && IsNextCharacter('.'))
					ParseEllipse();
				else
				{
					if (IsNumberStart(c))
						ParseNumber();
					else
						ParseName();
				}
				continue;
			}
			};

			// Check for errors
			if (m_error)
				break;

			// Advance one UTF-8 character
			AdvanceCurrent();
		}

		// Make sure programs always end with a new line marker for parsing consistency
		if (!m_symbolList.empty() && m_symbolList.back().type != SymbolType::NewLine)
			CreateSymbol(SymbolType::NewLine);

		// Return error status
		return !m_error;
	}

	inline bool Lexer::IsName(const char * ptr) const
	{
		char c = *ptr;
		if (IsWhitespace(c) || IsNewline(c))
			return false;
		if (static_cast<unsigned char>(c) <= 32)
			return false;
		if (c == ',' || c == '.' || c == '[' || c == ']' || c == '(' || c == ')' || c == '{' || c == '}' || c == '/')
			return false;
		return true;
	}

	inline bool Lexer::IsNameStart(const char * ptr) const
	{
		char c = *ptr;
		if (IsNumberStart(c) || !IsName(ptr))
			return false;
		return true;
	}

	inline bool Lexer::IsNextCharacter(unsigned char c) const
	{
		if (IsEndOfText())
			return false;
		const char ch = *(m_current + 1);
		if (IsNewline(c))
			return false;
		if (ch != c)
			return false;
		return true;
	}

	inline bool Lexer::IsNextDigit() const
	{
		if (IsEndOfText())
			return false;
		const char ch = *(m_current + 1);
		if (ch < 0)
			return false;
		if (IsNewline(ch))
			return false;
		if (!std::isdigit(ch))
			return false;
		return true;
	}

	inline void Lexer::ParseComment()
	{
		AdvanceCurrent();
		bool blockComment = IsNextCharacter('-');

		// This is a block comment
		if (blockComment)
		{
			AdvanceCurrent();

			// Advance until the end of the line or until we stop seeing dashes
			while (!IsEndOfText())
			{
				if (*m_current != '-')
					break;
				else if (IsNewline(*m_current))
				{
					ParseEndOfLine();
					break;
				}
				AdvanceCurrent();
			}

			// Find minimum run of three dashes to closed block comment
			while (!IsEndOfText())
			{
				uint32_t dashCount = 0;
				while (!IsEndOfText() && *m_current == '-')
				{
					dashCount++;
					AdvanceCurrent();
				}
				if (dashCount >= 3)
					return;
				else if (IsNewline(*m_current))
					ParseEndOfLine();
				else
					AdvanceCurrent();
			}
			Error("Mismatched block comments");
		}
		// This is a single line comment
		else
		{
			// Advance until the end of the line, then return
			while (!IsEndOfText())
			{
				if (IsNewline(*m_current))
				{
					ParseEndOfLine();
					return;
				}
				AdvanceCurrent();
			}
		}
	}

	inline void Lexer::ParseEllipse()
	{
		int count = 0;
		while (!IsEndOfText() && count < 3)
		{
			char c = *m_current;
			if (c != '.')
			{
				Error("Expected ellipse");
				return;
			}
			++count;
			AdvanceCurrent();
		}
		ParseWhitespaceAndNewlines();
		if (*m_current == '\r')
			AdvanceCurrent();
		if (IsEndOfText())
			return;
		if (*m_current == '\n')
			AdvanceCurrent();
	}

	inline void Lexer::ParseEndOfLine()
	{
		// Handle both Unix and Windows style line ends.
		if (*m_current == '\r')
			AdvanceCurrent();
		if (IsEndOfText())
			return;
		if (*m_current == '\n')
			AdvanceCurrent();
		// No need to add redundant end of line symbols
		if (!m_symbolList.empty() && m_symbolList.back().type != SymbolType::NewLine)
			CreateSymbol(SymbolType::NewLine);
		++m_lineNumber;
		m_columnNumber = 1;
		m_columnMarker = 1;
	}

	inline void Lexer::ParseName()
	{
		bool quotedName = false;
		if (*m_current == '\'')
		{
			quotedName = true;
			AdvanceCurrent();
		}

		const char * startName = m_current;

		if (!IsNameStart(m_current))
			Error("Invalid character in name");
		AdvanceCurrent();

		// Advance until we reach the end of the text or hit a non-name character
		while (!IsEndOfText())
		{
			if (quotedName)
			{
				if (*m_current == '\'')
					break;
			}
			else
			{
				if (!IsName(m_current))
					break;
			}
			AdvanceCurrent();
		}
		size_t count = m_current - startName;
		if (quotedName)
			AdvanceCurrent();
		auto name = String(startName, count);
		CreateSymbol(name);
	}

	inline void Lexer::ParseNumber()
	{
		const char * startNum = m_current;
		int points = 0;
		if (*startNum == '.')
			points = 1;
		AdvanceCurrent();
		while (!IsEndOfText())
		{
			char c = *m_current;
			if (!IsNumber(c))
			{
				if (IsName(m_current))
				{
					Error("Invalid number format");
					break;
				}
				else
				{
					break;
				}
			}
			if (c == '.')
				++points;
			AdvanceCurrent();
		}
		if (points > 1)
			Error("Invalid number format");
		else if (points == 0)
		{
			int64_t integer = atol(reinterpret_cast<const char *>(startNum));
			CreateSymbol(integer);
		}
		else
		{
			double number = atof(reinterpret_cast<const char *>(startNum));
			CreateSymbol(number);
		}
	}

	inline void Lexer::ParseString()
	{
		// Advance past double quote character
		AdvanceCurrent();

		// Mark the start of the string
		const char * startName = m_current;

		// Check for matching end string token
		bool validString = false;
		while (!IsEndOfText())
		{
			if (IsNewline(*m_current))
				break;
			if (*m_current == '"')
			{
				validString = true;
				break;
			}
			AdvanceCurrent();
		}
		if (!validString)
		{
			Error("Could not find matching quote");
			return;
		}
		size_t count = m_current - startName;
		auto str = String(startName, count);
		CreateSymbolString(std::move(str));
	}

	inline void Lexer::ParseWhitespace()
	{
		while (!IsEndOfText())
		{
			if (!IsWhitespace(*m_current))
				break;
			if (*m_current == '\t')
				m_columnNumber += (LogTabWidth - 1);
			AdvanceCurrent();
		}
		m_columnMarker = m_columnNumber;
	}


	inline void Lexer::ParseWhitespaceAndNewlines()
	{
		while (!IsEndOfText())
		{
			if (!IsWhitespace(*m_current) && !IsNewline(*m_current))
				break;
			AdvanceCurrent();
		}
		m_columnMarker = m_columnNumber;
	}

} // namespace Jinx::Impl



// end --- JxLexer.cpp --- 



// begin --- JxLibCore.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

namespace Jinx::Impl
{

	inline void DebugWriteInternal(LogLevel level, const Variant & var)
	{
		if (var.IsCollection())
		{
			const auto & coll = *var.GetCollection();
			for (const auto & v : coll)
			{
				DebugWriteInternal(level, v.second);
			}
		}
		else
		{
			auto str = var.GetString();
			auto cstr = str.c_str();
			if (cstr)
				LogWrite(level, cstr);
		}
	}

	inline Variant Write(ScriptPtr, const Parameters & params)
	{
		if (params.empty())
			return nullptr;
		DebugWriteInternal(LogLevel::Info, params[0]);
		return nullptr;
	}

	inline Variant WriteLine(ScriptPtr, const Parameters & params)
	{
		if (!params.empty())
			DebugWriteInternal(LogLevel::Info, params[0]);
		LogWrite(LogLevel::Info, "\n");
		return nullptr;
	}

	inline Variant GetSize(ScriptPtr, const Parameters & params)
	{
		switch (params[0].GetType())
		{
		case ValueType::Collection:
			return static_cast<int64_t>(params[0].GetCollection()->size());
		case ValueType::String:
			return static_cast<int64_t>(params[0].GetString().length());
		case ValueType::Buffer:
			return static_cast<int64_t>(params[0].GetBuffer()->Size());
		default:
			break;
		}
		return nullptr;
	}

	inline Variant IsEmpty(ScriptPtr, const Parameters & params)
	{
		switch (params[0].GetType())
		{
		case ValueType::Collection:
			return params[0].GetCollection()->empty();
		case ValueType::String:
			return params[0].GetString().empty();
		case ValueType::Buffer:
			return params[0].GetBuffer()->Size() == 0;
		default:
			break;
		}
		return nullptr;
	}

	inline Variant GetKey(ScriptPtr, const Parameters & params)
	{
		if (!params[0].IsCollectionItr())
		{
			LogWriteLine(LogLevel::Error, "'get key' called with non-iterator param");
			return nullptr;
		}
		return params[0].GetCollectionItr().first->first;
	}

	inline Variant GetValue(ScriptPtr, const Parameters & params)
	{
		if (!params[0].IsCollectionItr())
		{
			LogWriteLine(LogLevel::Error, "'get value' called with non-iterator param");
			return nullptr;
		}
		return params[0].GetCollectionItr().first->second;
	}

	inline Variant GetCallStack(ScriptPtr script, [[maybe_unused]] const Parameters & params)
	{
		ScriptIPtr s = std::static_pointer_cast<Script>(script);
		auto functions = s->GetCallStack();
		auto var = CreateCollection();
		int64_t index = 1;
		for (const auto & fnName : functions)
			var->insert(std::make_pair(index++, fnName));
		return var;
	}

	inline void RegisterLibCore(RuntimePtr runtime)
	{
		auto library = runtime->GetLibrary("core");

		// Register core functions
		library->RegisterFunction(Visibility::Public, { "write {}" }, Write);
		library->RegisterFunction(Visibility::Public, { "write line {}" }, WriteLine);
		library->RegisterFunction(Visibility::Public, { "{} (get) size" }, GetSize);
		library->RegisterFunction(Visibility::Public, { "{} (is) empty" }, IsEmpty);
		library->RegisterFunction(Visibility::Public, { "{} (get) key" }, GetKey);
		library->RegisterFunction(Visibility::Public, { "{} (get) value" }, GetValue);
		library->RegisterFunction(Visibility::Public, { "(get) call stack" }, GetCallStack);

		// Register core properties
		library->RegisterProperty(Visibility::Public, Access::ReadOnly, { "newline" }, "\n");
	}

} // namespace Jinx::Impl



// end --- JxLibCore.cpp --- 



// begin --- JxLibrary.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

namespace Jinx::Impl
{

	inline Library::Library(RuntimeWPtr runtime, const String & name) :
		m_name(name),
		m_maxPropertyParts(0),
		m_runtime(runtime)
	{
	}

	inline FunctionSignature Library::CreateFunctionSignature(Visibility visibility, const String & name) const
	{
		Lexer lexer(m_runtime.lock()->GetSymbolTypeMap(), name.c_str(), name.c_str() + name.size(), name);
		if (!lexer.Execute())
			return FunctionSignature();
		Parser parser(m_runtime.lock(), lexer.GetSymbolList(), name);
		return parser.ParseFunctionSignature(visibility == Visibility::Public ? VisibilityType::Public : VisibilityType::Private, m_name);
	}

	inline FunctionSignature Library::FindFunctionSignature(Visibility visibility, const String & name) const
	{
		auto signature = CreateFunctionSignature(visibility, name);
		std::lock_guard<std::mutex> lock(m_functionMutex);
		auto itr = std::find(m_functionList.begin(), m_functionList.end(), signature);
		if (itr == m_functionList.end())
			return FunctionSignature();
		return signature;
	}

	inline const FunctionPtrList Library::Functions() const
	{
		std::lock_guard<std::mutex> lock(m_functionMutex);
		FunctionPtrList fnList;
		fnList.reserve(m_functionList.size());
		for (const auto & fn : m_functionList)
			fnList.push_back(&fn);
		return fnList;
	}

	inline bool Library::FunctionSignatureExists(const FunctionSignature & signature) const
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex);
		auto itr = std::find(m_functionList.begin(), m_functionList.end(), signature);
		return itr == m_functionList.end() ? false : true;
	}

	inline PropertyName Library::GetPropertyName(const String & name)
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex);
		auto itr = m_propertyNameTable.find(name);
		if (itr == m_propertyNameTable.end())
			return PropertyName();
		return itr->second;
	}

	inline Variant Library::GetProperty(const String & name) const
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex);
		auto itr = m_propertyNameTable.find(name);
		if (itr == m_propertyNameTable.end())
			return Variant();
		auto runtime = m_runtime.lock();
		if (!runtime)
			return Variant();
		return runtime->GetProperty(itr->second.GetId());
	}

	inline bool Library::PropertyNameExists(const String & name) const
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex);
		return m_propertyNameTable.find(name) == m_propertyNameTable.end() ? false : true;
	}

	inline bool Library::RegisterFunction(Visibility visibility, const String & name, FunctionCallback function)
	{
		if (name.empty())
		{
			LogWriteLine(LogLevel::Error, "Registered function requires a valid name");
			return false;
		}
		if (!function)
		{
			LogWriteLine(LogLevel::Error, "Registered function requires a valid callback");
			return false;
		}

		// Calculate the function signature
		auto signature = CreateFunctionSignature(visibility, name);
		if (!signature.IsValid())
			return false;

		// Register function in library table.  This allows the the parser to find
		// and use this function.
		RegisterFunctionSignature(signature);

		// Register the function definition with the runtime system for 
		// runtime lookups.
		auto runtime = m_runtime.lock();
		if (!runtime)
			return false;
		runtime->RegisterFunction(signature, function);

		// Return success
		return true;
	}

	inline void Library::RegisterFunctionSignature(const FunctionSignature & signature)
	{
		std::lock_guard<std::mutex> lock(m_functionMutex);
		m_functionList.push_back(signature);
	}

	inline bool Library::RegisterProperty(Visibility visibility, Access access, const String & name, const Variant & value)
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex);

		// Register the property name with the library
		PropertyName prop(visibility == Visibility::Public ? VisibilityType::Public : VisibilityType::Private, access == Access::ReadOnly ? true : false, GetName(), name);
		if (!RegisterPropertyNameInternal(prop, false))
			return false;

		// Set the property with the runtime value
		auto runtime = m_runtime.lock();
		if (!runtime)
			return false;
		runtime->SetProperty(prop.GetId(), value);

		// Return success
		return true;
	}

	inline bool Library::RegisterPropertyName(const PropertyName & propertyName, bool checkForDuplicates)
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex);
		return RegisterPropertyNameInternal(propertyName, checkForDuplicates);
	}

	inline bool Library::RegisterPropertyNameInternal(const PropertyName & propertyName, bool checkForDuplicates)
	{
		if (checkForDuplicates && (m_propertyNameTable.find(propertyName.GetName()) != m_propertyNameTable.end()))
			return false;
		m_propertyNameTable.insert(std::make_pair(propertyName.GetName(), propertyName));
		if (propertyName.GetPartCount() > m_maxPropertyParts)
			m_maxPropertyParts = propertyName.GetPartCount();
		return true;
	}

	inline void Library::SetProperty(const String & name, const Variant & value)
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex);
		auto itr = m_propertyNameTable.find(name);
		if (itr == m_propertyNameTable.end())
			return;
		auto runtime = m_runtime.lock();
		if (!runtime)
			return;
		runtime->SetProperty(itr->second.GetId(), value);
	}

} // namespace Jinx::Impl



// end --- JxLibrary.cpp --- 



// begin --- JxLogging.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

namespace Jinx::Impl
{

	const unsigned BufferSize = 1024 * 4;

	struct Log
	{
		static inline bool enableLogging = true;
		static inline bool logSymbols = false;
		static inline bool logBytecode = false;
		static inline LogFn logFn = [](LogLevel, const char * logText) { printf("%s", logText); };
		static inline std::mutex logMutex;
	};

	inline void LogWrite(LogLevel level, const char * format, ...)
	{
		std::unique_lock<std::mutex> lock(Log::logMutex);
		if (!Log::enableLogging)
			return;
		va_list argptr;
		va_start(argptr, format);
		char buffer[BufferSize];
#if defined(JINX_WINDOWS)
		_vsnprintf_s(buffer, BufferSize, _TRUNCATE, format, argptr);
#else
		vsnprintf(buffer, BufferSize, format, argptr);
#endif
		Log::logFn(level, buffer);
		va_end(argptr);
	}

	inline void LogWriteLine(LogLevel level, const char * format, ...)
	{
		std::unique_lock<std::mutex> lock(Log::logMutex);
		if (!Log::enableLogging)
			return;
		va_list argptr;
		va_start(argptr, format);
		char buffer[BufferSize];
#if defined(JINX_WINDOWS)
		_vsnprintf_s(buffer, BufferSize, _TRUNCATE, format, argptr);
#else
		vsnprintf(buffer, BufferSize, format, argptr);
#endif
		size_t len = strlen(buffer);
		if (len < BufferSize - 2)
		{
			buffer[len] = '\n';
			buffer[len + 1] = 0;
		}
		Log::logFn(level, buffer);
		va_end(argptr);
	}

	inline void InitializeLogging(const GlobalParams & params)
	{
		Log::enableLogging = params.enableLogging;
		if (Log::enableLogging)
		{
			if (params.logFn)
				Log::logFn = params.logFn;
			Log::logSymbols = params.logSymbols;
			Log::logBytecode = params.logBytecode;
		}
	}

	inline bool IsLogSymbolsEnabled()
	{
		return Log::logSymbols;
	}

	inline bool IsLogBytecodeEnabled()
	{
		return Log::logBytecode;
	}

} // namespace Jinx::Impl



// end --- JxLogging.cpp --- 



// begin --- JxMemory.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

namespace Jinx
{
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


	// External allocation functions
	namespace Impl
	{
		struct Alloc
		{
			static inline AllocFn allocFn = [](size_t size) { return malloc(size); };
			static inline ReallocFn reallocFn = [](void * p, size_t size) { return realloc(p, size); };
			static inline FreeFn freeFn = [](void * p) { return free(p); };
		};

#ifndef JINX_DISABLE_POOL_ALLOCATOR

#ifdef JINX_USE_MEMORY_GUARDS
		const uint8_t MEMORY_GUARD_PATTERN = 0xA8;
		const size_t MEMORY_GUARD_SIZE = 16;
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
			std::mutex m_mutex;
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
			void * Alloc(size_t bytes) { return Alloc::allocFn(bytes); }
			void * Realloc(void * ptr, size_t bytes) { return Alloc::reallocFn(ptr, bytes); }
			void Free(void * ptr) { Alloc::freeFn(ptr); }
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

		struct Mem
		{
#ifndef JINX_DISABLE_POOL_ALLOCATOR

#ifdef JINX_USE_MEMORY_GUARDS
			static inline uint8_t memoryGuardCheck[MEMORY_GUARD_SIZE] =
			{
				MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN,
				MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN,
				MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN,
				MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN, MEMORY_GUARD_PATTERN,
			};
			static_assert(countof(memoryGuardCheck) == (size_t)MEMORY_GUARD_SIZE, "Memory guard array size mismatch");

#endif // JINX_USE_MEMORY_GUARDS

			// Each thread has its own local heap.  This helps to avoid thread contention
			// when scripts are executed on parallel threads.  Note that reallocs and frees
			// are allowed to occur from different threads (preventing this would be unweildy), 
			// so locks are still needed to ensure thread-safety inside the heap itself.
			static inline thread_local BlockHeap	heap;

			// List of all thread-local heaps
			static inline std::recursive_mutex heapMutex;
			static inline BlockHeap * head;
			static inline BlockHeap * tail;

			// Allocation parameters
			static inline size_t allocBlockSize = (1024 * 32) - sizeof(MemoryBlock);
			static inline size_t maxAllocSpareBlocks = 2;

			// Track global memory statistics
			static inline std::atomic<uint64_t> externalAllocCount;
			static inline std::atomic<uint64_t> externalFreeCount;
			static inline std::atomic<uint64_t> internalAllocCount;
			static inline std::atomic<uint64_t> internalFreeCount;
			static inline std::atomic<uint64_t> currentBlockCount;
			static inline std::atomic<uint64_t> currentAllocatedMemory;
			static inline std::atomic<uint64_t> currentUsedMemory;

#else // JINX_DISABLE_POOL_ALLOCATOR
			static inline DefaultHeap heap;
#endif // JINX_DISABLE_POOL_ALLOCATOR
		};

#ifndef JINX_DISABLE_POOL_ALLOCATOR

		inline BlockHeap::BlockHeap() :
			m_next(nullptr),
			m_allocHead(nullptr),
			m_allocTail(nullptr),
			m_spareHead(nullptr),
			m_spareTail(nullptr),
			m_allocSpareBlocks(0)
		{

			// Ensure thread-safe access to the global heap list
			std::lock_guard<std::recursive_mutex> lock(Mem::heapMutex);

			// Add block heap to the global list
			if (Mem::head == nullptr)
			{
				assert(Mem::tail == nullptr);
				Mem::head = this;
				m_prev = nullptr;
				Mem::tail = this;
			}
			else
			{
				assert(Mem::tail);
				m_prev = Mem::tail;
				Mem::tail->m_next = this;
				Mem::tail = this;
			}
		}

		inline BlockHeap::~BlockHeap()
		{
			ShutDown();
			assert(m_allocHead == nullptr && m_allocTail == nullptr);
			assert(m_spareHead == nullptr && m_spareTail == nullptr);
		}

		inline void * BlockHeap::Alloc(size_t bytes)
		{
			if (!bytes)
				return nullptr;

			// Ensure thread-safe access to the allocator
			std::lock_guard<std::mutex> lock(m_mutex);

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
			Mem::currentUsedMemory += requestedBytes;
			Mem::internalAllocCount++;

			// Return the allocated pointer advanced by the size of the memory header.  We'll
			// reverse the process when freeing the memory.
			return static_cast<char*>(ptr) + sizeof(MemoryHeader);
		}

		inline MemoryBlock * BlockHeap::AllocBlock(size_t bytes)
		{
			size_t blockSize = Mem::allocBlockSize;
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
				newBlock = static_cast<MemoryBlock *>(Impl::Alloc::allocFn(blockSize + sizeof(MemoryBlock)));
				newBlock->capacity = blockSize;
				newBlock->data = reinterpret_cast<uint8_t *>(newBlock) + sizeof(MemoryBlock);
				Mem::currentAllocatedMemory += (blockSize + sizeof(MemoryBlock));
				Mem::externalAllocCount++;
				Mem::currentBlockCount++;
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

		inline void BlockHeap::Free(void * ptr)
		{
			// Ensure thread-safe access to the allocated blocks
			std::lock_guard<std::mutex> lock(m_mutex);

			// Retrieve the memory header from the raw pointer
			MemoryHeader * header = reinterpret_cast<MemoryHeader*>(static_cast<char *>(ptr) - sizeof(MemoryHeader));

			// Free the memory from the allocated block
			FreeInternal(header);
		}

		inline void BlockHeap::Free(MemoryHeader * header)
		{
			// Ensure thread-safe access to the allocated blocks
			std::lock_guard<std::mutex> lock(m_mutex);

			// Free the memory from the allocated block
			FreeInternal(header);
		}

		inline void BlockHeap::FreeInternal(MemoryHeader * header)
		{
			// Retrieve the memory block from the header
			MemoryBlock * memBlock = header->memBlock;

#ifdef JINX_USE_MEMORY_GUARDS
			assert(memcmp(header->memGuardHead, Mem::memoryGuardCheck, MEMORY_GUARD_SIZE) == 0);
			assert(memcmp(header->memGuardTail, Mem::memoryGuardCheck, MEMORY_GUARD_SIZE) == 0);
#endif 

			// Assert we're not freeing more than we've allocated
			assert(memBlock->usedBytes >= header->bytes);
			assert(memBlock->count > 0);

			// Subtract memory buffer size
			memBlock->usedBytes -= header->bytes;

			// Subtract an allocation count
			memBlock->count--;

			// Update memory stats
			Mem::currentUsedMemory -= header->bytes;
			Mem::internalFreeCount++;

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

		inline void BlockHeap::FreeBlock(MemoryBlock * block)
		{
			assert(block);
			assert(block->data);
			assert(block->count == 0);

#ifdef JINX_USE_MEMORY_GUARDS
			assert(memcmp(block->memGuardHead, Mem::memoryGuardCheck, MEMORY_GUARD_SIZE) == 0);
			assert(memcmp(block->memGuardTail, Mem::memoryGuardCheck, MEMORY_GUARD_SIZE) == 0);
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
			if (m_allocSpareBlocks < Mem::maxAllocSpareBlocks)
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
				Mem::externalFreeCount++;
				Mem::currentAllocatedMemory -= (block->capacity + sizeof(MemoryBlock));
				Mem::currentBlockCount--;

				// Free the block of memory
				Impl::Alloc::freeFn(block);
			}
		}

		inline void BlockHeap::LogAllocations()
		{
			LogWriteLine(LogLevel::Info, "=== Memory Log Begin ===");

			// Log all memory blocks
			auto memBlock = m_allocHead;
			while (memBlock)
			{
				LogWriteLine(LogLevel::Info, "");
				LogWriteLine(LogLevel::Info, "--- Memory Block ---");
#ifdef JINX_USE_MEMORY_GUARDS
				bool memGuardsIntact = (memcmp(memBlock->memGuardHead, Mem::memoryGuardCheck, MEMORY_GUARD_SIZE) == 0) &&
					(memcmp(memBlock->memGuardTail, Mem::memoryGuardCheck, MEMORY_GUARD_SIZE) == 0) ? true : false;
				LogWriteLine(LogLevel::Info, "Memory guards intact: %s", memGuardsIntact ? "true" : "false");
#endif 		
				LogWriteLine(LogLevel::Info, "Data = %p", memBlock->data);
				LogWriteLine(LogLevel::Info, "Used bytes = %" PRIuPTR, memBlock->usedBytes);
				LogWriteLine(LogLevel::Info, "Allocated bytes = %" PRIuPTR, memBlock->allocatedBytes);
				LogWriteLine(LogLevel::Info, "Capacity = %" PRIuPTR, memBlock->capacity);
				LogWriteLine(LogLevel::Info, "Count = %" PRIuPTR, memBlock->count);
#ifdef JINX_DEBUG_ALLOCATION
				LogWriteLine(LogLevel::Info, "Memory allocations:");
				auto memHeader = memBlock->head;
				while (memHeader)
				{
					LogWriteLine(LogLevel::Info, "Allocation %p, size: %"  PRIuPTR ", File: %s, Function: %s",
						memHeader->memBlock, memHeader->bytes, memHeader->file ? memHeader->file : "", memHeader->function ? memHeader->function : "");
					memHeader = memHeader->next;
				}
#endif
				memBlock = memBlock->next;
			}
			LogWriteLine(LogLevel::Info, "");

			// Log memory stats
			auto memStats = GetMemoryStats();
			LogWriteLine(LogLevel::Info, "--- Memory Stats ---");
			LogWriteLine(LogLevel::Info, "External alloc count:     %i", memStats.externalAllocCount);
			LogWriteLine(LogLevel::Info, "External free count:      %i", memStats.externalFreeCount);
			LogWriteLine(LogLevel::Info, "Internal alloc count:     %i", memStats.internalAllocCount);
			LogWriteLine(LogLevel::Info, "Internal free count:      %i", memStats.internalFreeCount);
			LogWriteLine(LogLevel::Info, "Current block count:      %i", memStats.currentBlockCount);
			LogWriteLine(LogLevel::Info, "Current allocated memory: %lli", memStats.currentAllocatedMemory);
			LogWriteLine(LogLevel::Info, "Current used memory:      %lli", memStats.currentAllocatedMemory);
			LogWriteLine(LogLevel::Info, "");
			LogWriteLine(LogLevel::Info, "=== Memory Log End ===");

		}

		inline void * BlockHeap::Realloc(void * ptr, size_t bytes)
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

		inline void BlockHeap::ShutDown()
		{
			// Free memory from allocation list
			MemoryBlock * curr = m_allocHead;
			MemoryBlock * next;
			while (curr)
			{
				next = curr->next;
				if (curr->usedBytes == 0)
				{
					Mem::externalFreeCount++;
					Mem::currentAllocatedMemory -= (curr->capacity + sizeof(MemoryBlock));
					Impl::Alloc::freeFn(curr);
				}
				else
				{
					LogWriteLine(LogLevel::Warning, "Could not free block at shutdown.  Memory still in use.");
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
					Mem::externalFreeCount++;
					Mem::currentAllocatedMemory -= (curr->capacity + sizeof(MemoryBlock));
					Impl::Alloc::freeFn(curr);
				}
				else
				{
					LogWriteLine(LogLevel::Warning, "Could not free block at shutdown.  Memory still in use.");
				}
				curr = next;
			}
			m_spareHead = nullptr;
			m_spareTail = nullptr;

			// Block heap has already been removed from the global list
			if (m_prev == nullptr && m_next == nullptr)
				return;

			// Ensure thread-safe access to the global heap list
			std::lock_guard<std::recursive_mutex> lock(Mem::heapMutex);

			// Remove block heap from global heap list
			if (this == Mem::head)
				Mem::head = m_next;
			else
				m_prev->m_next = m_next;
			if (this == Mem::tail)
				Mem::tail = m_prev;
			else
				m_next->m_prev = m_prev;
			m_prev = nullptr;
			m_next = nullptr;
		}

#endif // JINX_DISABLE_POOL_ALLOCATOR

	} // namespace Impl

#ifdef JINX_DEBUG_ALLOCATION

	inline void * MemPoolAllocate(const char * file, const char * function, uint32_t line, size_t bytes)
	{
		void * p;
#if defined(JINX_DEBUG_USE_STD_ALLOC)
		Jinx::unused(file);
		Jinx::unused(function);
		Jinx::unused(line);
		p = malloc(bytes);
#elif defined(JINX_DISABLE_POOL_ALLOCATOR)
		Jinx::unused(file);
		Jinx::unused(function);
		Jinx::unused(line);
		p = Impl::Mem::heap.Alloc(bytes);
#else
		p = Impl::Mem::heap.Alloc(bytes);
		Impl::MemoryHeader * header = reinterpret_cast<Impl::MemoryHeader*>(static_cast<char *>(p) - sizeof(Impl::MemoryHeader));
		header->file = file;
		header->function = function;
		header->line = line;
#endif
		return p;
	}

	inline void * MemPoolReallocate(const char * file, const char * function, uint32_t line, void * ptr, size_t bytes)
	{
		void * p;
#ifdef JINX_DEBUG_USE_STD_ALLOC
		Jinx::unused(file);
		Jinx::unused(function);
		Jinx::unused(line);
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
		Jinx::unused(file);
		Jinx::unused(function);
		Jinx::unused(line);
		p = Impl::Mem::heap.Realloc(ptr, bytes);
#else
		if (ptr)
		{
			Impl::MemoryHeader * header = reinterpret_cast<Impl::MemoryHeader*>(static_cast<char *>(ptr) - sizeof(Impl::MemoryHeader));
			p = header->heap->Realloc(ptr, bytes);
		}
		else
		{
			p = Impl::Mem::heap.Realloc(ptr, bytes);
		}
		if (p)
		{
			Impl::MemoryHeader * header = reinterpret_cast<Impl::MemoryHeader*>(static_cast<char *>(p) - sizeof(Impl::MemoryHeader));
			header->file = file;
			header->function = function;
			header->line = line;
		}
#endif
		return p;
	}

#else

	inline void * MemPoolAllocate(size_t bytes)
	{
		void * p;
#ifdef JINX_DEBUG_USE_STD_ALLOC
		p = malloc(bytes);
#else
		p = Impl::Mem::heap.Alloc(bytes);
#endif
		return p;
	}

	inline void * MemPoolReallocate(void * ptr, size_t bytes)
	{
		void * p;
#ifdef JINX_DEBUG_USE_STD_ALLOC
		p = realloc(ptr, bytes);
#elif defined(JINX_DISABLE_POOL_ALLOCATOR)
		p = Impl::Mem::heap.Realloc(ptr, bytes);
#else
		if (ptr)
		{
			Impl::MemoryHeader * header = reinterpret_cast<Impl::MemoryHeader*>(static_cast<char *>(ptr) - sizeof(Impl::MemoryHeader));
			p = header->heap->Realloc(ptr, bytes);
		}
		else
		{
			p = Impl::Mem::heap.Realloc(ptr, bytes);
		}
#endif
		return p;
	}

#endif

	inline void MemPoolFree(void * ptr)
	{
		if (!ptr)
			return;
#ifdef JINX_DEBUG_USE_STD_ALLOC
		free(ptr);
#elif defined(JINX_DISABLE_POOL_ALLOCATOR)
		Impl::Mem::heap.Free(ptr);
#else
		Impl::MemoryHeader * header = reinterpret_cast<Impl::MemoryHeader*>(static_cast<char *>(ptr) - sizeof(Impl::MemoryHeader));
		header->heap->Free(ptr);
#endif
	}

	inline void InitializeMemory(const GlobalParams & params)
	{
		if (params.allocFn || params.reallocFn || params.freeFn)
		{
			// If you're using one custom memory function, you must use them ALL
			assert(params.allocFn && params.reallocFn && params.freeFn);
			Impl::Alloc::allocFn = params.allocFn;
			Impl::Alloc::reallocFn = params.reallocFn;
			Impl::Alloc::freeFn = params.freeFn;
		}

#ifndef JINX_DISABLE_POOL_ALLOCATOR
		// Alloc block size must be at least 4K (otherwise, what's the point of a block allocator?)
		assert(params.allocBlockSize >= (1024 * 4));
		Impl::Mem::allocBlockSize = params.allocBlockSize - sizeof(Impl::MemoryBlock);
		Impl::Mem::maxAllocSpareBlocks = params.allocSpareBlocks;
#endif

	}

	inline void ShutDownMemory()
	{
#ifndef JINX_DEBUG_USE_STD_ALLOC
#ifndef JINX_DISABLE_POOL_ALLOCATOR
		std::lock_guard<std::recursive_mutex> lock(Impl::Mem::heapMutex);
		Impl::BlockHeap * heap = Impl::Mem::head;
		while (heap)
		{
			heap->ShutDown();
			heap = heap->Next();
		}
#endif
#endif
	}

	inline MemoryStats GetMemoryStats()
	{
		MemoryStats stats;
#ifndef JINX_DISABLE_POOL_ALLOCATOR
		stats.externalAllocCount = Impl::Mem::externalAllocCount;
		stats.externalFreeCount = Impl::Mem::externalFreeCount;
		stats.internalAllocCount = Impl::Mem::internalAllocCount;
		stats.internalFreeCount = Impl::Mem::internalFreeCount;
		stats.currentBlockCount = Impl::Mem::currentBlockCount;
		stats.currentAllocatedMemory = Impl::Mem::currentAllocatedMemory;
		stats.currentUsedMemory = Impl::Mem::currentUsedMemory;
#endif
		return stats;
	}

	inline void LogAllocations()
	{
#ifndef JINX_DEBUG_USE_STD_ALLOC
#ifndef JINX_DISABLE_POOL_ALLOCATOR
		std::lock_guard<std::recursive_mutex> lock(Impl::Mem::heapMutex);
		Impl::BlockHeap * heap = Impl::Mem::head;
		while (heap)
		{
			heap->LogAllocations();
			heap = heap->Next();
		}
#endif
#endif
	}


} // namespace Jinx



// end --- JxMemory.cpp --- 



// begin --- JxParser.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

namespace Jinx::Impl
{

	inline Parser::Parser(RuntimeIPtr runtime, const SymbolList & symbolList, const String & name, std::initializer_list<String> libraries) :
		m_runtime(runtime),
		m_name(name),
		m_symbolList(symbolList),
		m_lastLine(1),
		m_error(false),
		m_breakAddress(false),
		m_bytecode(CreateBuffer()),
		m_writer(m_bytecode)
	{
		m_currentSymbol = symbolList.begin();
		m_importList = libraries;
		if (EnableDebugInfo())
			m_debugLines.reserve(1024);
	}

	inline Parser::Parser(RuntimeIPtr runtime, const SymbolList & symbolList, const String & name) :
		m_runtime(runtime),
		m_name(name),
		m_symbolList(symbolList),
		m_lastLine(1),
		m_error(false),
		m_breakAddress(false),
		m_bytecode(CreateBuffer()),
		m_writer(m_bytecode)
	{
		m_currentSymbol = symbolList.begin();
	}

	inline bool Parser::Execute()
	{
		// Reserve 1K space
		m_bytecode->Reserve(1024);

		// Write bytecode header
		BytecodeHeader header;
		m_writer.Write(&header, sizeof(header));

		// Write script name
		m_writer.Write(m_name);

		// Parse script symbols into bytecode
		ParseScript();

		// Return error status
		return !m_error;
	}

	inline FunctionSignature Parser::ParseFunctionSignature(VisibilityType access, const String & libraryName)
	{
		m_library = m_runtime->GetLibraryInternal(libraryName);
		return ParseFunctionSignature(access, false);
	}

	inline String Parser::GetNameFromID(RuntimeID id) const
	{
		auto itr = m_idNameMap.find(id);
		if (itr == m_idNameMap.end())
			return String();
		return itr->second;
	}

	inline RuntimeID Parser::VariableNameToRuntimeID(const String & name)
	{
		auto id = GetVariableId(name.c_str(), name.size(), m_variableStackFrame.GetStackDepthFromName(name));
		m_idNameMap[id] = name;
		return id;
	}

	inline void Parser::VariableAssign(const String & name)
	{
		if (!m_variableStackFrame.VariableAssign(name))
			Error("%s", m_variableStackFrame.GetErrorMessage());
	}

	inline bool Parser::VariableExists(const String & name) const
	{
		return m_variableStackFrame.VariableExists(name);
	}

	inline void Parser::FrameBegin()
	{
		m_variableStackFrame.FrameBegin();
	}

	inline void Parser::FrameEnd()
	{
		if (!m_variableStackFrame.FrameEnd())
			Error("%", m_variableStackFrame.GetErrorMessage());
	}

	inline void Parser::ScopeBegin()
	{
		if (!m_variableStackFrame.ScopeBegin())
			Error("%s", m_variableStackFrame.GetErrorMessage());
		EmitOpcode(Opcode::ScopeBegin);
	}

	inline void Parser::ScopeEnd()
	{
		if (!m_variableStackFrame.ScopeEnd())
			Error("%s", m_variableStackFrame.GetErrorMessage());
		EmitOpcode(Opcode::ScopeEnd);
	}

	inline uint32_t Parser::GetOperatorPrecedence(Opcode opcode) const
	{
		switch (opcode)
		{
		case Opcode::Multiply: return 1;
		case Opcode::Divide: return 1;
		case Opcode::Mod: return 1;
		case Opcode::Add: return 2;
		case Opcode::Subtract: return 2;
		case Opcode::Less: return 3;
		case Opcode::LessEq: return 3;
		case Opcode::Greater: return 3;
		case Opcode::GreaterEq: return 3;
		case Opcode::Equals: return 3;
		case Opcode::NotEquals: return 3;
		case Opcode::And: return 4;
		case Opcode::Or: return 4;
		default:
			assert(!"Unknown opcode used in binary expression");
			return 0;
		};
	}

	inline bool Parser::IsSymbolValid(SymbolListCItr symbol) const
	{
		if (m_error)
			return false;
		if (symbol == m_symbolList.end())
			return false;
		if (symbol->type == SymbolType::NewLine)
			return false;
		return true;
	}

	inline bool Parser::IsLibraryName(const String & name) const
	{
		if (name == m_library->GetName())
			return true;
		for (const auto & n : m_importList)
		{
			if (name == n)
				return true;
		}
		return false;
	}

	inline bool Parser::IsPropertyName(const String & libraryName, const String & propertyName) const
	{
		if (!libraryName.empty())
		{
			auto library = m_runtime->GetLibraryInternal(libraryName);
			return library->PropertyNameExists(propertyName);
		}
		else
		{
			if (m_library->PropertyNameExists(propertyName))
				return true;
			for (const auto & n : m_importList)
			{
				auto library = m_runtime->GetLibraryInternal(n);
				if (library->PropertyNameExists(propertyName))
					return true;
			}
		}
		return false;
	}

	inline void Parser::EmitAddress(size_t address)
	{
		m_writer.Write(uint32_t(address));
	}

	inline size_t Parser::EmitAddressPlaceholder()
	{
		size_t offset = m_writer.Tell();
		m_writer.Write(uint32_t(0));
		return offset;
	}

	inline void Parser::EmitAddressBackfill(size_t address)
	{
		// This function is used to back-fill jump locations once we've parsed far enough to know
		// where a jump should land.

		// Retrieve current writer location
		size_t current = m_writer.Tell();
		// Seek to previous offset location
		m_writer.Seek(address);
		// Write the current location as the new jump offset location.
		m_writer.Write(static_cast<uint32_t>(current));
		// Restore the current writer location
		m_writer.Seek(current);
	}

	inline void Parser::EmitCount(uint32_t count)
	{
		m_writer.Write(count);
	}

	inline void Parser::EmitName(const String & name)
	{
		m_writer.Write(name);
	}

	inline void Parser::EmitOpcode(Opcode opcode)
	{
		// Only bother with writing this information if we're generating debug info
		if (EnableDebugInfo())
		{
			// Only write a new entry when we're at a new line, since that's all we're tracking
			if (m_debugLines.empty() || m_debugLines.back().lineNumber != m_lastLine)
			{
				auto pos = static_cast<uint32_t>(m_writer.Tell());
				m_debugLines.push_back({ pos, m_lastLine });
			}
		}
		m_writer.Write<Opcode, uint8_t>(opcode);
	}

	inline void Parser::EmitValue(const Variant & value)
	{
		value.Write(m_writer);
	}

	inline void Parser::EmitId(RuntimeID id)
	{
		m_writer.Write(id);
	}

	inline void Parser::EmitIndex(int32_t index)
	{
		m_writer.Write(index);
	}

	inline void Parser::EmitValueType(ValueType valueType)
	{
		m_writer.Write(ValueTypeToByte(valueType));
	}

	inline void Parser::WriteBytecodeHeader()
	{
		// Get bytecode data size
		size_t currentPos = m_writer.Tell();
		size_t bytecodeSize = currentPos - sizeof(BytecodeHeader);
		if (bytecodeSize > UINT_MAX)
		{
			Error("Bytecode data has exceeded maximum allowable size");
			return;
		}
		BytecodeHeader header;
		header.dataSize = static_cast<uint32_t>(bytecodeSize);
		m_writer.Seek(0);
		m_writer.Write(&header, sizeof(header));
		m_writer.Seek(currentPos);
	}

	inline void Parser::WriteDebugInfo()
	{
		DebugHeader opcodeHeader;
		opcodeHeader.lineEntryCount = static_cast<uint32_t>(m_debugLines.size());
		opcodeHeader.dataSize = static_cast<uint32_t>(m_debugLines.size() + sizeof(DebugLineEntry));
		m_writer.Write(&opcodeHeader, sizeof(opcodeHeader));
		for (const auto & lineEntry : m_debugLines)
			m_writer.Write(&lineEntry, sizeof(lineEntry));
	}

	inline void Parser::NextSymbol()
	{
		m_lastLine = m_currentSymbol->lineNumber;
		++m_currentSymbol;
	}

	inline bool Parser::Accept(SymbolType symbol)
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return false;
		if (symbol == m_currentSymbol->type)
		{
			NextSymbol();
			return true;
		}
		return false;
	}

	inline bool Parser::Expect(SymbolType symbol)
	{
		if (Accept(symbol))
			return true;
		Error("Expected symbol %s", GetSymbolTypeText(symbol));
		return false;
	}

	inline bool Parser::Check(SymbolType symbol) const
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return false;
		return (symbol == m_currentSymbol->type);
	}

	inline bool Parser::CheckBinaryOperator() const
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return false;
		auto type = m_currentSymbol->type;
		return
			type == SymbolType::And ||
			type == SymbolType::Asterisk ||
			type == SymbolType::Equals ||
			type == SymbolType::NotEquals ||
			type == SymbolType::ForwardSlash ||
			type == SymbolType::GreaterThan ||
			type == SymbolType::GreaterThanEquals ||
			type == SymbolType::LessThan ||
			type == SymbolType::LessThanEquals ||
			type == SymbolType::Minus ||
			type == SymbolType::Or ||
			type == SymbolType::Percent ||
			type == SymbolType::Plus;
	}

	inline bool Parser::CheckName() const
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return false;
		return m_currentSymbol->type == SymbolType::NameValue;
	}

	inline bool Parser::CheckValue() const
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return false;
		auto type = m_currentSymbol->type;
		return
			type == SymbolType::NumberValue ||
			type == SymbolType::IntegerValue ||
			type == SymbolType::BooleanValue ||
			type == SymbolType::StringValue ||
			type == SymbolType::Null;
	}

	inline bool Parser::CheckValueType() const
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return false;
		auto type = m_currentSymbol->type;
		return
			type == SymbolType::Number ||
			type == SymbolType::Integer ||
			type == SymbolType::Boolean ||
			type == SymbolType::String ||
			type == SymbolType::Collection ||
			type == SymbolType::Guid ||
			type == SymbolType::Null;
	}

	inline bool Parser::CheckFunctionNamePart() const
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return false;
		return (m_currentSymbol->type == SymbolType::NameValue) || IsKeyword(m_currentSymbol->type);
	}

	inline String Parser::CheckLibraryName() const
	{
		String libraryName;
		if (m_currentSymbol->type == SymbolType::NameValue || IsKeyword(m_currentSymbol->type))
		{
			String tokenName = m_currentSymbol->text;
			if (tokenName == m_library->GetName())
			{
				libraryName = m_library->GetName();
			}
			else
			{
				for (auto & importName : m_importList)
				{
					if (tokenName == importName)
					{
						libraryName = importName;
						break;
					}
				}
			}
		}
		return libraryName;
	}

	inline bool Parser::CheckFunctionCallPart(const FunctionSignatureParts & parts, size_t partsIndex, SymbolListCItr currSym, SymbolListCItr endSym, FunctionMatch & match) const
	{
		// If we reach the end of the parts list, return success
		if (partsIndex >= parts.size())
			return false;

		// If we've exceeded our range, return failure
		if (currSym == endSym)
			return false;

		const auto & part = parts[partsIndex];

		// Check for invalid symbol
		if (!IsSymbolValid(currSym))
			return part.optional && partsIndex == (parts.size() - 1);

		// Recursively iterate through all parts and match them against the signature.
		if (part.partType == FunctionSignaturePartType::Name)
		{
			if (!part.optional && currSym->type != SymbolType::NameValue && !IsKeyword(currSym->type))
				return false;

			for (const auto & name : part.names)
			{
				if (name == currSym->text)
				{
					match.partData.push_back(std::make_tuple(FunctionSignaturePartType::Name, 1, part.optional));
					auto newCurrSym = currSym;
					++newCurrSym;
					auto newMatch = match;
					if (CheckFunctionCallPart(parts, partsIndex + 1, newCurrSym, endSym, newMatch))
					{
						match = newMatch;
						return true;
					}
				}
			}

			if (part.optional)
			{
				auto newMatch = match;
				newMatch.partData.push_back(std::make_tuple(FunctionSignaturePartType::Name, 0, true));
				if (CheckFunctionCallPart(parts, partsIndex + 1, currSym, endSym, newMatch))
				{
					match = newMatch;
					return true;
				}
			}
		}
		else
		{
			// Set a flag if this is the initial token in the expression.
			bool isInitialToken = partsIndex >= match.partData.size();

			// Check for valid expressions
			size_t symCount = 0;
			if (CheckVariable(currSym, &symCount) || CheckProperty(currSym, &symCount))
			{
				for (size_t i = 0; i < symCount; ++i)
					++currSym;
			}
			else if (currSym->type == SymbolType::ParenOpen)
			{
				size_t parenCount = 1;
				symCount = 1;
				while (parenCount)
				{
					++currSym;
					++symCount;
					if (!IsSymbolValid(currSym))
						return false;
					if (currSym->type == SymbolType::ParenOpen)
						++parenCount;
					else if (currSym->type == SymbolType::ParenClose)
						--parenCount;
				}
				++currSym;
			}
			else if (currSym->type == SymbolType::ParenClose)
			{
				return false;
			}
			else if (IsConstant(currSym->type) || currSym->type == SymbolType::Not)
			{
				++currSym;
				symCount = 1;
			}
			else if (!isInitialToken && (IsOperator(currSym->type) || currSym->type == SymbolType::And || currSym->type == SymbolType::Or))
			{
				++currSym;
				symCount = 1;
			}
			else
			{
				if (partsIndex != 0)
				{
					FunctionMatch exprMatch = CheckFunctionCall(false, currSym, endSym);
					if (exprMatch.signature)
					{
						for (size_t i = 0; i < exprMatch.partData.size(); ++i)
						{
							++currSym;
							++symCount;
						}
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}

			// If our match structure isn't up to date, push new match items.  Otherwise,
			// advance our expression token count.  This will be important for determining
			// how many symbols we need to parse for an expression.
			if (isInitialToken)
				match.partData.push_back(std::make_tuple(FunctionSignaturePartType::Parameter, symCount, false));
			else
				std::get<1>(match.partData[partsIndex]) = std::get<1>(match.partData[partsIndex]) + symCount;

			// Store off current match structure
			auto newMatch = match;

			// Check to see if advancing part index leads to success
			if (CheckFunctionCallPart(parts, partsIndex + 1, currSym, endSym, newMatch))
			{
				match = newMatch;
				return true;
			}

			// Check symbols against the current part.
			if (CheckFunctionCallPart(parts, partsIndex, currSym, endSym, newMatch))
			{
				match = newMatch;
				return true;
			}
		}

		return match.partData.size() == parts.size();
	}

	inline Parser::FunctionMatch Parser::CheckFunctionCall(const FunctionSignature & signature, SymbolListCItr currSym, SymbolListCItr endSym, bool skipInitialParam) const
	{
		FunctionMatch match;
		const auto & parts = signature.GetParts();
		size_t partsIndex = 0;
		if (skipInitialParam && signature.GetParts()[0].partType == FunctionSignaturePartType::Parameter)
		{
			partsIndex++;
			match.partData.push_back(std::make_tuple(FunctionSignaturePartType::Parameter, 0, false));
		}
		if (CheckFunctionCallPart(parts, partsIndex, currSym, endSym, match))
		{
			match.signature = &signature;
			return match;
		}
		return FunctionMatch();
	}

	inline Parser::FunctionMatch Parser::CheckFunctionCall(const FunctionList & functionList, SymbolListCItr currSym, SymbolListCItr endSym, bool skipInitialParam) const
	{
		FunctionMatch match;
		for (const auto & functionSig : functionList)
		{
			auto currentSymbol = currSym;
			auto newMatch = CheckFunctionCall(functionSig, currentSymbol, endSym, skipInitialParam);
			if (newMatch.signature)
			{
				if (!match.signature || match.partData.size() < newMatch.partData.size())
					match = newMatch;
			}
		}
		return match;
	}

	inline Parser::FunctionMatch Parser::CheckFunctionCall(LibraryIPtr library, SymbolListCItr currSym, SymbolListCItr endSym, bool skipInitialParam) const
	{
		const auto functionPtrList = library->Functions();

		FunctionMatch match;
		for (const auto functionSig : functionPtrList)
		{
			auto currentSymbol = currSym;
			auto newMatch = CheckFunctionCall(*functionSig, currentSymbol, endSym, skipInitialParam);
			if (newMatch.signature)
			{
				if (!match.signature || match.partData.size() < newMatch.partData.size())
					match = newMatch;
			}
		}
		return match;
	}

	inline Parser::FunctionMatch Parser::CheckFunctionCall(bool skipInitialParam, SymbolListCItr currSym, SymbolListCItr endSym) const
	{
		FunctionMatch match;

		// Store current symbol
		auto currentSymbol = currSym;

		// Check for error or invalid symbols
		if (m_error || currentSymbol == m_symbolList.end())
			return match;

		// Any operators other than open parentheses mean this can't be a function call
		if (IsOperator(currentSymbol->type) && (currentSymbol->type != SymbolType::ParenOpen))
			return match;

		// Check for explicit library name in the first symbol.  libraryName is empty if not found.
		String libraryName = CheckLibraryName();

		// Advance the current symbol if we found a valid library name
		if (!libraryName.empty())
		{
			++currentSymbol;
			if (currentSymbol == m_symbolList.end())
				return match;
		}

		// If we explicitly specify a library name, then only look in that library
		if (!libraryName.empty())
		{
			auto library = m_runtime->GetLibraryInternal(libraryName);
			match = CheckFunctionCall(library, currentSymbol, endSym, skipInitialParam);
		}
		else
		{
			// Check local function table for signature match
			match = CheckFunctionCall(m_localFunctions, currentSymbol, endSym, skipInitialParam);

			// If not found in local function table, search in libraries for a function match
			if (!match.signature)
			{
				// Check the current library for a signature match
				match = CheckFunctionCall(m_library, currentSymbol, endSym, skipInitialParam);

				// If a library name isn't specified or a signature wasn't found, search first in current library, then in order of imports
				if (!match.signature)
				{
					// Search default library first
					auto library = m_runtime->GetLibraryInternal(libraryName);
					match = CheckFunctionCall(library, currentSymbol, endSym, skipInitialParam);

					// If function wasn't found in default library, search through all import libraries
					if (!match.signature)
					{
						// Loop through all imported library names
						for (const auto & libName : m_importList)
						{
							// Make sure the library exists
							if (!m_runtime->LibraryExists(libName))
							{
								LogWriteLine(LogLevel::Warning, "Unable to find library '%s'", libName.c_str());
								return FunctionMatch();
							}

							// Search for function in this library
							library = m_runtime->GetLibraryInternal(libName);
							auto newMatch = CheckFunctionCall(library, currentSymbol, endSym, skipInitialParam);
							if (newMatch.signature)
							{
								if (match.signature)
								{
									LogWriteLine(LogLevel::Warning, "Ambiguous function name detected.  Use library name to disambiguate.");
									return FunctionMatch();
								}
								else
								{
									match = newMatch;
									if (match.signature->GetVisibility() == VisibilityType::Private && library != m_library)
									{
										LogWriteLine(LogLevel::Error, "Unable to call library function with private scope.");
										return FunctionMatch();
									}
								}
							}
						}
					}
				}
			}
		}

		return match;
	}

	inline Parser::FunctionMatch Parser::CheckFunctionCall(bool skipInitialParam, SymbolListCItr endSym) const
	{
		return CheckFunctionCall(skipInitialParam, m_currentSymbol, endSym);
	}

	inline Parser::FunctionMatch Parser::CheckFunctionCall() const
	{
		return CheckFunctionCall(false, m_symbolList.end());
	}

	inline bool Parser::CheckVariable(SymbolListCItr currSym, size_t * symCount) const
	{
		if (m_error || currSym == m_symbolList.end())
			return false;
		if (currSym->type != SymbolType::NameValue)
			return false;

		// Check up to the max number of parts
		auto maxParts = m_variableStackFrame.GetMaxVariableParts();
		for (size_t s = maxParts; s > 0; --s)
		{
			auto curr = currSym;
			String name = curr->text;
			size_t sc = 1;
			bool error = false;
			for (size_t i = 1; i < s; ++i)
			{
				++curr;
				if (!IsSymbolValid(curr) || curr->text.empty())
				{
					error = true;
					break;
				}
				name += " ";
				name += curr->text;
				++sc;
			}
			if (error)
				continue;
			bool exists = VariableExists(name);
			if (exists)
			{
				if (symCount)
					*symCount = sc;
				return true;
			}
		}
		return false;

	}

	inline bool Parser::CheckVariable() const
	{
		return CheckVariable(m_currentSymbol);
	}

	inline bool Parser::CheckProperty(SymbolListCItr currSym, size_t * symCount) const
	{
		// Check symbol validity
		if (m_error || currSym == m_symbolList.end())
			return false;
		if (currSym->type != SymbolType::NameValue)
			return false;

		// Check to see if this begins with a library name
		String libraryName = CheckLibraryName();
		if (!libraryName.empty())
		{
			// Get next symbol and check validity
			auto currentSymbol = currSym;
			++currentSymbol;
			if (currentSymbol == m_symbolList.end())
				return false;
			if (currentSymbol->type != SymbolType::NameValue)
				return false;

			// Check for property name in this specific library
			auto library = m_runtime->GetLibraryInternal(libraryName);
			assert(library);
			return CheckPropertyName(library, currentSymbol, symCount);
		}

		// Check for property name in the current library
		assert(m_library);
		if (CheckPropertyName(m_library, currSym, symCount))
			return true;

		// Check against all imported libraries
		for (auto & importName : m_importList)
		{
			auto library = m_runtime->GetLibraryInternal(importName);
			if (library != m_library && CheckPropertyName(library, currSym, symCount))
				return true;
		}
		return false;
	}

	inline bool Parser::CheckProperty(size_t * symCount) const
	{
		return CheckProperty(m_currentSymbol, symCount);
	}

	inline bool Parser::CheckPropertyName(LibraryIPtr library, SymbolListCItr currSym, size_t * symCount) const
	{
		// Internal function called once we've established a library to check
		// Check up to the max number of parts

		// Initial error checks
		if (m_error || currSym == m_symbolList.end())
			return false;
		if (currSym->type != SymbolType::NameValue)
			return false;

		// Check for names starting with max property count
		auto maxParts = library->GetMaxPropertyParts();
		for (size_t s = maxParts; s > 0; --s)
		{
			auto curr = currSym;
			String name = curr->text;
			size_t sc = 1;
			for (size_t i = 1; i < s; ++i)
			{
				++curr;
				if (!IsSymbolValid(curr) || curr->text.empty())
					continue;
				name += " ";
				name += curr->text;
				++sc;
			}
			bool exists = library->PropertyNameExists(name);
			if (exists)
			{
				if (symCount)
					*symCount = sc;
				return true;
			}
		}
		return false;
	}

	inline VisibilityType Parser::ParseScope()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return VisibilityType::Local;
		if (m_currentSymbol->type == SymbolType::Private)
		{
			NextSymbol();
			return VisibilityType::Private;
		}
		if (m_currentSymbol->type == SymbolType::Public)
		{
			NextSymbol();
			return VisibilityType::Public;
		}
		return VisibilityType::Local;
	}

	inline Opcode Parser::ParseBinaryOperator()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return Opcode::NumOpcodes;

		Opcode opcode = Opcode::NumOpcodes;
		switch (m_currentSymbol->type)
		{
		case SymbolType::And:
			opcode = Opcode::And;
			break;
		case SymbolType::Asterisk:
			opcode = Opcode::Multiply;
			break;
		case SymbolType::Equals:
			opcode = Opcode::Equals;
			break;
		case SymbolType::NotEquals:
			opcode = Opcode::NotEquals;
			break;
		case SymbolType::ForwardSlash:
			opcode = Opcode::Divide;
			break;
		case SymbolType::GreaterThan:
			opcode = Opcode::Greater;
			break;
		case SymbolType::GreaterThanEquals:
			opcode = Opcode::GreaterEq;
			break;
		case SymbolType::LessThan:
			opcode = Opcode::Less;
			break;
		case SymbolType::LessThanEquals:
			opcode = Opcode::LessEq;
			break;
		case SymbolType::Minus:
			opcode = Opcode::Subtract;
			break;
		case SymbolType::Or:
			opcode = Opcode::Or;
			break;
		case SymbolType::Percent:
			opcode = Opcode::Mod;
			break;
		case SymbolType::Plus:
			opcode = Opcode::Add;
			break;
		default:
			Error("Unknown type when parsing binary operator");
			break;
		}
		NextSymbol();
		return opcode;
	}

	inline Variant Parser::ParseValue()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return Variant();
		Variant val;
		switch (m_currentSymbol->type)
		{
		case SymbolType::NumberValue:
			val.SetNumber(m_currentSymbol->numVal);
			break;
		case SymbolType::IntegerValue:
			val.SetInteger(m_currentSymbol->intVal);
			break;
		case SymbolType::BooleanValue:
			val.SetBoolean(m_currentSymbol->boolVal);
			break;
		case SymbolType::StringValue:
			val.SetString(m_currentSymbol->text);
			break;
		case SymbolType::Null:
			break;
		default:
			Error("Unknown value");
		}
		NextSymbol();
		return val;
	}

	inline ValueType Parser::ParseValueType()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return ValueType::Null;

		auto type = m_currentSymbol->type;
		NextSymbol();
		switch (type)
		{
		case SymbolType::Number:
			return ValueType::Number;
		case SymbolType::Integer:
			return ValueType::Integer;
		case SymbolType::Boolean:
			return ValueType::Boolean;
		case SymbolType::String:
			return ValueType::String;
		case SymbolType::Null:
			return ValueType::Null;
		case SymbolType::Collection:
			return ValueType::Collection;
		case SymbolType::Guid:
			return ValueType::Guid;
		default:
			Error("Unknown type");
			break;
		}
		return ValueType::Null;
	}

	inline String Parser::ParseName()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return String();
		if (m_currentSymbol->type != SymbolType::NameValue)
		{
			Error("Unexpected symbol type when parsing name");
			return String();
		}
		String s = m_currentSymbol->text;
		NextSymbol();
		return s;
	}

	inline String Parser::ParseMultiName(std::initializer_list<SymbolType> symbols)
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return String();
		if (m_currentSymbol->type != SymbolType::NameValue)
		{
			Error("Unexpected symbol type when parsing name");
			return String();
		}
		String s = m_currentSymbol->text;
		NextSymbol();

		while (IsSymbolValid(m_currentSymbol) && !m_currentSymbol->text.empty())
		{
			if (m_currentSymbol->type != SymbolType::NameValue)
			{
				for (auto symbol : symbols)
				{
					if (m_currentSymbol->type == symbol)
						return s;
				}
			}
			s += " ";
			s += m_currentSymbol->text;
			NextSymbol();
		}

		return s;
	}

	inline String Parser::ParseVariable()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return String();
		if (m_currentSymbol->type != SymbolType::NameValue)
		{
			Error("Unexpected symbol type when parsing variable");
			return String();
		}

		// Check up to the max number of parts until we find a variable match
		auto maxParts = m_variableStackFrame.GetMaxVariableParts();
		for (size_t s = maxParts; s > 0; --s)
		{
			auto curr = m_currentSymbol;
			String name = curr->text;
			size_t symbolCount = 1;
			for (size_t i = 1; i < s; ++i)
			{
				++curr;
				if (!IsSymbolValid(curr) || curr->text.empty())
					continue;
				name += " ";
				name += curr->text;
				++symbolCount;
			}
			bool exists = VariableExists(name);
			if (exists)
			{
				// Now that we know the longest variable count that matches, advance this number of symbols
				for (size_t i = 0; i < symbolCount; ++i)
					NextSymbol();

				// Return the variable name
				return name;
			}
		}
		Error("Could not parse variable name");
		return String();
	}

	inline void Parser::ParseSubscriptGet()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return;
		while (Accept(SymbolType::SquareOpen))
		{
			ParseExpression();
			Expect(SymbolType::SquareClose);
			EmitOpcode(Opcode::PushKeyVal);
		}
	}

	inline uint32_t Parser::ParseSubscriptSet()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return 0;
		uint32_t count = 0;
		while (Accept(SymbolType::SquareOpen))
		{
			ParseExpression();
			Expect(SymbolType::SquareClose);
			++count;
		}
		return count;
	}

	inline void Parser::ParsePropertyDeclaration(VisibilityType scope, bool readOnly)
	{
		if (m_error)
			return;

		if (m_currentSymbol == m_symbolList.end())
		{
			Error("Unexpected end of script when parsing property declaration");
			return;
		}
		if (m_currentSymbol->type != SymbolType::NameValue)
		{
			Error("Unexpected symbol type when parsing property declaration");
			return;
		}

		// Check if first keyword matches a library name
		for (auto libName : m_importList)
		{
			if (libName == m_currentSymbol->text)
			{
				Error("Property name cannot start with an import library name");
				return;
			}
		}

		// Find out which library this property belongs to
		auto propertyLibrary = m_library;
		auto libraryName = CheckLibraryName();
		if (!libraryName.empty())
		{
			propertyLibrary = m_runtime->GetLibraryInternal(libraryName);
			NextSymbol();
		}

		// Parse the property name
		if (!CheckName())
		{
			Error("Property name expected");
			return;
		}

		// Search for multi-part property names
		String name = ParseMultiName({ SymbolType::To });

		if (propertyLibrary->PropertyNameExists(name))
		{
			Error("Property is already defined");
			return;
		}

		// Create a PropertyName object for registration
		PropertyName propertyName(scope, readOnly, propertyLibrary->GetName(), name);

		// Register the property name, and check for duplicates
		if (!propertyLibrary->RegisterPropertyName(propertyName, true))
		{
			Error("Error registering property name.  Possible duplicate.");
			return;
		}

		// Set property value
		EmitOpcode(Opcode::Property);
		propertyName.Write(m_writer);

		if (Accept(SymbolType::To))
		{
			ParseExpression();
			EmitOpcode(Opcode::SetProp);
			EmitId(propertyName.GetId());
			m_idNameMap[propertyName.GetId()] = propertyName.GetName();
		}
		else if (readOnly)
		{
			// A declaration with no assignment is allowed, but not for a readonly property			
			Error("Must assign property an initial value");
			return;
		}
		Expect(SymbolType::NewLine);
	}

	inline PropertyName Parser::ParsePropertyName()
	{
		PropertyName propertyName;
		// Check to see if this begins with a library name
		auto library = m_library;
		String libraryName = CheckLibraryName();
		if (!libraryName.empty())
		{
			libraryName = ParseName();
			library = m_runtime->GetLibraryInternal(libraryName);
			propertyName = ParsePropertyNameParts(library);
			if (!propertyName.IsValid())
			{
				Error("Could not find property name");
				return PropertyName();
			}
			// Check for invalid scope
			if (m_library->GetName() != libraryName && propertyName.GetVisibility() != VisibilityType::Public)
			{
				Error("Unable to access private property");
				return PropertyName();
			}
		}
		// No library name, so we have to search for the best match
		else
		{
			// Check default library for property first
			propertyName = ParsePropertyNameParts(m_library);

			// Check import names if we can't find the property name locally.
			if (!propertyName.IsValid())
			{
				bool foundProperty = false;
				for (const auto & import : m_importList)
				{
					// Get import library by name
					library = m_runtime->GetLibraryInternal(import);

					// Don't bother checking the default library again
					if (library == m_library)
						continue;

					// Attempt to find valid import library name
					auto newPropertyName = ParsePropertyNameParts(library);
					if (newPropertyName.IsValid())
					{
						// If we haven't specified the library name explicitly, we can assume we're looking
						// for a different library.
						if (m_library->GetName() != libraryName && newPropertyName.GetVisibility() != VisibilityType::Public)
							continue;

						// Check for multiple found property names, which indicates this name is ambiguous
						if (foundProperty)
						{
							Error("Ambiguous property name found");
							return PropertyName();
						}
						foundProperty = true;
						propertyName = newPropertyName;
					}
				}

				// Check for invalid scope
				if (propertyName.IsValid() && library != m_library && propertyName.GetVisibility() != VisibilityType::Public)
				{
					Error("Unable to access private property");
					return PropertyName();
				}
			}
		}
		m_idNameMap[propertyName.GetId()] = propertyName.GetName();
		return propertyName;
	}

	inline PropertyName Parser::ParsePropertyNameParts(LibraryIPtr library)
	{
		// Check for initial errors
		if (m_error || m_currentSymbol == m_symbolList.end() || m_currentSymbol->type != SymbolType::NameValue)
			return PropertyName();

		// Check up to the max number of parts until we find a variable match
		auto maxParts = library->GetMaxPropertyParts();
		for (size_t s = maxParts; s > 0; --s)
		{
			auto curr = m_currentSymbol;
			String name = curr->text;
			size_t symbolCount = 1;
			for (size_t i = 1; i < s; ++i)
			{
				++curr;
				if (!IsSymbolValid(curr) || curr->text.empty())
					continue;
				name += " ";
				name += curr->text;
				++symbolCount;
			}
			bool exists = library->PropertyNameExists(name);
			if (exists)
			{
				// Now that we know the longest variable count that matches, advance this number of symbols
				for (size_t i = 0; i < symbolCount; ++i)
					NextSymbol();

				// Return the property name
				return library->GetPropertyName(name);
			}
		}
		return PropertyName();

	}

	inline String Parser::ParseFunctionNamePart()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return String();
		if (m_currentSymbol->text.empty())
		{
			Error("Unexpected symbol type when parsing function name");
			return String();
		}
		String s = m_currentSymbol->text;
		NextSymbol();
		return s;
	}

	inline FunctionSignature Parser::ParseFunctionSignature(VisibilityType scope, bool signatureOnly)
	{
		if (Check(SymbolType::NewLine))
		{
			Error("Empty function signature");
			return FunctionSignature();
		}
		bool parsedParameter = false;
		bool parsedNonKeywordName = false;
		int parsedNameCount = 0;
		int optionalNameCount = 0;
		int parsedNameCountSection = 0;
		int optionalNameCountSection = 0;
		FunctionSignatureParts signatureParts;
		while (!Check(SymbolType::NewLine))
		{
			FunctionSignaturePart part;
			if (Accept(SymbolType::CurlyOpen))
			{
				if (parsedParameter)
				{
					Error("Functions cannot have multiple variables without a name separating them");
					return FunctionSignature();
				}
				if (parsedNameCount && parsedNameCountSection <= optionalNameCountSection)
				{
					Error("Arguments can't be separated soley by optional parameters");
					return FunctionSignature();
				}
				part.partType = FunctionSignaturePartType::Parameter;
				if (CheckValueType())
				{
					part.valueType = ParseValueType();
				}
				if (CheckName())
				{
					String paramName = ParseMultiName({ SymbolType::CurlyClose });
					if (m_library->PropertyNameExists(paramName))
					{
						Error("Function parameter name '%s' already exists as a property name", paramName.c_str());
						return FunctionSignature();
					}
					part.names.push_back(paramName);
				}
				else if (signatureOnly)
				{
					Error("No variable name or class identifier found in function signature");
					return FunctionSignature();
				}
				Expect(SymbolType::CurlyClose);
				parsedParameter = true;
				parsedNameCountSection = 0;
				optionalNameCountSection = 0;
			}
			else
			{
				part.optional = Accept(SymbolType::ParenOpen);
				if (!CheckFunctionNamePart())
				{
					Error("Invalid name in function signature");
					return FunctionSignature();
				}
				parsedNameCount++;
				parsedNameCountSection++;
				if (IsKeyword(m_currentSymbol->type) == false)
					parsedNonKeywordName = true;
				part.partType = FunctionSignaturePartType::Name;
				part.names.push_back(ParseFunctionNamePart());
				while (Accept(SymbolType::ForwardSlash))
				{
					if (!CheckFunctionNamePart())
					{
						Error("Invalid name in function signature");
						return FunctionSignature();
					}
					auto name = ParseFunctionNamePart();
					for (auto n : part.names)
					{
						if (n == name)
						{
							Error("Duplicate alternative name in function signature");
							return FunctionSignature();
						}
					}
					part.names.push_back(name);
				}
				if (part.optional)
				{
					optionalNameCount++;
					optionalNameCountSection++;
					if (!Expect(SymbolType::ParenClose))
					{
						Error("Expected closing parentheses for optional function name part");
						return FunctionSignature();
					}
				}
				parsedParameter = false;
			}
			signatureParts.push_back(part);
		}
		if (!Expect(SymbolType::NewLine))
		{
			Error("Expected new line at end of function signature");
			return FunctionSignature();
		}

		// Check for function signature validity with matching keywords
		if (!parsedNonKeywordName)
		{
			if (parsedNameCount == 1 && signatureParts.size() == 1)
			{
				Error("Function signature cannot match keyword");
				return FunctionSignature();
			}
		}

		// Check to make sure the function has at least one non-optional keyword part
		if (parsedNameCount == optionalNameCount)
		{
			Error("Function signature must have at least one non-optional name part");
			return FunctionSignature();
		}

		// Create the function signature
		FunctionSignature signature(scope, m_library->GetName(), signatureParts);

		// This flag indicates that we're not generating bytecode, so no need to output that data.
		if (signatureOnly)
		{
			// Emit function definition opcode
			EmitOpcode(Opcode::Function);
			signature.Write(m_writer);
		}

		return signature;
	}

	inline void Parser::ParseFunctionDefinition(VisibilityType scope)
	{
		// Check to make sure we're at the root frame
		if (!m_variableStackFrame.IsRootFrame())
		{
			Error("Can't define a function inside another class or function");
			return;
		}

		// Check to make sure we're at the root scope
		if (!m_variableStackFrame.IsRootScope())
		{
			Error("Can't define a function inside a scoped execution block");
			return;
		}

		// Parse function signature
		FunctionSignature signature = ParseFunctionSignature(scope);
		if (!signature.IsValid())
		{
			Error("Invalid function definition");
			return;
		}

		// Check function scope type
		if (signature.GetVisibility() == VisibilityType::Local)
		{
			// Register function signature for local scope only
			auto itr = std::find(m_localFunctions.begin(), m_localFunctions.end(), signature);
			if (itr != m_localFunctions.end())
			{
				Error("Function already defined in script %s", m_library->GetName().c_str());
				return;
			}
			m_localFunctions.push_back(signature);
		}
		else
		{
			// Register function signature in library
			if (m_library->FunctionSignatureExists(signature))
			{
				Error("Function already defined in library %s", m_library->GetName().c_str());
				return;
			}
			m_library->RegisterFunctionSignature(signature);
		}

		// During initial execution, jump over code body
		EmitOpcode(Opcode::Jump);
		auto jumpBackfillAddress = EmitAddressPlaceholder();

		// Mark beginning of new execution frame
		FrameBegin();

		// Get function parameters
		FunctionSignatureParts params = signature.GetParameters();

		// We're indexing from the top of the stack.
		int32_t stackIndex = -1;

		// Assign parameter names expected on the call stack.  We assign them in reverse order since
		// they were pushed on the stack in order.
		for (FunctionSignatureParts::reverse_iterator itr = params.rbegin(); itr != params.rend(); ++itr)
		{

			VariableAssign(itr->names.front());
			EmitOpcode(Opcode::SetIndex);
			EmitId(VariableNameToRuntimeID(itr->names.front()));
			EmitIndex(stackIndex);
			EmitValueType(itr->valueType);
			--stackIndex;
		}

		// Parse the function body
		bool returnedValue = false;
		while (!Check(SymbolType::End) && !m_error)
		{
			if (ParseStatement())
				returnedValue = true;
		}
		Expect(SymbolType::End);
		Expect(SymbolType::NewLine);

		// Check to make sure we've returned a value as expected.
		if (!returnedValue)
		{
			EmitOpcode(Opcode::PushVal);
			EmitValue(nullptr);
			EmitOpcode(Opcode::Return);
		}

		// Backfill jump destination 
		EmitAddressBackfill(jumpBackfillAddress);

		// Mark end of execution frame
		FrameEnd();
	}

	inline void Parser::ParseFunctionCall(const FunctionMatch & match)
	{
		assert(match.signature);

		auto libName = CheckLibraryName();
		if (!libName.empty())
			NextSymbol();

		// Parse function components according to match data
		for (size_t i = 0; i < match.partData.size(); ++i)
		{
			if (std::get<0>(match.partData[i]) == FunctionSignaturePartType::Name)
			{
				// Check to make sure this isn't a skipped optional name part
				if (std::get<1>(match.partData[i]) != 0)
					NextSymbol();
			}
			else
			{
				if (Accept(SymbolType::ParenOpen))
				{
					ParseExpression();
					Expect(SymbolType::ParenClose);
				}
				else
				{
					auto expressionSize = std::get<1>(match.partData[i]);
					auto endSymbol = m_currentSymbol;
					for (size_t j = 0; j < expressionSize; ++j)
						++endSymbol;
					ParseExpression(endSymbol);
				}
			}
		}

		// When finished validating the function and pushing parameters, call the function
		EmitOpcode(Opcode::CallFunc);
		EmitId(match.signature->GetId());
		m_idNameMap[match.signature->GetId()] = match.signature->GetName();

		// Check for post-function index operator
		ParseSubscriptGet();
	}

	inline void Parser::ParseCast()
	{
		EmitOpcode(Opcode::Cast);
		auto valueType = ParseValueType();
		EmitValueType(valueType);
	}

	inline void Parser::ParseSubexpressionOperand(bool required, SymbolListCItr endSymbol)
	{
		if (m_error)
			return;

		if (Check(SymbolType::Comma) || Check(SymbolType::ParenClose) || Check(SymbolType::SquareClose) || Check(SymbolType::To) || Check(SymbolType::By))
		{
			if (required)
				Error("Expected operand");
			return;
		}
		else
		{
			bool negation = Accept(SymbolType::Minus);
			FunctionMatch functionMatch = CheckFunctionCall(false, endSymbol);
			if (functionMatch.signature)
			{
				ParseFunctionCall(functionMatch);
			}
			else if (Accept(SymbolType::ParenOpen))
			{
				ParseExpression();
				Expect(SymbolType::ParenClose);
			}
			else if (CheckProperty())
			{
				auto propertyName = ParsePropertyName();
				if (!propertyName.IsValid())
				{
					Error("Unable to find property name in library");
					return;
				}
				EmitOpcode(Opcode::PushProp);
				EmitId(propertyName.GetId());
				m_idNameMap[propertyName.GetId()] = propertyName.GetName();
				ParseSubscriptGet();
				if (Accept(SymbolType::Type))
					EmitOpcode(Opcode::Type);
			}
			else if (CheckVariable())
			{
				String name = ParseVariable();
				EmitOpcode(Opcode::PushVar);
				EmitId(VariableNameToRuntimeID(name));
				ParseSubscriptGet();
				if (Accept(SymbolType::Type))
					EmitOpcode(Opcode::Type);
			}
			else if (CheckValue())
			{
				auto val = ParseValue();
				EmitOpcode(Opcode::PushVal);
				EmitValue(val);
			}
			else if (CheckValueType())
			{
				auto val = ParseValueType();
				EmitOpcode(Opcode::PushVal);
				EmitValue(val);
			}
			else
			{
				// If we can't find a valid function call within this end symbol, try
				// a match without those limits.  This gives priority to initial expressions,
				// but still allows functions to parse correctly if no valid expressions
				// otherwise exist.
				endSymbol = m_symbolList.end();
				functionMatch = CheckFunctionCall(false, endSymbol);
				if (functionMatch.signature)
				{
					ParseFunctionCall(functionMatch);
				}
				else
				{
					Error("Expected operand");
				}
			}
			if (negation)
				EmitOpcode(Opcode::Negate);
		}
	}

	inline void Parser::ParseSubexpression(SymbolListCItr endSymbol)
	{
		if (m_error)
			return;

		// Make sure we have a valid expression
		if (Check(SymbolType::NewLine))
		{
			Error("Expected valid expression");
			return;
		}

		// Opcode stack for operators 
		std::vector<Opcode, Allocator<Opcode>> opcodeStack;

		bool requiredOperand = false;
		std::vector<size_t, Allocator<size_t>> jumpAddrStack;

		bool notOp = false;
		while (IsSymbolValid(m_currentSymbol) && m_currentSymbol->type != SymbolType::NewLine && m_currentSymbol != endSymbol)
		{
			// Check for a unary negation operator
			if (Accept(SymbolType::Not))
			{
				notOp = !notOp;
				if (Check(SymbolType::Not))
				{
					Error("More than one consecutive not operator is not permitted");
					return;
				}
			}

			// Parse operand
			ParseSubexpressionOperand(requiredOperand, endSymbol);
			requiredOperand = false;

			// Check for casts
			if (Accept(SymbolType::As))
				ParseCast();

			// Parse binary operator
			if (CheckBinaryOperator())
			{
				requiredOperand = true;
				auto opcode = ParseBinaryOperator();

				// Check precedence if we've already parsed a binary math expression
				while (!opcodeStack.empty() && GetOperatorPrecedence(opcode) >= GetOperatorPrecedence(opcodeStack.back()))
				{
					EmitOpcode(opcodeStack.back());
					opcodeStack.pop_back();
				}

				// Emit short-circuit evaluation jump check with placeholder address
				if (opcode == Opcode::And || opcode == Opcode::Or)
				{
					// Emit not opcode if required
					if (notOp)
					{
						EmitOpcode(Opcode::Not);
						notOp = false;
					}

					EmitOpcode(opcode == Opcode::And ? Opcode::JumpFalseCheck : Opcode::JumpTrueCheck);
					jumpAddrStack.push_back(EmitAddressPlaceholder());
				}

				opcodeStack.push_back(opcode);
			}
			else if (!opcodeStack.empty())
			{
				while (!opcodeStack.empty())
				{
					EmitOpcode(opcodeStack.back());
					opcodeStack.pop_back();
				}
			}
			else
			{
				break;
			}
		}

		// Emit not opcode if required
		if (notOp)
			EmitOpcode(Opcode::Not);

		// Check for chained function calls
		while (true)
		{
			const auto match = CheckFunctionCall(true, endSymbol);
			if (match.signature)
			{
				if (match.signature->GetParts()[0].partType != FunctionSignaturePartType::Parameter)
				{
					Error("Missing operator before function '%s'", match.signature->GetName().c_str());
					return;
				}
				ParseFunctionCall(match);
			}
			else
				break;
		}

		// Backfill any short-circuit test jump address now that we're finished with local expression
		while (!jumpAddrStack.empty())
		{
			EmitAddressBackfill(jumpAddrStack.back());
			jumpAddrStack.pop_back();
		}

		// Check for leftover operators
		if (!opcodeStack.empty())
		{
			Error("Syntax error when parsing expression");
		}

	}

	inline void Parser::ParseSubexpression()
	{
		ParseSubexpression(m_symbolList.end());
	}

	inline void Parser::ParseExpression(SymbolListCItr endSymbol)
	{
		// Check first for an opening bracket, which indicates either an index operator or a key-value pair.
		if (Accept(SymbolType::SquareOpen))
		{
			if (Accept(SymbolType::SquareClose))
			{
				// If we immediately see a close bracket, create an empty collection
				EmitOpcode(Opcode::PushColl);
				EmitCount(0);
			}
			else
			{
				ParseSubexpression(endSymbol);

				// If we see a comma after a square open bracket, we're parsing a key-value pair
				if (Accept(SymbolType::Comma))
				{
					ParseExpression(endSymbol);
					Expect(SymbolType::SquareClose);

					// Parse all subsequent key-value pairs
					uint32_t count = 1;
					while (Accept(SymbolType::Comma))
					{
						Accept(SymbolType::NewLine);
						Expect(SymbolType::SquareOpen);
						ParseSubexpression(endSymbol);
						Expect(SymbolType::Comma);
						ParseSubexpression(endSymbol);
						Expect(SymbolType::SquareClose);
						++count;
					}

					// Pop all key-value pairs and push a new collection onto the stack
					EmitOpcode(Opcode::PushColl);
					EmitCount(count);
				}
				else
				{
					Error("Expected comma separating key-value pair");
				}
			}
		}
		else
		{
			// Parse the first subexpression, defined as any normal expression excluding index operators or lists, 
			// which are handled in this function
			ParseSubexpression(endSymbol);

			// If we finish the first subexpression with a common, then we're parsing an indexed list
			if (Accept(SymbolType::Comma))
			{
				// Parse all subexpressions in comma-delimited list
				uint32_t count = 1;
				do
				{
					Accept(SymbolType::NewLine);
					ParseSubexpression(endSymbol);
					++count;
				} 
				while (Accept(SymbolType::Comma));

				// Pop all key-value pairs and push the results on the stack
				EmitOpcode(Opcode::PushList);
				EmitCount(count);
			}
		}
	}

	inline void Parser::ParseExpression()
	{
		ParseExpression(m_symbolList.end());
	}

	inline void Parser::ParseErase()
	{
		if (CheckProperty())
		{
			auto propName = ParsePropertyName();
			if (propName.IsReadOnly())
			{
				Error("Can't erase a readonly property");
				return;
			}
			uint32_t subscripts = ParseSubscriptSet();
			Expect(SymbolType::NewLine);
			if (subscripts)
			{
				EmitOpcode(Opcode::ErasePropKeyVal);
				EmitCount(subscripts);
				EmitId(propName.GetId());
			}
			else
			{
				Error("Expected index operator after erase keyword");
				return;
			}
		}
		else if (CheckVariable())
		{
			auto varName = ParseVariable();
			uint32_t subscripts = ParseSubscriptSet();
			Expect(SymbolType::NewLine);
			if (subscripts)
			{
				EmitOpcode(Opcode::EraseVarKeyVal);
				EmitCount(subscripts);
				EmitId(VariableNameToRuntimeID(varName));
			}
			else
			{
				EmitOpcode(Opcode::EraseItr);
				EmitId(VariableNameToRuntimeID(varName));
			}
		}
		else
		{
			Error("Valid property or variable name expected after erase keyword");
			return;
		}
	}

	inline void Parser::ParseIncDec()
	{
		bool increment = Accept(SymbolType::Increment);
		if (!increment)
			Expect(SymbolType::Decrement);
		PropertyName propName;
		String varName;
		if (CheckProperty())
		{
			propName = ParsePropertyName();
			if (propName.IsReadOnly())
			{
				Error("Can't %s a readonly property", increment ? "increment" : "decrement");
				return;
			}
			EmitOpcode(Opcode::PushProp);
			EmitId(propName.GetId());
			m_idNameMap[propName.GetId()] = propName.GetName();
		}
		else if (CheckVariable())
		{
			varName = ParseVariable();
			EmitOpcode(Opcode::PushVar);
			EmitId(VariableNameToRuntimeID(varName));
		}
		else
		{
			Error("Valid property or variable name expected after %s keyword", increment ? "increment" : "decrement");
			return;
		}
		if (Accept(SymbolType::By))
		{
			ParseExpression();
		}
		else
		{
			EmitOpcode(Opcode::PushVal);
			EmitValue(1);
		}
		EmitOpcode(increment ? Opcode::Increment : Opcode::Decrement);
		if (!propName.GetName().empty())
		{
			EmitOpcode(Opcode::SetProp);
			EmitId(propName.GetId());
			m_idNameMap[propName.GetId()] = propName.GetName();
		}
		else
		{
			EmitOpcode(Opcode::SetVar);
			EmitId(VariableNameToRuntimeID(varName));
		}
		Expect(SymbolType::NewLine);
	}

	inline void Parser::ParseIfElse()
	{
		// Parse expression after the if keyword
		ParseExpression();
		Expect(SymbolType::NewLine);

		// Add jump instruction, making sure to store the jump address for later backfilling
		EmitOpcode(Opcode::JumpFalse);
		auto ifJumpAddress = EmitAddressPlaceholder();

		// Parse new block of code after if line
		ParseBlock();

		// Check to see if we continue with 'else' or 'end' the if block
		if (Accept(SymbolType::Else))
		{
			// Set a jump statement before the else for the end of the if-true execution block
			EmitOpcode(Opcode::Jump);
			auto elseJumpAddress = EmitAddressPlaceholder();

			// Backfill jump target address after we've reserved space for a new jump
			EmitAddressBackfill(ifJumpAddress);

			// We should now see either an endline or another if symbol.
			// If we see an endline, then we just have an else block
			if (Accept(SymbolType::NewLine))
			{
				// Parse the 'else' block
				ParseBlock();

				// Check that the block ends properly
				Expect(SymbolType::End);
				Expect(SymbolType::NewLine);
			}
			else if (Accept(SymbolType::If))
			{
				ParseIfElse();
			}
			else
			{
				Error("Unexpected symbol after else");
			}

			// Backfill else jump
			EmitAddressBackfill(elseJumpAddress);
		}
		else if (Accept(SymbolType::End))
		{
			// We're expecting a newline after the 'end' keyword
			Expect(SymbolType::NewLine);

			// Backfill if-true jump target address
			EmitAddressBackfill(ifJumpAddress);
		}
		else
		{
			Error("Missing block termination after if");
		}
	}

	inline void Parser::ParseLoop()
	{
		// Check to see if we're using an explicitly named variable for the loop counter
		String name;
		if (CheckName())
		{
			// Parse initial name part
			name = ParseMultiName({ SymbolType::From, SymbolType::Over, SymbolType::Until, SymbolType::While });

			// Make sure the variable name doesn't already exist
			if (VariableExists(name))
			{
				Error("Variable name '%s' already exists in this scope", name.c_str());
				return;
			}
		}

		// We're looping using a counter
		if (Accept(SymbolType::From))
		{
			// Begin scope for loop control variables
			ScopeBegin();

			// Parse from value
			ParseExpression();

			// Assign the counter to a variable name if it exists
			if (!name.empty())
			{
				VariableAssign(name);
				EmitOpcode(Opcode::SetVar);
				EmitId(VariableNameToRuntimeID(name));
			}

			// Parse to value
			Expect(SymbolType::To);
			ParseExpression();

			// Parse increment amount
			if (Accept(SymbolType::By))
			{
				ParseExpression();
			}
			else
			{
				EmitOpcode(Opcode::PushVal);
				EmitValue(nullptr);
			}
			Expect(SymbolType::NewLine);

			// Mark where the loop count evaluation has to jump
			auto loopBeginAddress = m_writer.Tell();

			// Evaluate code block inside loop
			ParseBlock();
			Expect(SymbolType::End);
			Expect(SymbolType::NewLine);

			// Advance counter and evaluate
			EmitOpcode(Opcode::LoopCount);

			// Evaluate result of loop count instruction
			EmitOpcode(Opcode::JumpTrue);
			EmitAddress(loopBeginAddress);

			// End loop scope
			ScopeEnd();
		}
		// We're looping over a collection
		else if (Accept(SymbolType::Over))
		{
			// Begin scope for loop control variables
			ScopeBegin();

			// Parse the collection
			ParseExpression();
			if (!Expect(SymbolType::NewLine))
				return;

			// Check to see if the collection is empty, and if so, skip the loop
			EmitOpcode(Opcode::PushTop);
			EmitOpcode(Opcode::JumpFalse);
			auto emptyLoopJumpAddress = EmitAddressPlaceholder();

			// Retrieve collection from top of stack and push iterator from beginning position
			EmitOpcode(Opcode::PushItr);

			// Assign the iterator to a variable name if it exists
			if (!name.empty())
			{
				VariableAssign(name);
				EmitOpcode(Opcode::SetVar);
				EmitId(VariableNameToRuntimeID(name));
			}

			// Store where the loop logic begins
			auto loopBeginAddress = m_writer.Tell();

			// Parse the while loop block
			ParseBlock();
			Expect(SymbolType::End);
			Expect(SymbolType::NewLine);

			// Increment iterator and test against collection end
			EmitOpcode(Opcode::LoopOver);
			EmitOpcode(Opcode::JumpFalse);
			EmitAddress(loopBeginAddress);

			// Backfill empty loop jump address
			EmitAddressBackfill(emptyLoopJumpAddress);

			// End loop scope
			ScopeEnd();

		}
		// Loops while a condition is true or false
		else if (Check(SymbolType::Until) || Check(SymbolType::While))
		{
			// Store where the loop logic begins
			auto loopBeginAddress = m_writer.Tell();

			// Check for keyword
			bool jumpTrue = Accept(SymbolType::Until);
			if (!jumpTrue)
				Expect(SymbolType::While);

			// Parse the expression to control the loop's jump branch
			ParseExpression();
			if (!Expect(SymbolType::NewLine))
				return;

			// Add jump instruction, making sure to store the jump address for later backfilling
			EmitOpcode(jumpTrue ? Opcode::JumpTrue : Opcode::JumpFalse);
			auto loopJumpAddress = EmitAddressPlaceholder();

			// Parse the while loop block
			ParseBlock();
			Expect(SymbolType::End);
			Expect(SymbolType::NewLine);

			// Jump to top of loop
			EmitOpcode(Opcode::Jump);
			EmitAddress(loopBeginAddress);

			// Backfill loop jump target address
			EmitAddressBackfill(loopJumpAddress);
		}
		// Executes once and then loops again while a condition is true/false, depending on keyword used
		else if (Accept(SymbolType::NewLine))
		{
			// Store where the loop logic begins
			auto loopBeginAddress = m_writer.Tell();

			// Parse the until/while loop block
			ParseBlock();

			// Check the keyword is used
			bool jumpTrue = Accept(SymbolType::While);
			if (!jumpTrue)
				Expect(SymbolType::Until);

			// Parse expression used to determine if loop should execute again
			ParseExpression();
			Expect(SymbolType::NewLine);

			// Conditionally jump to top of loop
			EmitOpcode(jumpTrue ? Opcode::JumpTrue : Opcode::JumpFalse);
			EmitAddress(loopBeginAddress);
		}
		else
		{
			Error("Unknown syntax after loop keyword");
			return;
		}

		// If we used a break somewhere inside the loop, backfill the address now
		if (m_breakAddress)
		{
			EmitAddressBackfill(m_breakAddress);
			m_breakAddress = 0;
		}
	}

	inline bool Parser::ParseStatement()
	{
		// No need to continue if an error has been flagged
		if (m_error)
			return false;

		bool returnedValue = false;

		// Functions signatures have precedence over everything, so check for a 
		// potential signature match before anything else.
		const auto functionMatch = CheckFunctionCall();
		if (functionMatch.signature)
		{
			// We found a valid function signature that matches the current token(s)
			ParseFunctionCall(functionMatch);

			// Since all functions return a value, we need to discard the return
			// value not on the stack, since we're not assigning it to a variable.
			EmitOpcode(Opcode::Pop);
			Expect(SymbolType::NewLine);
		}
		else
		{
			bool set = Accept(SymbolType::Set);

			// Parse scope level
			VisibilityType scope = ParseScope();

			// Parse optional readonly, which can only apply to properties
			bool readOnly = Accept(SymbolType::Readonly);
			if (readOnly)
			{
				if (scope == VisibilityType::Local)
				{
					Error("The 'readonly' keyword must follow a private or public keyword");
					return false;
				}
			}

			if (Accept(SymbolType::Function))
			{
				// We're parsing a function definition
				ParseFunctionDefinition(scope);
			}
			else if (set && CheckName())
			{
				// Can't use the current library name or preface the variable with it
				if (m_currentSymbol->text == m_library->GetName())
				{
					Error("Illegal use of library name in identifier");
					return false;
				}

				// We're declaring a new property if we see a non-local scope declaration
				if (scope != VisibilityType::Local)
				{
					ParsePropertyDeclaration(scope, readOnly);
				}
				// Either this is an existing property or a variable
				else
				{
					// Check to see if this is an existing property
					if (CheckProperty())
					{
						// Get the property name
						auto propertyName = ParsePropertyName();

						// Make sure we're not trying to assign a value to a readonly property
						if (propertyName.IsReadOnly())
						{
							Error("Can't change readonly property");
							return false;
						}

						// Check for subscript operators
						uint32_t subscripts = ParseSubscriptSet();

						// Check for a 'to' statement
						Expect(SymbolType::To);

						// Parse assignment expression
						ParseExpression();
						Expect(SymbolType::NewLine);

						// Assign property
						if (subscripts)
						{
							EmitOpcode(Opcode::SetPropKeyVal);
							EmitCount(subscripts);
						}
						else
							EmitOpcode(Opcode::SetProp);
						EmitId(propertyName.GetId());
						m_idNameMap[propertyName.GetId()] = propertyName.GetName();
					}
					// Otherwise we're just dealing with an ordinary variable
					else
					{
						// Get the variable name
						String name = ParseMultiName({ SymbolType::To, SymbolType::SquareOpen });

						// Check for subscript operator
						uint32_t subscripts = ParseSubscriptSet();

						// Check for a 'to' statement
						Expect(SymbolType::To);

						// Parse assignment expression
						ParseExpression();
						Expect(SymbolType::NewLine);

						// Add to variable table
						VariableAssign(name);

						// Assign a variable. 
						if (subscripts)
						{
							EmitOpcode(Opcode::SetVarKeyVal);
							EmitCount(subscripts);
						}
						else
							EmitOpcode(Opcode::SetVar);
						EmitId(VariableNameToRuntimeID(name));
					}
				}
			}
			else if (scope == VisibilityType::Local)
			{
				if (Accept(SymbolType::Begin))
				{
					// We're parsing a begin/end block
					Expect(SymbolType::NewLine);
					ParseBlock();
					Expect(SymbolType::End);
					Expect(SymbolType::NewLine);
				}
				else if (Accept(SymbolType::If))
				{
					// We're parsing an if or if/else block
					ParseIfElse();
				}
				else if (Accept(SymbolType::Loop))
				{
					// We're parsing a loop block
					ParseLoop();
				}
				else if (Accept(SymbolType::Erase))
				{
					// We're parsing an erase operation
					ParseErase();
				}
				else if (Check(SymbolType::Increment) || Check(SymbolType::Decrement))
				{
					// We're parsing an increment or decrement statement
					ParseIncDec();
				}
				else if (Accept(SymbolType::Return))
				{
					// We've hit a return value.  There are different behaviors depending whether or
					// not we're at the base scope or not.
					if (m_variableStackFrame.IsRootFrame())
					{
						if (!Check(SymbolType::NewLine))
						{
							LogWriteLine(LogLevel::Warning, "Return values at root scope do nothing");
							ParseExpression();
						}
						EmitOpcode(Opcode::Exit);
						Accept(SymbolType::NewLine);
						returnedValue = true;
					}
					else
					{
						if (!Check(SymbolType::NewLine))
						{
							ParseExpression();
						}
						else
						{
							EmitOpcode(Opcode::PushVal);
							EmitValue(nullptr);
						}
						EmitOpcode(Opcode::Return);
						Accept(SymbolType::NewLine);
						returnedValue = true;
					}
				}
				else if (Accept(SymbolType::Break))
				{
					// We've hit a break statement
					Expect(SymbolType::NewLine);
					EmitOpcode(Opcode::Jump);
					m_breakAddress = EmitAddressPlaceholder();
				}
				else if (Accept(SymbolType::Wait))
				{
					// Check for basic wait statement
					if (Accept(SymbolType::NewLine))
						EmitOpcode(Opcode::Wait);
					else if (Check(SymbolType::Until) || Check(SymbolType::While))
					{
						// Store expression address
						auto expressionAddress = m_writer.Tell();

						// Check which keyword was used
						bool jumpTrue = Accept(SymbolType::Until);
						if (!jumpTrue)
							Expect(SymbolType::While);

						// Parse the expression to check for wait
						ParseExpression();
						if (!Expect(SymbolType::NewLine))
							return false;

						// Add jump if false/true over wait statement depending on keyword
						EmitOpcode(jumpTrue ? Opcode::JumpTrue : Opcode::JumpFalse);

						// Mark placeholder for later jump address insertion
						auto addressPlaceholder = EmitAddressPlaceholder();

						// Wait statement is executed if expression is true
						EmitOpcode(Opcode::Wait);
						EmitOpcode(Opcode::Jump);
						EmitAddress(expressionAddress);

						// Backfill placeholder at end of conditional wait statement
						EmitAddressBackfill(addressPlaceholder);
					}
					else
					{
						Error("Unexpected symbol after wait");
					}
				}
				else if (Accept(SymbolType::External))
				{
					// First check to see if this collides with an existing property name
					bool propExists = CheckProperty();

					// Get the variable name
					String name = ParseMultiName({ });

					// Validate the name is legal and register it as a variable name
					if (!m_variableStackFrame.IsRootFrame())
						Error("External variable '%s' can't be declared in a function", name.c_str());
					else if (!m_variableStackFrame.IsRootScope())
						Error("External variable '%s' must be declared at the root scope", name.c_str());
					else if (propExists)
						Error("External variable '%s' is already a property name", name.c_str());
					else if (m_variableStackFrame.VariableExists(name))
						Error("Variable '%s' already exists", name.c_str());
					else if (!m_variableStackFrame.VariableAssign(name))
						Error("Error creating external variable '%s'", name.c_str());

					Expect(SymbolType::NewLine);
				}
				else
				{
					Error("Unknown symbol in statement");
				}
			}
			else
			{
				Error("Invalid symbol after scope specifier '%s'", scope == VisibilityType::Public ? "public" : "private");
			}
		}

		// This is not an error value, but signals whether we've returned a value from this statement
		return returnedValue;
	}

	inline void Parser::ParseBlock()
	{
		if (m_error)
			return;

		// Push local variable stack frame
		ScopeBegin();

		// Parse each statement until we reach a terminating symbol
		while (!(Check(SymbolType::End) || Check(SymbolType::Else) || Check(SymbolType::Until) || Check(SymbolType::While)) && !m_error)
			ParseStatement();

		// Pop local variable stack frame
		ScopeEnd();
	}

	inline void Parser::ParseLibraryImports()
	{
		while (true)
		{
			if (!Accept(SymbolType::Import))
				break;
			auto name = ParseName();
			if (name.empty())
			{
				Error("Expected valid name after 'import' keyword");
				return;
			}
			Expect(SymbolType::NewLine);

			// Check to make sure we're not adding duplicates
			bool foundDup = false;
			for (auto & importName : m_importList)
			{
				if (importName == name)
				{
					foundDup = true;
					continue;
				}
			}

			// Add library to the list of imported libraries for this script
			if (!foundDup)
			{
				m_importList.push_back(name);
				if (!m_runtime->LibraryExists(name))
				{
					Error("Could not find library named '%s'", name.c_str());
					break;
				}
			}
		}
	}

	inline void Parser::ParseLibraryDeclaration()
	{
		if (m_library != nullptr)
		{
			Error("Library has already been declared for this script");
			return;
		}
		String libraryName;
		if (Accept(SymbolType::Library))
		{
			libraryName = ParseName();
			if (libraryName.empty())
			{
				Error("Expected valid name after 'library' keyword");
				return;
			}
			Expect(SymbolType::NewLine);
			m_library = m_runtime->GetLibraryInternal(libraryName);
		}

		// Emit library declaration bytecode
		EmitOpcode(Opcode::Library);
		EmitName(libraryName);

		// Retrieve library interface by name
		m_library = m_runtime->GetLibraryInternal(libraryName);
	}

	inline void Parser::ParseScript()
	{
		ParseLibraryImports();
		ParseLibraryDeclaration();
		while (m_currentSymbol != m_symbolList.end() && !m_error)
			ParseStatement();
		if (m_breakAddress)
			Error("Illegal break");
		EmitOpcode(Opcode::Exit);
		WriteBytecodeHeader();
		if (EnableDebugInfo())
			WriteDebugInfo();
	}

} // namespace Jinx::Impl



// end --- JxParser.cpp --- 



// begin --- JxPropertyName.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

namespace Jinx::Impl
{

	inline PropertyName::PropertyName()
	{
	}

	inline PropertyName::PropertyName(VisibilityType visibility, bool readOnly, const String & moduleName, const String & propertyName, const Variant & defaultValue) :
		m_visibility(visibility),
		m_readOnly(readOnly),
		m_name(propertyName),
		m_defaultValue(defaultValue)
	{
		String combinedName = moduleName + " " + propertyName;
		m_id = GetHash(combinedName.c_str(), combinedName.size());
		m_partCount = GetNamePartCount(propertyName);
	}

	inline void PropertyName::Read(BinaryReader & reader)
	{
		reader.Read(&m_id);
		reader.Read(&m_readOnly);
		reader.Read(&m_name);
		reader.Read<VisibilityType, uint8_t>(&m_visibility);
		reader.Read<size_t, uint16_t>(&m_partCount);
		m_defaultValue.Read(reader);
	}

	inline void PropertyName::Write(BinaryWriter & writer) const
	{
		writer.Write(m_id);
		writer.Write(m_readOnly);
		writer.Write(m_name);
		writer.Write<VisibilityType, uint8_t>(m_visibility);
		writer.Write<size_t, uint16_t>(m_partCount);
		m_defaultValue.Write(writer);
	}

} // namespace Jinx::Impl



// end --- JxPropertyName.cpp --- 



// begin --- JxRuntime.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

namespace Jinx::Impl
{

	inline Runtime::Runtime()
	{
		m_perfStartTime = std::chrono::high_resolution_clock::now();

		// Build symbol type map, excluding symbols without a text representation
		for (size_t i = static_cast<size_t>(SymbolType::ForwardSlash); i < static_cast<size_t>(SymbolType::NumSymbols); ++i)
		{
			SymbolType symType = static_cast<SymbolType>(i);
			auto symTypeText = GetSymbolTypeText(symType);
			m_symbolTypeMap.insert(std::make_pair(String(symTypeText), symType));
		}
	}

	inline Runtime::~Runtime()
	{
		// Clear potential circular references by explicitly destroying collection values
		for (auto & s : m_propertyMap)
		{
			if (s.second.IsCollection())
			{
				auto c = s.second.GetCollection();
				for (auto & e : *c)
				{
					e.second.SetNull();
				}
			}
		}
	}

	inline void Runtime::AddPerformanceParams(bool finished, uint64_t timeNs, uint64_t instCount)
	{
		std::lock_guard<std::mutex> lock(m_perfMutex);
		m_perfStats.executionTimeNs += timeNs;
		m_perfStats.instructionCount += instCount;
		m_perfStats.scriptExecutionCount++;
		if (finished)
			m_perfStats.scriptCompletionCount++;
	}

	inline BufferPtr Runtime::Compile(BufferPtr scriptBuffer, String name, std::initializer_list<String> libraries)
	{
		// Mark script execution start time
		auto begin = std::chrono::high_resolution_clock::now();

		// Lex script text into tokens
		Lexer lexer(m_symbolTypeMap, reinterpret_cast<const char *>(scriptBuffer->Ptr()), reinterpret_cast<const char *>(scriptBuffer->Ptr() + scriptBuffer->Size()), name);

		// Exit if errors when lexing
		if (!lexer.Execute())
			return nullptr;

		// Log all lexed symbols for development and debug purposes
		if (IsLogSymbolsEnabled())
			LogSymbols(lexer.GetSymbolList());

		// Create parser with symbol list
		Parser parser(shared_from_this(), lexer.GetSymbolList(), name, libraries);

		// Generate bytecode from symbol list
		if (!parser.Execute())
			return nullptr;

		// Log bytecode for development and debug purposes
		if (IsLogBytecodeEnabled())
			LogBytecode(parser);

		// Track accumulated script compilation time and count
		auto end = std::chrono::high_resolution_clock::now();
		uint64_t compilationTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
		std::lock_guard<std::mutex> lock(m_perfMutex);
		m_perfStats.scriptCompilationCount++;
		m_perfStats.compilationTimeNs += compilationTimeNs;

		// Return the bytecode
		return parser.GetBytecode();
	}

	inline BufferPtr Runtime::Compile(const char * scriptText, String name, std::initializer_list<String> libraries)
	{
		auto scriptBuffer = CreateBuffer();
		scriptBuffer->Write(scriptText, strlen(scriptText) + 1);
		return Compile(scriptBuffer, name, libraries);
	}

	inline ScriptPtr Runtime::CreateScript(BufferPtr bytecode, Any userContext)
	{
		return std::allocate_shared<Script>(Allocator<Script>(), shared_from_this(), std::static_pointer_cast<Buffer>(bytecode), userContext);
	}

	inline ScriptPtr Runtime::CreateScript(const char * scriptText, Any userContext, String name, std::initializer_list<String> libraries)
	{
		// Compile script text to bytecode
		auto bytecode = Compile(scriptText, name, libraries);
		if (!bytecode)
			return nullptr;

		// Create and return the script
		return CreateScript(bytecode, userContext);
	}

	inline ScriptPtr Runtime::ExecuteScript(const char * scriptcode, Any userContext, String name, std::initializer_list<String> libraries)
	{
		// Compile the text to bytecode
		auto bytecode = Compile(scriptcode, name, libraries);
		if (!bytecode)
			return nullptr;

		// Create a runtime script with the given bytecode
		auto script = CreateScript(bytecode, userContext);
		if (!script)
			return nullptr;

		// Execute script and update runtime until script is finished
		script->Execute();

		// Return script
		return script;
	}

	inline FunctionDefinitionPtr Runtime::FindFunction(RuntimeID id) const
	{
		std::lock_guard<std::mutex> lock(m_functionMutex[id % NumMutexes]);
		auto itr = m_functionMap.find(id);
		if (itr == m_functionMap.end())
			return nullptr;
		return itr->second;
	}

	inline PerformanceStats Runtime::GetScriptPerformanceStats(bool resetStats)
	{
		std::lock_guard<std::mutex> lock(m_perfMutex);
		auto end = std::chrono::high_resolution_clock::now();
		m_perfStats.perfTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_perfStartTime).count();
		PerformanceStats s = m_perfStats;
		if (resetStats)
		{
			m_perfStats = PerformanceStats();
			m_perfStartTime = end;
		}
		return s;
	}

	inline Variant Runtime::GetProperty(RuntimeID id) const
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex[id % NumMutexes]);
		auto itr = m_propertyMap.find(id);
		if (itr == m_propertyMap.end())
			return Variant();
		return itr->second;
	}

	inline LibraryPtr Runtime::GetLibrary(const String & name)
	{
		std::lock_guard<std::mutex> lock(m_libraryMutex);
		auto itr = m_libraryMap.find(name);
		if (itr == m_libraryMap.end())
		{
			auto library = std::allocate_shared<Library>(Allocator<Library>(), shared_from_this(), name);
			m_libraryMap.insert(std::make_pair(name, library));
			return library;
		}
		return itr->second;
	}

	inline bool Runtime::LibraryExists(const String & name) const
	{
		std::lock_guard<std::mutex> lock(m_libraryMutex);
		return m_libraryMap.find(name) != m_libraryMap.end();
	}

	inline void Runtime::LogBytecode(const Parser & parser) const
	{
		LogWriteLine(LogLevel::Info, "\nBytecode:\n====================");
		const size_t columnWidth = 16;
		auto buffer = parser.GetBytecode();
		BinaryReader reader(buffer);
		BytecodeHeader header;
		reader.Read(&header, sizeof(header));
		String scriptName;
		reader.Read(&scriptName);
		int instructionCount = 0;
		while (reader.Tell() < header.dataSize + sizeof(header))
		{
			// Read opcode instruction
			uint8_t opByte;
			reader.Read(&opByte);
			if (opByte >= static_cast<uint32_t>(Opcode::NumOpcodes))
			{
				LogWriteLine(LogLevel::Error, "LogBytecode(): Invalid operation in bytecode");
				return;
			}
			Opcode opcode = static_cast<Opcode>(opByte);

			++instructionCount;

			const char * opcodeName = GetOpcodeText(opcode);
			size_t opcodeNameLength = strlen(opcodeName);
			LogWrite(LogLevel::Info, opcodeName);

			// Advance to column offset
			assert(opcodeNameLength < columnWidth);
			for (size_t i = 0; i < (columnWidth - opcodeNameLength); ++i)
				LogWrite(LogLevel::Info, " ");

			// Read and log opcode arguments
			switch (opcode)
			{
			case Opcode::CallFunc:
			case Opcode::EraseItr:
			case Opcode::PushProp:
			case Opcode::PushVar:
			case Opcode::SetProp:
			case Opcode::SetVar:
			{
				RuntimeID id;
				reader.Read(&id);
				LogWrite(LogLevel::Info, "%s", parser.GetNameFromID(id).c_str());
			}
			break;
			case Opcode::ErasePropKeyVal:
			case Opcode::EraseVarKeyVal:
			case Opcode::SetPropKeyVal:
			case Opcode::SetVarKeyVal:
			{
				uint32_t subscripts;
				reader.Read(&subscripts);
				RuntimeID id;
				reader.Read(&id);
				LogWrite(LogLevel::Info, "%i %s, %s", subscripts, subscripts == 1 ? "subscript" : "subscripts", parser.GetNameFromID(id).c_str());
			}
			break;
			case Opcode::Cast:
			{
				uint8_t b;
				reader.Read(&b);
				auto type = ByteToValueType(b);
				LogWrite(LogLevel::Info, "%s", GetValueTypeName(type));
			}
			break;
			case Opcode::Function:
			{
				FunctionSignature signature;
				signature.Read(reader);
				LogWrite(LogLevel::Info, "%s", parser.GetNameFromID(signature.GetId()).c_str());
			}
			break;
			case Opcode::Library:
			{
				String name;
				reader.Read(&name);
				LogWrite(LogLevel::Info, "%s", name.c_str());
			}
			break;
			case Opcode::Property:
			{
				PropertyName propertyName;
				propertyName.Read(reader);
			}
			break;
			case Opcode::Jump:
			case Opcode::JumpFalse:
			case Opcode::JumpFalseCheck:
			case Opcode::JumpTrue:
			case Opcode::JumpTrueCheck:
			case Opcode::PopCount:
			case Opcode::PushColl:
			case Opcode::PushList:
			{
				uint32_t count;
				reader.Read(&count);
				LogWrite(LogLevel::Info, "%i", count);
			}
			break;
			case Opcode::PushVal:
			{
				Variant val;
				val.Read(reader);
				LogWrite(LogLevel::Info, "%s", val.GetString().c_str());
			}
			break;
			case Opcode::SetIndex:
			{
				RuntimeID id;
				reader.Read(&id);
				int32_t stackIndex;
				reader.Read(&stackIndex);
				ValueType type;
				reader.Read<ValueType, uint8_t>(&type);
				LogWrite(LogLevel::Info, "%s %i %s", parser.GetNameFromID(id).c_str(), stackIndex, GetValueTypeName(type));
			}
			break;
			default:
			{
			}
			break;
			}
			LogWrite(LogLevel::Info, "\n");
		}
		LogWrite(LogLevel::Info, "\nInstruction Count: %i\n\n", instructionCount);
	}

	inline void Runtime::LogSymbols(const SymbolList & symbolList) const
	{
		LogWriteLine(LogLevel::Info, "\nSymbols:\n====================");
		bool newLine = true;

		// Store offset of first symbol
		auto offset = 0;
		if (!symbolList.empty())
			offset = symbolList.begin()->columnNumber;

		int lineCount = 0;

		// Iterate through the symbol list and write everything to the log
		for (auto symbol = symbolList.begin(); symbol != symbolList.end(); ++symbol)
		{
			// Write indentation at each new line
			if (newLine)
			{
				for (uint32_t i = 1; i < symbol->columnNumber - offset; ++i)
					LogWrite(LogLevel::Info, " ");
				newLine = false;
			}

			// Write to the log based on the symbol type
			switch (symbol->type)
			{
			case SymbolType::None:
				LogWrite(LogLevel::Info, "(None) ");
				break;
			case SymbolType::Invalid:
				LogWrite(LogLevel::Info, "(Invalid) ");
				break;
			case SymbolType::NewLine:
				LogWrite(LogLevel::Info, "\n");
				newLine = true;
				++lineCount;
				break;
			case SymbolType::NameValue:
				// Display names with spaces as surrounded by single quotes to help delineate them
				// from surrounding symbols.
				if (strstr(symbol->text.c_str(), " "))
					LogWrite(LogLevel::Info, "'%s' ", symbol->text.c_str());
				else
					LogWrite(LogLevel::Info, "%s ", symbol->text.c_str());
				break;
			case SymbolType::StringValue:
				LogWrite(LogLevel::Info, "\"%s\" ", symbol->text.c_str());
				break;
			case SymbolType::NumberValue:
				LogWrite(LogLevel::Info, "%f ", symbol->numVal);
				break;
			case SymbolType::IntegerValue:
				LogWrite(LogLevel::Info, "%" PRId64 " ", static_cast<int64_t>(symbol->intVal));
				break;
			case SymbolType::BooleanValue:
				LogWrite(LogLevel::Info, "%s ", symbol->boolVal ? "true" : "false");
				break;
			default:
				LogWrite(LogLevel::Info, "%s ", GetSymbolTypeText(symbol->type));
				break;
			};
		}
		LogWrite(LogLevel::Info, "\nLine Count: %i\n\n", lineCount);
	}

	inline bool Runtime::PropertyExists(RuntimeID id) const
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex[id % NumMutexes]);
		return m_propertyMap.find(id) != m_propertyMap.end();
	}

	inline void Runtime::RegisterFunction(const FunctionSignature & signature, BufferPtr bytecode, size_t offset)
	{
		std::mutex & mutex = m_functionMutex[signature.GetId() % NumMutexes];
		std::lock_guard<std::mutex> lock(mutex);
		auto functionDefPtr = std::allocate_shared<FunctionDefinition>(Allocator<FunctionDefinition>(), signature, bytecode, offset);
		m_functionMap.insert(std::make_pair(signature.GetId(), functionDefPtr));
	}

	inline void Runtime::RegisterFunction(const FunctionSignature & signature, FunctionCallback function)
	{
		std::mutex & mutex = m_functionMutex[signature.GetId() % NumMutexes];
		std::lock_guard<std::mutex> lock(mutex);
		auto functionDefPtr = std::allocate_shared<FunctionDefinition>(Allocator<FunctionDefinition>(), signature, function);
		m_functionMap.insert(std::make_pair(signature.GetId(), functionDefPtr));
	}

	inline void Runtime::SetProperty(RuntimeID id, std::function<void(Variant &)> fn)
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex[id % NumMutexes]);
		auto& prop = m_propertyMap[id];
		fn(prop);
	}

	inline void Runtime::SetProperty(RuntimeID id, const Variant & value)
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex[id % NumMutexes]);
		m_propertyMap[id] = value;
	}

	inline BufferPtr Runtime::StripDebugInfo(BufferPtr bytecode) const
	{
		// Validate input
		assert(bytecode);
		if (!bytecode)
			return nullptr;
		if (bytecode->Size() < sizeof(BytecodeHeader))
			return nullptr;

		// Read the bytecode header
		BytecodeHeader header;
		BinaryReader reader(bytecode);
		reader.Read(&header, sizeof(header));

		// Validate bytecode header
		if (header.signature != BytecodeSignature || header.version != BytecodeVersion || header.dataSize == 0)
			return nullptr;

		// If the total size matches size indicated by bytecode header, there is no
		// debug info.  Just return the original buffer.
		if (header.dataSize + sizeof(header) <= bytecode->Size())
			return bytecode;

		// Copy just the bytecode without debug info to a new buffer and return it
		auto newBytecode = CreateBuffer();
		newBytecode->Reserve(bytecode->Size());
		newBytecode->Write(bytecode->Ptr(), bytecode->Size());
		return bytecode;
	}

} // namespace Jinx::Impl

namespace Jinx
{

	inline RuntimePtr CreateRuntime()
	{
		auto runtime = std::allocate_shared<Impl::Runtime>(Allocator<Impl::Runtime>());
		Impl::RegisterLibCore(runtime);
		return runtime;
	}

} // namespace Jinx



// end --- JxRuntime.cpp --- 



// begin --- JxScript.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

namespace Jinx::Impl
{

	inline Script::Script(RuntimeIPtr runtime, BufferPtr bytecode, Any userContext) :
		m_runtime(runtime),
		m_userContext(userContext),
		m_bytecodeStart(0),
		m_finished(false),
		m_error(false)
	{
		m_execution.reserve(6);
		m_execution.push_back(ExecutionFrame(bytecode, "root"));
		m_stack.reserve(32);

		// Assume default unnamed library unless explicitly overridden
		m_library = m_runtime->GetLibraryInternal("");

		// Read bytecode header
		BytecodeHeader header;
		auto & reader = m_execution.back().reader;
		reader.Read(&header, sizeof(header));
		if (header.signature != BytecodeSignature || header.version != BytecodeVersion || header.dataSize == 0)
		{
			Error("Invalid bytecode");
		}

		// Read the script name
		reader.Read(&m_name);
		if (m_name.empty())
			m_name = "(unnamed)";

		// Mark the starting position of the executable bytecode
		m_bytecodeStart = reader.Tell();
	}

	inline Script::~Script()
	{
		// Clear potential circular references by explicitly destroying collection values
		for (auto & s : m_stack)
		{
			if (s.IsCollection())
			{
				auto c = s.GetCollection();
				for (auto & e : *c)
				{
					e.second.SetNull();
				}
			}
		}
	}

	inline void Script::Error(const char * message)
	{
		// Set flags to indicate a fatal runtime error
		m_error = true;
		m_finished = true;

		// Try to determine line number in current script execution context
		uint32_t lineNumber = 0;
		auto & reader = m_execution.back().reader;
		auto bytecodePos = reader.Tell();
		reader.Seek(0);
		BytecodeHeader bytecodeHeader;
		reader.Read(&bytecodeHeader, sizeof(bytecodeHeader));
		if (reader.Size() > sizeof(bytecodeHeader) + bytecodeHeader.dataSize)
		{
			// Validate debug info
			reader.Seek(sizeof(bytecodeHeader) + bytecodeHeader.dataSize);
			if (reader.Size() < sizeof(bytecodeHeader) + bytecodeHeader.dataSize + sizeof(DebugHeader))
			{
				LogWriteLine(LogLevel::Error, "Potentially corrupt bytecode debug data");
				return;
			}
			DebugHeader debugHeader;
			reader.Read(&debugHeader, sizeof(debugHeader));
			if (debugHeader.signature != DebugSignature ||
				reader.Size() < sizeof(bytecodeHeader) + bytecodeHeader.dataSize + sizeof(debugHeader) + debugHeader.dataSize)
			{
				LogWriteLine(LogLevel::Error, "Potentially corrupt bytecode debug data");
				return;
			}

			// Read bytecode to line number table
			for (uint32_t i = 0; i < debugHeader.lineEntryCount; ++i)
			{
				DebugLineEntry lineEntry;
				reader.Read(&lineEntry, sizeof(lineEntry));
				if (lineEntry.opcodePosition > bytecodePos)
					break;
				lineNumber = lineEntry.lineNumber;
			}
		}

		// If we have a line number, use it.  Otherwise, just report what we know.
		if (lineNumber)
			LogWriteLine(LogLevel::Error, "Runtime error in script '%s' at line %i: %s", m_name.c_str(), lineNumber, message);
		else
			LogWriteLine(LogLevel::Error, "Runtime error in script '%s': %s", m_name.c_str(), message);
	}

	inline bool Script::Execute()
	{
		// Don't continue executing if we've encountered an error
		if (m_error)
			return false;

		// Make sure we have bytecode
		if (!m_execution.back().bytecode)
		{
			Error("No bytecode to execute");
			return false;
		}

		// Auto reset if finished
		if (m_finished)
		{
			assert(m_execution.size() == 1);
			m_finished = false;
			m_execution.back().reader.Seek(m_bytecodeStart);
		}

		// Mark script execution start time
		auto begin = std::chrono::high_resolution_clock::now();

		uint32_t tickInstCount = 0;
		uint32_t maxInstCount = MaxInstructions();

		Opcode opcode;
		do
		{
			// Check instruction count before altering the script state
			++tickInstCount;
			if (tickInstCount >= maxInstCount)
			{
				if (ErrorOnMaxInstrunction())
				{
					Error("Exceeded max instruction count");
					return false;
				}
				return true;
			}

			// Read opcode instruction
			uint8_t opByte;
			m_execution.back().reader.Read(&opByte);
			if (opByte >= static_cast<uint32_t>(Opcode::NumOpcodes))
			{
				Error("Invalid operation in bytecode");
				return false;
			}
			opcode = static_cast<Opcode>(opByte);

			// Execute the current opcode
			switch (opcode)
			{
			case Opcode::Add:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				auto result = op1 + op2;
				if (result.IsNull())
				{
					Error("Invalid variable for addition");
					return false;
				}
				Push(result);
			}
			break;
			case Opcode::And:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				auto result = op1.GetBoolean() && op2.GetBoolean();
				Push(result);
			}
			break;
			case Opcode::CallFunc:
			{
				RuntimeID id;
				m_execution.back().reader.Read(&id, sizeof(id));
				FunctionDefinitionPtr functionDef = m_runtime->FindFunction(id);
				if (!functionDef)
				{
					Error("Could not find function definition");
					return false;
				}
				// Check to see if this is a bytecode function
				if (functionDef->GetBytecode())
				{
					m_execution.push_back(ExecutionFrame(functionDef));
					m_execution.back().reader.Seek(functionDef->GetOffset());
					assert(m_stack.size() >= functionDef->GetParameterCount());
					m_execution.back().stackTop = m_stack.size() - functionDef->GetParameterCount();
				}
				// Otherwise, call a native function callback
				else if (functionDef->GetCallback())
				{
					Parameters params;
					size_t numParams = functionDef->GetParameterCount();
					for (size_t i = 0; i < numParams; ++i)
					{
						size_t index = m_stack.size() - (numParams - i);
						auto param = m_stack[index];
						params.push_back(param);
					}
					for (size_t i = 0; i < numParams; ++i)
						m_stack.pop_back();
					Variant retVal = functionDef->GetCallback()(shared_from_this(), params);
					Push(retVal);
				}
				else
				{
					Error("Error in function definition");
					return false;
				}
			}
			break;
			case Opcode::Cast:
			{
				uint8_t b;
				m_execution.back().reader.Read(&b);
				auto type = ByteToValueType(b);
				assert(!m_stack.empty());
				m_stack.back().ConvertTo(type);
			}
			break;
			case Opcode::Decrement:
			{
				auto op1 = Pop();
				auto op2 = Pop();
				if (!op1.IsNumericType())
				{
					Error("Can't increment non-numeric type");
					return false;
				}
				if (!op2.IsNumericType())
				{
					Error("Can't increment value by a non-numeric type");
					return false;
				}
				op2 -= op1;
				Push(op2);
			}
			break;
			case Opcode::Divide:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				if (op2.GetNumber() == 0.0)
				{
					Error("Divide by zero");
					return false;
				}
				auto result = op1 / op2;
				if (result.IsNull())
				{
					Error("Invalid variable for division");
					return false;
				}
				Push(result);
			}
			break;
			case Opcode::Equals:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				auto result = op1 == op2;
				Push(result);
			}
			break;
			case Opcode::EraseItr:
			{
				RuntimeID id;
				m_execution.back().reader.Read(&id);
				auto var = GetVariable(id);
				if (!var.IsCollectionItr())
				{
					Error("Expected collection iterator for this form of erase");
					return false;
				}
				auto itr = var.GetCollectionItr().first;
				auto coll = var.GetCollectionItr().second;
				if (itr != coll->end())
					itr = coll->erase(itr);
				SetVariable(id, std::make_pair(itr, coll));
			}
			break;
			case Opcode::ErasePropKeyVal:
			{
				uint32_t subscripts;
				m_execution.back().reader.Read(&subscripts);
				RuntimeID propId;
				m_execution.back().reader.Read(&propId);

				m_runtime->SetProperty(propId, [this, subscripts](Variant& coll)
				{
					if (!coll.IsCollection())
					{
						this->Error("Expected collection when accessing by key");
						return;
					}
					auto collection = coll.GetCollection();

					// Find the appropriate collection-key pair
					auto pair = WalkSubscripts(subscripts, collection);
					if (pair.first == nullptr)
						return;
					collection = pair.first;
					Variant key = pair.second;

					// Erase the value based on the key if it exists
					auto itr = collection->find(key);
					if (itr != collection->end())
						collection->erase(itr);
				});
			}
			break;
			case Opcode::EraseVarKeyVal:
			{
				uint32_t subscripts;
				m_execution.back().reader.Read(&subscripts);
				RuntimeID id;
				m_execution.back().reader.Read(&id);
				Variant coll = GetVariable(id);
				if (!coll.IsCollection())
				{
					Error("Expected collection when accessing by key");
					return false;
				}
				auto collection = coll.GetCollection();

				// Find the appropriate collection-key pair
				auto pair = WalkSubscripts(subscripts, collection);
				if (pair.first == nullptr)
					return false;
				collection = pair.first;
				Variant key = pair.second;

				// Erase the value based on the key if it exists
				auto itr = collection->find(key);
				if (itr != collection->end())
					collection->erase(itr);
			}
			break;
			case Opcode::Exit:
			{
				while (m_execution.size() > 1)
					m_execution.pop_back();
				m_finished = true;
			}
			break;
			case Opcode::Function:
			{
				FunctionSignature signature;
				signature.Read(m_execution.back().reader);
				if (signature.GetVisibility() != VisibilityType::Local)
					m_library->RegisterFunctionSignature(signature);
				// Note: we add 5 bytes to the current position to skip over the jump command and offset value
				m_runtime->RegisterFunction(signature, m_execution.back().bytecode, m_execution.back().reader.Tell() + 5);
			}
			break;
			case Opcode::Greater:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				if (!ValidateValueComparison(op1, op2))
				{
					Error("Incompatible types in operator >");
					return false;
				}
				auto result = op1 > op2;
				Push(result);
			}
			break;
			case Opcode::GreaterEq:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				if (!ValidateValueComparison(op1, op2))
				{
					Error("Incompatible types in operator >=");
					return false;
				}
				auto result = op1 >= op2;
				Push(result);
			}
			break;
			case Opcode::Increment:
			{
				auto op1 = Pop();
				auto op2 = Pop();
				if (!op1.IsNumericType())
				{
					Error("Can't increment non-numeric type");
					return false;
				}
				if (!op2.IsNumericType())
				{
					Error("Can't increment value by a non-numeric type");
					return false;
				}
				op2 += op1;
				Push(op2);
			}
			break;
			case Opcode::Jump:
			{
				uint32_t jumpIndex;
				m_execution.back().reader.Read(&jumpIndex);
				m_execution.back().reader.Seek(jumpIndex);
			}
			break;
			case Opcode::JumpFalse:
			{
				uint32_t jumpIndex;
				m_execution.back().reader.Read(&jumpIndex);
				auto op1 = Pop();
				if (op1.GetBoolean() == false)
				{
					m_execution.back().reader.Seek(jumpIndex);
				}
			}
			break;
			case Opcode::JumpFalseCheck:
			{
				uint32_t jumpIndex;
				m_execution.back().reader.Read(&jumpIndex);
				if (m_stack.empty())
				{
					Error("Stack underflow");
					return false;
				}
				const auto & op1 = m_stack.back();
				if (op1.GetBoolean() == false)
				{
					m_execution.back().reader.Seek(jumpIndex);
				}
			}
			break;
			case Opcode::JumpTrue:
			{
				uint32_t jumpIndex;
				m_execution.back().reader.Read(&jumpIndex);
				auto op1 = Pop();
				if (op1.GetBoolean() == true)
				{
					m_execution.back().reader.Seek(jumpIndex);
				}
			}
			break;
			case Opcode::JumpTrueCheck:
			{
				uint32_t jumpIndex;
				m_execution.back().reader.Read(&jumpIndex);
				if (m_stack.empty())
				{
					Error("Stack underflow");
					return false;
				}
				const auto & op1 = m_stack.back();
				if (op1.GetBoolean() == true)
				{
					m_execution.back().reader.Seek(jumpIndex);
				}
			}
			break;
			case Opcode::Less:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				if (!ValidateValueComparison(op1, op2))
				{
					Error("Incompatible types in operator <");
					return false;
				}
				auto result = op1 < op2;
				Push(result);
			}
			break;
			case Opcode::LessEq:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				if (!ValidateValueComparison(op1, op2))
				{
					Error("Incompatible types in operator <=");
					return false;
				}
				auto result = op1 <= op2;
				Push(result);
			}
			break;
			case Opcode::Library:
			{
				String libraryName;
				m_execution.back().reader.Read(&libraryName);
				m_library = m_runtime->GetLibraryInternal(libraryName);
			}
			break;
			case Opcode::LoopCount:
			{
				assert(m_stack.size() >= 3);
				auto top = m_stack.size() - 1;
				auto loopVal = m_stack[top - 2];
				auto loopDest = m_stack[top - 1];
				auto incBy = m_stack[top];
				if (incBy.IsNull())
				{
					if (loopVal > loopDest)
						incBy = -1;
					else
						incBy = 1;
				}
				loopVal += incBy;
				m_stack[top - 2] = loopVal;
				auto incVal = incBy.GetNumber();
				if (incVal > 0)
					Push(loopVal <= loopDest);
				else if (incVal < 0)
					Push(loopVal >= loopDest);
				else
				{
					Error("Incremented loop counter by zero");
					return false;
				}
			}
			break;
			case Opcode::LoopOver:
			{
				assert(m_stack.size() >= 2);
				auto top = m_stack.size() - 1;
				auto itr = m_stack[top];
				assert(itr.IsCollectionItr());
				auto coll = m_stack[top - 1];
				assert(coll.IsCollection() && coll.GetCollection());
				bool finished = itr.GetCollectionItr().first == coll.GetCollection()->end();
				if (!finished)
				{
					++itr;
					finished = itr.GetCollectionItr().first == coll.GetCollection()->end();
				}
				m_stack[top] = itr;
				Push(finished);
			}
			break;
			case Opcode::Mod:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				if (op2.GetNumber() == 0.0)
				{
					Error("Mod by zero");
					return false;
				}
				auto result = op1 % op2;
				if (result.IsNull())
				{
					Error("Invalid variable for mod");
					return false;
				}
				Push(result);
			}
			break;
			case Opcode::Multiply:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				auto result = op1 * op2;
				if (result.IsNull())
				{
					Error("Invalid variable for multiply");
					return false;
				}
				Push(result);
			}
			break;
			case Opcode::Negate:
			{
				auto op1 = Pop();
				if (!op1.IsNumericType())
				{
					Error("Only numeric types can be negated");
					return false;
				}
				auto result = op1 * -1;
				Push(result);
			}
			break;
			case Opcode::Not:
			{
				auto op1 = Pop();
				auto result = !op1.GetBoolean();
				Push(result);
			}
			break;
			case Opcode::NotEquals:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				auto result = op1 != op2;
				Push(result);
			}
			break;
			case Opcode::Or:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				auto result = op1.GetBoolean() || op2.GetBoolean();
				Push(result);
			}
			break;
			case Opcode::Pop:
			{
				Pop();
			}
			break;
			case Opcode::PopCount:
			{
				uint32_t count;
				m_execution.back().reader.Read(&count);
				for (uint32_t i = 0; i < count; ++i)
					Pop();
			}
			break;
			case Opcode::Property:
			{
				PropertyName propertyName;
				propertyName.Read(m_execution.back().reader);
				m_library->RegisterPropertyName(propertyName, false);
				m_runtime->SetProperty(propertyName.GetId(), propertyName.GetDefaultValue());
			}
			break;
			case Opcode::PushColl:
			{
				uint32_t count;
				m_execution.back().reader.Read(&count);
				Variant collection(CreateCollection());
				for (uint32_t i = 0; i < count; ++i)
				{
					size_t offset = (count - i) * 2;
					if (offset > m_stack.size())
					{
						Error("Collection data error");
						return false;
					}
					size_t index = m_stack.size() - offset;
					if ((index + 1) > m_stack.size())
					{
						Error("Error in collection data");
						return false;
					}
					Variant key = m_stack[index];
					if (!key.IsKeyType())
					{
						Error("Invalid key type");
						return false;
					}
					Variant value = m_stack[index + 1];
					collection.GetCollection()->insert(std::make_pair(key, value));
				}
				for (uint32_t i = 0; i < count * 2; ++i)
					m_stack.pop_back();
				Push(collection);
			}
			break;
			case Opcode::PushItr:
			{
				assert(m_stack.size() >= 1);
				auto top = m_stack.size() - 1;
				auto coll = m_stack[top];
				if (!coll.IsCollection())
				{
					Error("Expected collection type");
					return false;
				}
				Variant itr = std::make_pair(coll.GetCollection()->begin(), coll.GetCollection());
				Push(itr);
			}
			break;
			case Opcode::PushKeyVal:
			{
				auto key = Pop();
				if (!key.IsKeyType())
				{
					Error("Invalid key type");
					return false;
				}
				auto coll = Pop();
				if (!coll.IsCollection())
				{
					Error("Expected collection type");
					return false;
				}
				auto itr = coll.GetCollection()->find(key);
				if (itr == coll.GetCollection()->end())
				{
					Error("Specified key does not exist in collection");
					return false;
				}
				else
				{
					Push(itr->second);
				}
			}
			break;
			case Opcode::PushList:
			{
				uint32_t count;
				m_execution.back().reader.Read(&count);
				Variant collection(CreateCollection());
				if (count > m_stack.size())
				{
					Error("Push list error");
					return false;
				}
				for (uint32_t i = 0; i < count; ++i)
				{
					size_t index = m_stack.size() - (count - i);
					Variant value = m_stack[index];
					Variant key(static_cast<int64_t>(i) + 1);
					collection.GetCollection()->insert(std::make_pair(key, value));
				}
				for (uint32_t i = 0; i < count; ++i)
					m_stack.pop_back();
				Push(collection);
			}
			break;
			case Opcode::PushProp:
			{
				uint64_t id;
				m_execution.back().reader.Read(&id);
				auto val = m_runtime->GetProperty(id);
				Push(val);
			}
			break;
			case Opcode::PushTop:
			{
				assert(m_stack.size() >= 1);
				auto op = m_stack[m_stack.size() - 1];
				Push(op);
			}
			break;
			case Opcode::PushVar:
			{
				RuntimeID id;
				m_execution.back().reader.Read(&id);
				auto var = GetVariable(id);
				Push(var);
			}
			break;
			case Opcode::PushVal:
			{
				Variant val;
				val.Read(m_execution.back().reader);
				Push(val);
			}
			break;
			case Opcode::Return:
			{
				auto val = Pop();
				assert(!m_execution.empty());
				size_t targetSize = m_execution.back().stackTop;
				bool exit = m_execution.back().waitOnReturn;
				m_execution.pop_back();
				assert(!m_execution.empty());
				while (m_stack.size() > targetSize)
					m_stack.pop_back();
				Push(val);
				if (exit)
					opcode = Opcode::Wait;
			}
			break;
			case Opcode::ScopeBegin:
			{
				m_execution.back().scopeStack.push_back(m_stack.size());
			}
			break;
			case Opcode::ScopeEnd:
			{
				auto stackTop = m_execution.back().scopeStack.back();
				m_execution.back().scopeStack.pop_back();
				while (m_stack.size() > stackTop)
					m_stack.pop_back();
				auto & ids = m_execution.back().ids;
				for (auto itr = ids.begin(); itr != ids.end();)
				{
					if (itr->second >= stackTop)
						itr = ids.erase(itr);
					else
						++itr;
				}
			}
			break;
			case Opcode::SetIndex:
			{
				assert(!m_stack.empty());
				RuntimeID id;
				m_execution.back().reader.Read(&id);
				int32_t stackIndex;
				m_execution.back().reader.Read(&stackIndex);
				ValueType type;
				m_execution.back().reader.Read<ValueType, uint8_t>(&type);
				size_t index = m_stack.size() + stackIndex;
				if (type != ValueType::Any)
				{
					if (!m_stack[index].ConvertTo(type))
					{
						Error("Invalid function parameter cast");
						return false;
					}
				}
				SetVariableAtIndex(id, index);
			}
			break;
			case Opcode::SetProp:
			{
				RuntimeID id;
				m_execution.back().reader.Read(&id);
				Variant val = Pop();
				m_runtime->SetProperty(id, val);
			}
			break;
			case Opcode::SetPropKeyVal:
			{
				uint32_t subscripts;
				m_execution.back().reader.Read(&subscripts);
				RuntimeID id;
				m_execution.back().reader.Read(&id);
				m_runtime->SetProperty(id, [this, subscripts](Variant& coll)
				{
					if (!coll.IsCollection())
					{
						this->Error("Expected collection when accessing by key");
						return;
					}
					auto collection = coll.GetCollection();
					Variant val = Pop();
					auto pair = WalkSubscripts(subscripts, collection);
					if (pair.first == nullptr)
						return;
					collection = pair.first;
					Variant key = pair.second;
					(*collection)[key] = val;
				});
			}
			break;
			case Opcode::SetVar:
			{
				RuntimeID id;
				m_execution.back().reader.Read(&id);
				Variant val = Pop();
				SetVariable(id, val);
			}
			break;
			case Opcode::SetVarKeyVal:
			{
				uint32_t subscripts;
				m_execution.back().reader.Read(&subscripts);
				RuntimeID id;
				m_execution.back().reader.Read(&id);
				Variant coll = GetVariable(id);
				if (!coll.IsCollection())
				{
					Error("Expected collection when accessing by key");
					return false;
				}
				auto collection = coll.GetCollection();
				Variant val = Pop();
				auto pair = WalkSubscripts(subscripts, collection);
				if (pair.first == nullptr )
					return false;
				collection = pair.first;
				Variant key = pair.second;
				(*collection)[key] = val;
			}
			break;
			case Opcode::Subtract:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				auto result = op1 - op2;
				if (result.IsNull())
				{
					Error("Invalid variable for subraction");
					return false;
				}
				Push(result);
			}
			break;
			case Opcode::Type:
			{
				auto val = Pop();
				Push(val.GetType());
			}
			break;
			case Opcode::Wait:
			{
			}
			break;
			default:
			{
				Error("Unknown opcode");
				return false;
			}
			break;
			}
		} 
		while (opcode != Opcode::Exit && opcode != Opcode::Wait);

		// Track accumulated script execution time
		auto end = std::chrono::high_resolution_clock::now();
		uint64_t executionTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
		m_runtime->AddPerformanceParams(m_finished, executionTimeNs, tickInstCount);

		return true;
	}

	inline RuntimeID Script::FindFunction(LibraryPtr library, const String & name)
	{
		if (library == nullptr)
			library = m_library;
		auto libraryInt = std::static_pointer_cast<Library>(library);
		return libraryInt->FindFunctionSignature(Visibility::Public, name).GetId();
	}

	inline Variant Script::CallFunction(RuntimeID id, Parameters params)
	{
		for (const auto & param : params)
			Push(param);
		return CallFunction(id);
	}

	inline Variant Script::CallFunction(RuntimeID id)
	{
		FunctionDefinitionPtr functionDef = m_runtime->FindFunction(id);
		if (!functionDef)
		{
			Error("Could not find function definition");
			return false;
		}
		// Check to see if this is a bytecode function
		if (functionDef->GetBytecode())
		{
			m_execution.push_back(ExecutionFrame(functionDef));
			m_execution.back().waitOnReturn = true;
			m_execution.back().reader.Seek(functionDef->GetOffset());
			m_execution.back().stackTop = m_stack.size() == 0 ? 0 : m_stack.size() - 1;
			bool finished = m_finished;
			m_finished = false;
			if (!Execute())
				return nullptr;
			m_finished = finished;
			return Pop();
		}
		// Otherwise, call a native function callback
		else if (functionDef->GetCallback())
		{
			Parameters params;
			size_t numParams = functionDef->GetParameterCount();
			for (size_t i = 0; i < numParams; ++i)
			{
				size_t index = m_stack.size() - (numParams - i);
				auto param = m_stack[index];
				params.push_back(param);
			}
			for (size_t i = 0; i < numParams; ++i)
				m_stack.pop_back();
			return functionDef->GetCallback()(shared_from_this(), params);
		}
		else
		{
			Error("Error in function definition");
		}
		return nullptr;
	}

	inline std::vector<String, Allocator<String>> Script::GetCallStack() const
	{
		std::vector<String, Allocator<String>> strings;
		for (const auto & frame : m_execution)
			strings.push_back(frame.name);
		return strings;
	}

	inline Variant Script::GetVariable(const String & name) const
	{
		const auto & foldedName = FoldCase(name);
		RuntimeID id = GetVariableId(foldedName.c_str(), foldedName.size(), 1);
		return GetVariable(id);
	}

	inline Variant Script::GetVariable(RuntimeID id) const
	{
		auto & names = m_execution.back().ids;
		auto itr = names.find(id);
		if (itr != names.end())
		{
			auto index = itr->second;
			if (index >= m_stack.size())
			{
				LogWriteLine(LogLevel::Error, "Attempted to access stack at invalid index");
				return Variant();
			}
			return m_stack[itr->second];
		}
		return Variant();
	}

	inline bool Script::IsFinished() const
	{
		return m_finished || m_error;
	}

	inline Variant Script::Pop()
	{
		if (m_stack.empty())
		{
			Error("Stack underflow");
			return Variant();
		}
		auto var = m_stack.back();
		m_stack.pop_back();
		return var;
	}

	inline void Script::Push(const Variant & value)
	{
		m_stack.push_back(value);
	}

	inline void Script::SetVariable(const String & name, const Variant & value)
	{
		const auto & foldedName = FoldCase(name);
		RuntimeID id = GetVariableId(foldedName.c_str(), foldedName.size(), 1);
		SetVariable(id, value);
	}

	inline void Script::SetVariable(RuntimeID id, const Variant & value)
	{
		// Search the current frame for the variable
		auto & names = m_execution.back().ids;
		auto itr = names.find(id);
		if (itr != names.end())
		{
			auto index = itr->second;
			if (index >= m_stack.size())
			{
				itr->second = m_stack.size();
				m_stack.push_back(value);
				return;
			}
			else
			{
				m_stack[itr->second] = value;
				return;
			}
		}

		// If we don't find the name, create a new variable on the top of the stack
		names.insert(std::make_pair(id, m_stack.size()));
		m_stack.push_back(value);
	}

	inline void Script::SetVariableAtIndex(RuntimeID id, size_t index)
	{
		assert(index < m_stack.size());
		m_execution.back().ids.insert(std::make_pair(id, index));
	}

	inline std::pair<CollectionPtr, Variant> Script::WalkSubscripts(uint32_t subscripts, CollectionPtr collection)
	{
		// Walk up through subsequent subscript operators, then pops the keys off the stack and 
		// returns the final collection and key pair,
		Variant key;

		// Loop through the number of subscript operations used
		for (uint32_t i = 0; i < subscripts; ++i)
		{
			// Grab the appropriate key in the stack for this subscript 
			size_t index = m_stack.size() - (subscripts - i);
			key = m_stack[index];
			if (!key.IsKeyType())
			{
				Error("Invalid key type");
				return {};
			}

			// We only need to retrieve a new collection and key set if
			// this isn't the last operation.
			if (i < (subscripts - 1))
			{
				auto itr = collection->find(key);
				if (itr == collection->end())
				{
					Variant newColl = CreateCollection();
					collection->insert(std::make_pair(key, newColl));
					collection = newColl.GetCollection();
				}
				else if (itr->second.IsCollection())
				{
					collection = itr->second.GetCollection();
				}
				else
				{
					Error("Expected collection when accessing by key");
					return {};
				}
			}
		}

		// Pop keys off the stack
		for (uint32_t i = 0; i < subscripts; ++i)
			Pop();

		// Return the final collection and key pair
		return std::make_pair(collection, key);
	}


} // namespace Jinx::Impl



// end --- JxScript.cpp --- 



// begin --- JxSerialize.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

namespace Jinx
{

	inline void BinaryReader::Read(String * val)
	{
		uint32_t length;
		m_buffer->Read(&m_pos, &length, sizeof(length));
		if (length < (1024 - 1))
		{
			char buffer[1024];
			m_buffer->Read(&m_pos, buffer, length + 1);
			*val = buffer;
		}
		else
		{
			char * buffer = (char *)JinxAlloc(length + 2);
			m_buffer->Read(&m_pos, buffer, length + 1);
			*val = buffer;
			JinxFree(buffer);
		}
	}

	inline void BinaryReader::Read(BufferPtr & val)
	{
		uint32_t size;
		m_buffer->Read(&m_pos, &size, sizeof(size));
		val->Reserve(size);
		m_buffer->Read(&m_pos, val, size);
	}


	inline void BinaryWriter::Write(const String & val)
	{
		uint32_t size = (uint32_t)val.size();
		m_buffer->Write(&m_pos, &size, sizeof(size));
		m_buffer->Write(&m_pos, val.c_str(), size + 1);
	}

	inline void BinaryWriter::Write(const BufferPtr & val)
	{
		uint32_t size = (uint32_t)val->Size();
		m_buffer->Write(&m_pos, &size, sizeof(uint32_t));
		m_buffer->Write(&m_pos, val->Ptr(), val->Size());
	}

	inline void BinaryWriter::Write(BinaryReader & reader, size_t bytes)
	{
		assert((reader.m_pos + bytes) <= reader.m_buffer->Size());
		m_buffer->Write(&m_pos, reader.m_buffer->Ptr() + reader.m_pos, bytes);
		reader.m_pos += bytes;
	}

} // namespace Jinx

// end --- JxSerialize.cpp --- 



// begin --- JxUnicode.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

namespace Jinx::Impl
{

	struct UnicodeData
	{
		static inline size_t sizeUTF8[256] = {
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
			3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, 4,4,4,4,4,4,4,4,5,5,5,5,6,6,6,6,
		};

		static inline uint32_t fastFold[128] = {
			0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
			0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
			0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
			0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
			0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
			0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x5B,0x5C,0x5D,0x5E,0x5F,
			0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
			0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
		};

	};

	inline void ConvertUtf8ToUtf32(const char * utf8In, size_t inBufferCount, char32_t * utf32CodePoint, size_t * numCharsOut)
	{
		// Validate parameters
		if (!utf8In || inBufferCount == 0 || !utf32CodePoint || !numCharsOut)
		{
			LogWriteLine(LogLevel::Error, "Invalid arguments passed to ConvertUtf8ToUtf32()");
			return;
		}

		// Initialize output
		*utf32CodePoint = 0;
		*numCharsOut = 1;

		// Check for all four conversion cases
		unsigned char * utf8 = (unsigned char *)utf8In;
		if ((utf8[0] & 0x80) == 0)
		{
			*utf32CodePoint = utf8[0];
			*numCharsOut = 1;
		}
		else if ((utf8[0] & 0xE0) == 0xC0)
		{
			if (inBufferCount < 2 || (utf8[1] & 0xC0) != 0x80)
			{
				LogWriteLine(LogLevel::Error, "Invalid character data passed to function ConvertUtf8ToUtf32()");
				return;
			}
			*utf32CodePoint = char32_t(utf8[0] & 0x1F);
			if (*utf32CodePoint < 2)
			{
				LogWriteLine(LogLevel::Error, "Invalid character data passed to function ConvertUtf8ToUtf32()");
				return;
			}
			*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[1] & 0x3F);
			*numCharsOut = 2;
		}
		else if ((utf8In[0] & 0xF0) == 0xE0)
		{
			if (inBufferCount < 3 || (utf8[1] & 0xC0) != 0x80 || (utf8[2] & 0xC0) != 0x80)
			{
				LogWriteLine(LogLevel::Error, "Invalid character data passed to function ConvertUtf8ToUtf32()");
				return;
			}
			*utf32CodePoint = char32_t(utf8[0] & 0x0F);
			*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[1] & 0x3F);
			*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[2] & 0x3F);
			*numCharsOut = 3;
		}
		else if ((utf8In[0] & 0xF8) == 0xF0)
		{
			if (inBufferCount < 4 || (utf8[1] & 0xC0) != 0x80 || (utf8[2] & 0xC0) != 0x80 || (utf8[3] & 0xC0) != 0x80)
			{
				LogWriteLine(LogLevel::Error, "Invalid character data passed to function ConvertUtf8ToUtf32()");
				return;
			}
			*utf32CodePoint = char32_t(utf8[0] & 0x07);
			*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[1] & 0x3F);
			*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[2] & 0x3F);
			*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[3] & 0x3F);
			*numCharsOut = 4;
		}
		else
		{
			// Invalid data
			LogWriteLine(LogLevel::Error, "Invalid character data passed to function ConvertUtf32ToUtf8()");
		}
	}

	inline void ConvertUtf32ToUtf8(char32_t utf32CodePoint, char * utf8Out, size_t outBufferCount, size_t * numCharsOut)
	{
		// Validate parameters
		if (!utf8Out || outBufferCount == 0 || !numCharsOut)
		{
			LogWriteLine(LogLevel::Error, "Invalid arguments passed to ConvertUtf32ToUtf8()");
			return;
		}

		// Initialize output
		*numCharsOut = 1;

		// Check for all four conversion cases
		if (utf32CodePoint < 0x80)
		{
			utf8Out[0] = char(utf32CodePoint & 0x7F);
			*numCharsOut = 1;
		}
		else if (utf32CodePoint < 0x0800)
		{
			if (outBufferCount < 2)
			{
				LogWriteLine(LogLevel::Error, "Invalid arguments passed to ConvertUtf32ToUtf8()");
				return;
			}
			utf8Out[0] = uint8_t(utf32CodePoint >> 6) | 0xC0;
			utf8Out[1] = (uint8_t(utf32CodePoint >> 0) & 0x3F) | 0x80;
			*numCharsOut = 2;
		}
		else if (utf32CodePoint < 0x010000)
		{
			if (outBufferCount < 3)
			{
				LogWriteLine(LogLevel::Error, "Invalid arguments passed to ConvertUtf32ToUtf8()");
				return;
			}
			utf8Out[0] = (uint8_t(utf32CodePoint >> 12) & 0x0F) | 0xE0;
			utf8Out[1] = (uint8_t(utf32CodePoint >> 6) & 0x3F) | 0x80;
			utf8Out[2] = (uint8_t(utf32CodePoint >> 0) & 0x3F) | 0x80;
			*numCharsOut = 3;
		}
		else if (utf32CodePoint < 0x110000)
		{
			if (outBufferCount < 4)
			{
				LogWriteLine(LogLevel::Error, "Invalid arguments passed to ConvertUtf32ToUtf8()");
				return;
			}
			utf8Out[0] = (uint8_t(utf32CodePoint >> 18) & 0x07) | 0xF0;
			utf8Out[1] = (uint8_t(utf32CodePoint >> 12) & 0x3F) | 0x80;
			utf8Out[2] = (uint8_t(utf32CodePoint >> 6) & 0x3F) | 0x80;
			utf8Out[3] = (uint8_t(utf32CodePoint >> 0) & 0x3F) | 0x80;
			*numCharsOut = 4;
		}
		else
		{
			// Invalid data
			LogWriteLine(LogLevel::Error, "Invalid character data passed to function ConvertUtf32ToUtf8()");
		}
	}

	inline void ConvertUtf16ToUtf32(const char16_t * utf16In, uint32_t inBufferCount, char32_t * utf32CodePoint, size_t * numCharsOut)
	{
		// Validate parameters
		if (!utf16In || inBufferCount == 0 || !utf32CodePoint || !numCharsOut)
		{
			LogWriteLine(LogLevel::Error, "Invalid arguments passed to ConvertUtf16ToUtf32()");
			return;
		}

		// Check for single-digit code points - simple conversion
		if (utf16In[0] < 0xD800 || utf16In[0] > 0xDFFF)
		{
			*utf32CodePoint = utf16In[0];
			*numCharsOut = 1;
			return;
		}

		// Make sure we have a second word
		if (inBufferCount < 2)
		{
			LogWriteLine(LogLevel::Error, "Invalid arguments passed to ConvertUtf16ToUtf32()");
			return;
		}

		// Check for invalid range of second word
		if (utf16In[1] < 0xDC00 || utf16In[1] > 0xDFFF)
		{
			LogWriteLine(LogLevel::Error, "Invalid data passed to ConvertUtf16ToUtf32()");
			return;
		}

		// Set code point and indicate two words have been read
		char32_t surrogate1 = ((utf16In[0] & 0x3FF) << 10);
		char32_t surrogate2 = utf16In[1];
		*utf32CodePoint = surrogate1 + (surrogate2 & 0x3ff) + 0x10000;
		*numCharsOut = 2;
	}

	inline void ConvertUtf32ToUtf16(char32_t utf32CodePoint, char16_t * utf16Out, size_t outBufferCount, size_t * numCharsOut)
	{
		// Validate parameters
		if (!utf16Out || outBufferCount == 0 || !numCharsOut)
		{
			LogWriteLine(LogLevel::Error, "Invalid arguments passed to ConvertUtf32ToUtf16()");
			return;
		}

		// Check for 16-bit code point - simple conversion
		if (utf32CodePoint < 0x10000)
		{
			utf16Out[0] = (char16_t)utf32CodePoint;
			*numCharsOut = 1;
		}
		else
		{
			// Check to make sure we have two buffers to read
			if (outBufferCount < 2)
			{
				LogWriteLine(LogLevel::Error, "Invalid arguments passed to ConvertUtf32ToUtf16()");
				return;
			}

			// Calculate utf16 words and indicate two words have been generated
			utf32CodePoint -= 0x10000;
			char32_t surrogate1 = ((utf32CodePoint >> 10) & 0x3FF) + 0xD800;
			char32_t surrogate2 = (utf32CodePoint & 0x3FF) + 0xDC00;
			utf16Out[0] = (char16_t)surrogate1;
			utf16Out[1] = (char16_t)surrogate2;
			*numCharsOut = 2;
		}
	}

	inline size_t GetUtf8CharSize(const char * utf8Str)
	{
		// Validate parameter
		if (!(utf8Str && utf8Str[0] != 0))
		{
			LogWriteLine(LogLevel::Error, "Invalid character value passed to GetUtf8CharSize()");
			return 1;
		}

		// Retrieve UTF-8 character size from lookup table
		unsigned char c = utf8Str[0];
		size_t s = Impl::UnicodeData::sizeUTF8[c];

		// While values of 5 or 6 bytes are technically possible, it is not a valid UTF-8 sequence
		if (s > 4)
		{
			LogWriteLine(LogLevel::Error, "Invalid character size calculated in GetUtf8CharSize()");
			return 4;
		}

		return s;
	}

	inline StringU16 ConvertUtf8ToUtf16(const String & utf8Str)
	{
		const char * cInStr = utf8Str.c_str();
		const char * cInStrEnd = cInStr + utf8Str.size();
		StringU16 outString;
		outString.reserve(utf8Str.size());
		char16_t outBuffer[3];
		char32_t utf32CodePoint;
		size_t numOut;
		while (*cInStr != 0)
		{
			Impl::ConvertUtf8ToUtf32(cInStr, (uint32_t)(cInStrEnd - cInStr), &utf32CodePoint, &numOut);
			cInStr += numOut;
			Impl::ConvertUtf32ToUtf16(utf32CodePoint, outBuffer, countof(outBuffer), &numOut);
			outBuffer[numOut] = 0;
			outString += outBuffer;
		}
		return outString;
	}

	inline String ConvertUtf16ToUtf8(const StringU16 & utf16_string)
	{
		const char16_t * cInStr = utf16_string.c_str();
		const char16_t * cInStrEnd = cInStr + utf16_string.size();
		String outString;
		outString.reserve(utf16_string.size());
		char outBuffer[5];
		char32_t utf32CodePoint = 0;
		size_t numOut = 0;
		while (*cInStr != 0)
		{
			Impl::ConvertUtf16ToUtf32(cInStr, (uint32_t)(cInStrEnd - cInStr), &utf32CodePoint, &numOut);
			cInStr += numOut;
			Impl::ConvertUtf32ToUtf8(utf32CodePoint, outBuffer, countof(outBuffer), &numOut);
			outBuffer[numOut] = 0;
			outString += outBuffer;
		}
		return outString;
	}

	inline WString ConvertUtf8ToWString(const String & utf8Str)
	{
		// Compile-time check to determine size of wchar_t, and perform appropriate UTF-16 or UTF-32 conversion.
		if constexpr (sizeof(wchar_t) == 4)
		{
			const char * cInStr = utf8Str.c_str();
			const char * cInStrEnd = cInStr + utf8Str.size();
			WString outString;
			outString.reserve(utf8Str.size());
			char32_t utf32CodePoint;
			size_t numOut;
			while (*cInStr != 0)
			{
				Impl::ConvertUtf8ToUtf32(cInStr, (uint32_t)(cInStrEnd - cInStr), &utf32CodePoint, &numOut);
				cInStr += numOut;
				outString += static_cast<wchar_t>(utf32CodePoint);
			}
			return outString;
		}
		else
		{
			return reinterpret_cast<const wchar_t *>(ConvertUtf8ToUtf16(utf8Str).c_str());
		}
	}

	inline String ConvertWStringToUtf8(const WString & wStr)
	{
		// Compile-time check to determine size of wchar_t, and perform appropriate UTF-16 or UTF-32 conversion
		if constexpr (sizeof(wchar_t) == 4)
		{
			auto cInStr = reinterpret_cast<const char32_t *>(wStr.c_str());
			String outString;
			outString.reserve(wStr.size());
			char outBuffer[5];
			size_t numOut;
			while (*cInStr != 0)
			{
				Impl::ConvertUtf32ToUtf8(*cInStr, outBuffer, countof(outBuffer), &numOut);
				outBuffer[numOut] = 0;
				++cInStr;
				outString += outBuffer;
			}
			return outString;
		}
		else
		{
			return ConvertUtf16ToUtf8(reinterpret_cast<const char16_t *>(wStr.c_str()));
		}
	}

	inline bool IsCaseFolded(const String & source)
	{
		const char * curr = source.c_str();
		const char * end = source.c_str() + source.size();
		while (curr < end)
		{
			// ASCII characters can do a fast table-based check
			unsigned char c = *curr;
			if (!((c & 0x80) == 0x80))
			{
				if (Impl::UnicodeData::fastFold[c] != c)
					return false;
				++curr;
			}
			// Non-ASCII characters have to perform a folding map lookup
			else
			{
				char32_t codepoint;
				size_t charsOut;
				Impl::ConvertUtf8ToUtf32(curr, end - curr, &codepoint, &charsOut);
				if (FindCaseFoldingData(codepoint, nullptr, nullptr))
					return false;
				curr += charsOut;
			}
		}
		return true;
	}

	inline String FoldCase(const String & source)
	{
		// Check to see if we can simply return the original source
		if (IsCaseFolded(source))
			return source;

		String s;
		s.reserve(source.size());
		const char * curr = source.c_str();
		const char * end = source.c_str() + source.size();
		while (curr < end)
		{
			// Attempt simple (ASCII-only) folding if possible first
			unsigned char c = *curr;
			if (!((c & 0x80) == 0x80))
			{
				s.push_back(static_cast<unsigned char>(Impl::UnicodeData::fastFold[c]));
				++curr;
			}
			// Non-ASCII codepoints require lookups via the global folding map
			else
			{
				char32_t codepoint;
				size_t charsOut;
				Impl::ConvertUtf8ToUtf32(curr, end - curr, &codepoint, &charsOut);

				char32_t cp1;
				char32_t cp2;
				if (FindCaseFoldingData(codepoint, &cp1, &cp2))
				{
					char buffer[5] = { 0, 0, 0, 0, 0 };
					Impl::ConvertUtf32ToUtf8(cp1, buffer, countof(buffer), &charsOut);
					s.append(buffer);
					if (cp2)
					{
						size_t charsOut2;
						char buffer2[5] = { 0, 0, 0, 0, 0 };
						Impl::ConvertUtf32ToUtf8(cp2, buffer2, countof(buffer2), &charsOut2);
						s.append(buffer);
						charsOut += charsOut2;
					}
				}
				else
				{
					s.append(curr, charsOut);
				}
				curr += charsOut;
			}
		}

		return s;
	}

} // namespace Jinx::Impl



// end --- JxUnicode.cpp --- 



// begin --- JxUnicodeCaseFolding.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
*/

// Note: This file is auto-generated using the CaseFolding.txt data file

namespace Jinx::Impl
{

	struct FoldTable
	{
		static inline const CaseFoldingData caseFoldingTable[] =
		{
			{ 0x0041, 0x0061, 0x0000},
			{ 0x0042, 0x0062, 0x0000},
			{ 0x0043, 0x0063, 0x0000},
			{ 0x0044, 0x0064, 0x0000},
			{ 0x0045, 0x0065, 0x0000},
			{ 0x0046, 0x0066, 0x0000},
			{ 0x0047, 0x0067, 0x0000},
			{ 0x0048, 0x0068, 0x0000},
			{ 0x0049, 0x0069, 0x0000},
			{ 0x004A, 0x006A, 0x0000},
			{ 0x004B, 0x006B, 0x0000},
			{ 0x004C, 0x006C, 0x0000},
			{ 0x004D, 0x006D, 0x0000},
			{ 0x004E, 0x006E, 0x0000},
			{ 0x004F, 0x006F, 0x0000},
			{ 0x0050, 0x0070, 0x0000},
			{ 0x0051, 0x0071, 0x0000},
			{ 0x0052, 0x0072, 0x0000},
			{ 0x0053, 0x0073, 0x0000},
			{ 0x0054, 0x0074, 0x0000},
			{ 0x0055, 0x0075, 0x0000},
			{ 0x0056, 0x0076, 0x0000},
			{ 0x0057, 0x0077, 0x0000},
			{ 0x0058, 0x0078, 0x0000},
			{ 0x0059, 0x0079, 0x0000},
			{ 0x005A, 0x007A, 0x0000},
			{ 0x00B5, 0x03BC, 0x0000},
			{ 0x00C0, 0x00E0, 0x0000},
			{ 0x00C1, 0x00E1, 0x0000},
			{ 0x00C2, 0x00E2, 0x0000},
			{ 0x00C3, 0x00E3, 0x0000},
			{ 0x00C4, 0x00E4, 0x0000},
			{ 0x00C5, 0x00E5, 0x0000},
			{ 0x00C6, 0x00E6, 0x0000},
			{ 0x00C7, 0x00E7, 0x0000},
			{ 0x00C8, 0x00E8, 0x0000},
			{ 0x00C9, 0x00E9, 0x0000},
			{ 0x00CA, 0x00EA, 0x0000},
			{ 0x00CB, 0x00EB, 0x0000},
			{ 0x00CC, 0x00EC, 0x0000},
			{ 0x00CD, 0x00ED, 0x0000},
			{ 0x00CE, 0x00EE, 0x0000},
			{ 0x00CF, 0x00EF, 0x0000},
			{ 0x00D0, 0x00F0, 0x0000},
			{ 0x00D1, 0x00F1, 0x0000},
			{ 0x00D2, 0x00F2, 0x0000},
			{ 0x00D3, 0x00F3, 0x0000},
			{ 0x00D4, 0x00F4, 0x0000},
			{ 0x00D5, 0x00F5, 0x0000},
			{ 0x00D6, 0x00F6, 0x0000},
			{ 0x00D8, 0x00F8, 0x0000},
			{ 0x00D9, 0x00F9, 0x0000},
			{ 0x00DA, 0x00FA, 0x0000},
			{ 0x00DB, 0x00FB, 0x0000},
			{ 0x00DC, 0x00FC, 0x0000},
			{ 0x00DD, 0x00FD, 0x0000},
			{ 0x00DE, 0x00FE, 0x0000},
			{ 0x00DF, 0x0073, 0x0073},
			{ 0x0100, 0x0101, 0x0000},
			{ 0x0102, 0x0103, 0x0000},
			{ 0x0104, 0x0105, 0x0000},
			{ 0x0106, 0x0107, 0x0000},
			{ 0x0108, 0x0109, 0x0000},
			{ 0x010A, 0x010B, 0x0000},
			{ 0x010C, 0x010D, 0x0000},
			{ 0x010E, 0x010F, 0x0000},
			{ 0x0110, 0x0111, 0x0000},
			{ 0x0112, 0x0113, 0x0000},
			{ 0x0114, 0x0115, 0x0000},
			{ 0x0116, 0x0117, 0x0000},
			{ 0x0118, 0x0119, 0x0000},
			{ 0x011A, 0x011B, 0x0000},
			{ 0x011C, 0x011D, 0x0000},
			{ 0x011E, 0x011F, 0x0000},
			{ 0x0120, 0x0121, 0x0000},
			{ 0x0122, 0x0123, 0x0000},
			{ 0x0124, 0x0125, 0x0000},
			{ 0x0126, 0x0127, 0x0000},
			{ 0x0128, 0x0129, 0x0000},
			{ 0x012A, 0x012B, 0x0000},
			{ 0x012C, 0x012D, 0x0000},
			{ 0x012E, 0x012F, 0x0000},
			{ 0x0130, 0x0069, 0x0307},
			{ 0x0132, 0x0133, 0x0000},
			{ 0x0134, 0x0135, 0x0000},
			{ 0x0136, 0x0137, 0x0000},
			{ 0x0139, 0x013A, 0x0000},
			{ 0x013B, 0x013C, 0x0000},
			{ 0x013D, 0x013E, 0x0000},
			{ 0x013F, 0x0140, 0x0000},
			{ 0x0141, 0x0142, 0x0000},
			{ 0x0143, 0x0144, 0x0000},
			{ 0x0145, 0x0146, 0x0000},
			{ 0x0147, 0x0148, 0x0000},
			{ 0x0149, 0x02BC, 0x006E},
			{ 0x014A, 0x014B, 0x0000},
			{ 0x014C, 0x014D, 0x0000},
			{ 0x014E, 0x014F, 0x0000},
			{ 0x0150, 0x0151, 0x0000},
			{ 0x0152, 0x0153, 0x0000},
			{ 0x0154, 0x0155, 0x0000},
			{ 0x0156, 0x0157, 0x0000},
			{ 0x0158, 0x0159, 0x0000},
			{ 0x015A, 0x015B, 0x0000},
			{ 0x015C, 0x015D, 0x0000},
			{ 0x015E, 0x015F, 0x0000},
			{ 0x0160, 0x0161, 0x0000},
			{ 0x0162, 0x0163, 0x0000},
			{ 0x0164, 0x0165, 0x0000},
			{ 0x0166, 0x0167, 0x0000},
			{ 0x0168, 0x0169, 0x0000},
			{ 0x016A, 0x016B, 0x0000},
			{ 0x016C, 0x016D, 0x0000},
			{ 0x016E, 0x016F, 0x0000},
			{ 0x0170, 0x0171, 0x0000},
			{ 0x0172, 0x0173, 0x0000},
			{ 0x0174, 0x0175, 0x0000},
			{ 0x0176, 0x0177, 0x0000},
			{ 0x0178, 0x00FF, 0x0000},
			{ 0x0179, 0x017A, 0x0000},
			{ 0x017B, 0x017C, 0x0000},
			{ 0x017D, 0x017E, 0x0000},
			{ 0x017F, 0x0073, 0x0000},
			{ 0x0181, 0x0253, 0x0000},
			{ 0x0182, 0x0183, 0x0000},
			{ 0x0184, 0x0185, 0x0000},
			{ 0x0186, 0x0254, 0x0000},
			{ 0x0187, 0x0188, 0x0000},
			{ 0x0189, 0x0256, 0x0000},
			{ 0x018A, 0x0257, 0x0000},
			{ 0x018B, 0x018C, 0x0000},
			{ 0x018E, 0x01DD, 0x0000},
			{ 0x018F, 0x0259, 0x0000},
			{ 0x0190, 0x025B, 0x0000},
			{ 0x0191, 0x0192, 0x0000},
			{ 0x0193, 0x0260, 0x0000},
			{ 0x0194, 0x0263, 0x0000},
			{ 0x0196, 0x0269, 0x0000},
			{ 0x0197, 0x0268, 0x0000},
			{ 0x0198, 0x0199, 0x0000},
			{ 0x019C, 0x026F, 0x0000},
			{ 0x019D, 0x0272, 0x0000},
			{ 0x019F, 0x0275, 0x0000},
			{ 0x01A0, 0x01A1, 0x0000},
			{ 0x01A2, 0x01A3, 0x0000},
			{ 0x01A4, 0x01A5, 0x0000},
			{ 0x01A6, 0x0280, 0x0000},
			{ 0x01A7, 0x01A8, 0x0000},
			{ 0x01A9, 0x0283, 0x0000},
			{ 0x01AC, 0x01AD, 0x0000},
			{ 0x01AE, 0x0288, 0x0000},
			{ 0x01AF, 0x01B0, 0x0000},
			{ 0x01B1, 0x028A, 0x0000},
			{ 0x01B2, 0x028B, 0x0000},
			{ 0x01B3, 0x01B4, 0x0000},
			{ 0x01B5, 0x01B6, 0x0000},
			{ 0x01B7, 0x0292, 0x0000},
			{ 0x01B8, 0x01B9, 0x0000},
			{ 0x01BC, 0x01BD, 0x0000},
			{ 0x01C4, 0x01C6, 0x0000},
			{ 0x01C5, 0x01C6, 0x0000},
			{ 0x01C7, 0x01C9, 0x0000},
			{ 0x01C8, 0x01C9, 0x0000},
			{ 0x01CA, 0x01CC, 0x0000},
			{ 0x01CB, 0x01CC, 0x0000},
			{ 0x01CD, 0x01CE, 0x0000},
			{ 0x01CF, 0x01D0, 0x0000},
			{ 0x01D1, 0x01D2, 0x0000},
			{ 0x01D3, 0x01D4, 0x0000},
			{ 0x01D5, 0x01D6, 0x0000},
			{ 0x01D7, 0x01D8, 0x0000},
			{ 0x01D9, 0x01DA, 0x0000},
			{ 0x01DB, 0x01DC, 0x0000},
			{ 0x01DE, 0x01DF, 0x0000},
			{ 0x01E0, 0x01E1, 0x0000},
			{ 0x01E2, 0x01E3, 0x0000},
			{ 0x01E4, 0x01E5, 0x0000},
			{ 0x01E6, 0x01E7, 0x0000},
			{ 0x01E8, 0x01E9, 0x0000},
			{ 0x01EA, 0x01EB, 0x0000},
			{ 0x01EC, 0x01ED, 0x0000},
			{ 0x01EE, 0x01EF, 0x0000},
			{ 0x01F0, 0x006A, 0x030C},
			{ 0x01F1, 0x01F3, 0x0000},
			{ 0x01F2, 0x01F3, 0x0000},
			{ 0x01F4, 0x01F5, 0x0000},
			{ 0x01F6, 0x0195, 0x0000},
			{ 0x01F7, 0x01BF, 0x0000},
			{ 0x01F8, 0x01F9, 0x0000},
			{ 0x01FA, 0x01FB, 0x0000},
			{ 0x01FC, 0x01FD, 0x0000},
			{ 0x01FE, 0x01FF, 0x0000},
			{ 0x0200, 0x0201, 0x0000},
			{ 0x0202, 0x0203, 0x0000},
			{ 0x0204, 0x0205, 0x0000},
			{ 0x0206, 0x0207, 0x0000},
			{ 0x0208, 0x0209, 0x0000},
			{ 0x020A, 0x020B, 0x0000},
			{ 0x020C, 0x020D, 0x0000},
			{ 0x020E, 0x020F, 0x0000},
			{ 0x0210, 0x0211, 0x0000},
			{ 0x0212, 0x0213, 0x0000},
			{ 0x0214, 0x0215, 0x0000},
			{ 0x0216, 0x0217, 0x0000},
			{ 0x0218, 0x0219, 0x0000},
			{ 0x021A, 0x021B, 0x0000},
			{ 0x021C, 0x021D, 0x0000},
			{ 0x021E, 0x021F, 0x0000},
			{ 0x0220, 0x019E, 0x0000},
			{ 0x0222, 0x0223, 0x0000},
			{ 0x0224, 0x0225, 0x0000},
			{ 0x0226, 0x0227, 0x0000},
			{ 0x0228, 0x0229, 0x0000},
			{ 0x022A, 0x022B, 0x0000},
			{ 0x022C, 0x022D, 0x0000},
			{ 0x022E, 0x022F, 0x0000},
			{ 0x0230, 0x0231, 0x0000},
			{ 0x0232, 0x0233, 0x0000},
			{ 0x023A, 0x2C65, 0x0000},
			{ 0x023B, 0x023C, 0x0000},
			{ 0x023D, 0x019A, 0x0000},
			{ 0x023E, 0x2C66, 0x0000},
			{ 0x0241, 0x0242, 0x0000},
			{ 0x0243, 0x0180, 0x0000},
			{ 0x0244, 0x0289, 0x0000},
			{ 0x0245, 0x028C, 0x0000},
			{ 0x0246, 0x0247, 0x0000},
			{ 0x0248, 0x0249, 0x0000},
			{ 0x024A, 0x024B, 0x0000},
			{ 0x024C, 0x024D, 0x0000},
			{ 0x024E, 0x024F, 0x0000},
			{ 0x0345, 0x03B9, 0x0000},
			{ 0x0370, 0x0371, 0x0000},
			{ 0x0372, 0x0373, 0x0000},
			{ 0x0376, 0x0377, 0x0000},
			{ 0x037F, 0x03F3, 0x0000},
			{ 0x0386, 0x03AC, 0x0000},
			{ 0x0388, 0x03AD, 0x0000},
			{ 0x0389, 0x03AE, 0x0000},
			{ 0x038A, 0x03AF, 0x0000},
			{ 0x038C, 0x03CC, 0x0000},
			{ 0x038E, 0x03CD, 0x0000},
			{ 0x038F, 0x03CE, 0x0000},
			{ 0x0390, 0x03B9, 0x0308},
			{ 0x0391, 0x03B1, 0x0000},
			{ 0x0392, 0x03B2, 0x0000},
			{ 0x0393, 0x03B3, 0x0000},
			{ 0x0394, 0x03B4, 0x0000},
			{ 0x0395, 0x03B5, 0x0000},
			{ 0x0396, 0x03B6, 0x0000},
			{ 0x0397, 0x03B7, 0x0000},
			{ 0x0398, 0x03B8, 0x0000},
			{ 0x0399, 0x03B9, 0x0000},
			{ 0x039A, 0x03BA, 0x0000},
			{ 0x039B, 0x03BB, 0x0000},
			{ 0x039C, 0x03BC, 0x0000},
			{ 0x039D, 0x03BD, 0x0000},
			{ 0x039E, 0x03BE, 0x0000},
			{ 0x039F, 0x03BF, 0x0000},
			{ 0x03A0, 0x03C0, 0x0000},
			{ 0x03A1, 0x03C1, 0x0000},
			{ 0x03A3, 0x03C3, 0x0000},
			{ 0x03A4, 0x03C4, 0x0000},
			{ 0x03A5, 0x03C5, 0x0000},
			{ 0x03A6, 0x03C6, 0x0000},
			{ 0x03A7, 0x03C7, 0x0000},
			{ 0x03A8, 0x03C8, 0x0000},
			{ 0x03A9, 0x03C9, 0x0000},
			{ 0x03AA, 0x03CA, 0x0000},
			{ 0x03AB, 0x03CB, 0x0000},
			{ 0x03B0, 0x03C5, 0x0308},
			{ 0x03C2, 0x03C3, 0x0000},
			{ 0x03CF, 0x03D7, 0x0000},
			{ 0x03D0, 0x03B2, 0x0000},
			{ 0x03D1, 0x03B8, 0x0000},
			{ 0x03D5, 0x03C6, 0x0000},
			{ 0x03D6, 0x03C0, 0x0000},
			{ 0x03D8, 0x03D9, 0x0000},
			{ 0x03DA, 0x03DB, 0x0000},
			{ 0x03DC, 0x03DD, 0x0000},
			{ 0x03DE, 0x03DF, 0x0000},
			{ 0x03E0, 0x03E1, 0x0000},
			{ 0x03E2, 0x03E3, 0x0000},
			{ 0x03E4, 0x03E5, 0x0000},
			{ 0x03E6, 0x03E7, 0x0000},
			{ 0x03E8, 0x03E9, 0x0000},
			{ 0x03EA, 0x03EB, 0x0000},
			{ 0x03EC, 0x03ED, 0x0000},
			{ 0x03EE, 0x03EF, 0x0000},
			{ 0x03F0, 0x03BA, 0x0000},
			{ 0x03F1, 0x03C1, 0x0000},
			{ 0x03F4, 0x03B8, 0x0000},
			{ 0x03F5, 0x03B5, 0x0000},
			{ 0x03F7, 0x03F8, 0x0000},
			{ 0x03F9, 0x03F2, 0x0000},
			{ 0x03FA, 0x03FB, 0x0000},
			{ 0x03FD, 0x037B, 0x0000},
			{ 0x03FE, 0x037C, 0x0000},
			{ 0x03FF, 0x037D, 0x0000},
			{ 0x0400, 0x0450, 0x0000},
			{ 0x0401, 0x0451, 0x0000},
			{ 0x0402, 0x0452, 0x0000},
			{ 0x0403, 0x0453, 0x0000},
			{ 0x0404, 0x0454, 0x0000},
			{ 0x0405, 0x0455, 0x0000},
			{ 0x0406, 0x0456, 0x0000},
			{ 0x0407, 0x0457, 0x0000},
			{ 0x0408, 0x0458, 0x0000},
			{ 0x0409, 0x0459, 0x0000},
			{ 0x040A, 0x045A, 0x0000},
			{ 0x040B, 0x045B, 0x0000},
			{ 0x040C, 0x045C, 0x0000},
			{ 0x040D, 0x045D, 0x0000},
			{ 0x040E, 0x045E, 0x0000},
			{ 0x040F, 0x045F, 0x0000},
			{ 0x0410, 0x0430, 0x0000},
			{ 0x0411, 0x0431, 0x0000},
			{ 0x0412, 0x0432, 0x0000},
			{ 0x0413, 0x0433, 0x0000},
			{ 0x0414, 0x0434, 0x0000},
			{ 0x0415, 0x0435, 0x0000},
			{ 0x0416, 0x0436, 0x0000},
			{ 0x0417, 0x0437, 0x0000},
			{ 0x0418, 0x0438, 0x0000},
			{ 0x0419, 0x0439, 0x0000},
			{ 0x041A, 0x043A, 0x0000},
			{ 0x041B, 0x043B, 0x0000},
			{ 0x041C, 0x043C, 0x0000},
			{ 0x041D, 0x043D, 0x0000},
			{ 0x041E, 0x043E, 0x0000},
			{ 0x041F, 0x043F, 0x0000},
			{ 0x0420, 0x0440, 0x0000},
			{ 0x0421, 0x0441, 0x0000},
			{ 0x0422, 0x0442, 0x0000},
			{ 0x0423, 0x0443, 0x0000},
			{ 0x0424, 0x0444, 0x0000},
			{ 0x0425, 0x0445, 0x0000},
			{ 0x0426, 0x0446, 0x0000},
			{ 0x0427, 0x0447, 0x0000},
			{ 0x0428, 0x0448, 0x0000},
			{ 0x0429, 0x0449, 0x0000},
			{ 0x042A, 0x044A, 0x0000},
			{ 0x042B, 0x044B, 0x0000},
			{ 0x042C, 0x044C, 0x0000},
			{ 0x042D, 0x044D, 0x0000},
			{ 0x042E, 0x044E, 0x0000},
			{ 0x042F, 0x044F, 0x0000},
			{ 0x0460, 0x0461, 0x0000},
			{ 0x0462, 0x0463, 0x0000},
			{ 0x0464, 0x0465, 0x0000},
			{ 0x0466, 0x0467, 0x0000},
			{ 0x0468, 0x0469, 0x0000},
			{ 0x046A, 0x046B, 0x0000},
			{ 0x046C, 0x046D, 0x0000},
			{ 0x046E, 0x046F, 0x0000},
			{ 0x0470, 0x0471, 0x0000},
			{ 0x0472, 0x0473, 0x0000},
			{ 0x0474, 0x0475, 0x0000},
			{ 0x0476, 0x0477, 0x0000},
			{ 0x0478, 0x0479, 0x0000},
			{ 0x047A, 0x047B, 0x0000},
			{ 0x047C, 0x047D, 0x0000},
			{ 0x047E, 0x047F, 0x0000},
			{ 0x0480, 0x0481, 0x0000},
			{ 0x048A, 0x048B, 0x0000},
			{ 0x048C, 0x048D, 0x0000},
			{ 0x048E, 0x048F, 0x0000},
			{ 0x0490, 0x0491, 0x0000},
			{ 0x0492, 0x0493, 0x0000},
			{ 0x0494, 0x0495, 0x0000},
			{ 0x0496, 0x0497, 0x0000},
			{ 0x0498, 0x0499, 0x0000},
			{ 0x049A, 0x049B, 0x0000},
			{ 0x049C, 0x049D, 0x0000},
			{ 0x049E, 0x049F, 0x0000},
			{ 0x04A0, 0x04A1, 0x0000},
			{ 0x04A2, 0x04A3, 0x0000},
			{ 0x04A4, 0x04A5, 0x0000},
			{ 0x04A6, 0x04A7, 0x0000},
			{ 0x04A8, 0x04A9, 0x0000},
			{ 0x04AA, 0x04AB, 0x0000},
			{ 0x04AC, 0x04AD, 0x0000},
			{ 0x04AE, 0x04AF, 0x0000},
			{ 0x04B0, 0x04B1, 0x0000},
			{ 0x04B2, 0x04B3, 0x0000},
			{ 0x04B4, 0x04B5, 0x0000},
			{ 0x04B6, 0x04B7, 0x0000},
			{ 0x04B8, 0x04B9, 0x0000},
			{ 0x04BA, 0x04BB, 0x0000},
			{ 0x04BC, 0x04BD, 0x0000},
			{ 0x04BE, 0x04BF, 0x0000},
			{ 0x04C0, 0x04CF, 0x0000},
			{ 0x04C1, 0x04C2, 0x0000},
			{ 0x04C3, 0x04C4, 0x0000},
			{ 0x04C5, 0x04C6, 0x0000},
			{ 0x04C7, 0x04C8, 0x0000},
			{ 0x04C9, 0x04CA, 0x0000},
			{ 0x04CB, 0x04CC, 0x0000},
			{ 0x04CD, 0x04CE, 0x0000},
			{ 0x04D0, 0x04D1, 0x0000},
			{ 0x04D2, 0x04D3, 0x0000},
			{ 0x04D4, 0x04D5, 0x0000},
			{ 0x04D6, 0x04D7, 0x0000},
			{ 0x04D8, 0x04D9, 0x0000},
			{ 0x04DA, 0x04DB, 0x0000},
			{ 0x04DC, 0x04DD, 0x0000},
			{ 0x04DE, 0x04DF, 0x0000},
			{ 0x04E0, 0x04E1, 0x0000},
			{ 0x04E2, 0x04E3, 0x0000},
			{ 0x04E4, 0x04E5, 0x0000},
			{ 0x04E6, 0x04E7, 0x0000},
			{ 0x04E8, 0x04E9, 0x0000},
			{ 0x04EA, 0x04EB, 0x0000},
			{ 0x04EC, 0x04ED, 0x0000},
			{ 0x04EE, 0x04EF, 0x0000},
			{ 0x04F0, 0x04F1, 0x0000},
			{ 0x04F2, 0x04F3, 0x0000},
			{ 0x04F4, 0x04F5, 0x0000},
			{ 0x04F6, 0x04F7, 0x0000},
			{ 0x04F8, 0x04F9, 0x0000},
			{ 0x04FA, 0x04FB, 0x0000},
			{ 0x04FC, 0x04FD, 0x0000},
			{ 0x04FE, 0x04FF, 0x0000},
			{ 0x0500, 0x0501, 0x0000},
			{ 0x0502, 0x0503, 0x0000},
			{ 0x0504, 0x0505, 0x0000},
			{ 0x0506, 0x0507, 0x0000},
			{ 0x0508, 0x0509, 0x0000},
			{ 0x050A, 0x050B, 0x0000},
			{ 0x050C, 0x050D, 0x0000},
			{ 0x050E, 0x050F, 0x0000},
			{ 0x0510, 0x0511, 0x0000},
			{ 0x0512, 0x0513, 0x0000},
			{ 0x0514, 0x0515, 0x0000},
			{ 0x0516, 0x0517, 0x0000},
			{ 0x0518, 0x0519, 0x0000},
			{ 0x051A, 0x051B, 0x0000},
			{ 0x051C, 0x051D, 0x0000},
			{ 0x051E, 0x051F, 0x0000},
			{ 0x0520, 0x0521, 0x0000},
			{ 0x0522, 0x0523, 0x0000},
			{ 0x0524, 0x0525, 0x0000},
			{ 0x0526, 0x0527, 0x0000},
			{ 0x0528, 0x0529, 0x0000},
			{ 0x052A, 0x052B, 0x0000},
			{ 0x052C, 0x052D, 0x0000},
			{ 0x052E, 0x052F, 0x0000},
			{ 0x0531, 0x0561, 0x0000},
			{ 0x0532, 0x0562, 0x0000},
			{ 0x0533, 0x0563, 0x0000},
			{ 0x0534, 0x0564, 0x0000},
			{ 0x0535, 0x0565, 0x0000},
			{ 0x0536, 0x0566, 0x0000},
			{ 0x0537, 0x0567, 0x0000},
			{ 0x0538, 0x0568, 0x0000},
			{ 0x0539, 0x0569, 0x0000},
			{ 0x053A, 0x056A, 0x0000},
			{ 0x053B, 0x056B, 0x0000},
			{ 0x053C, 0x056C, 0x0000},
			{ 0x053D, 0x056D, 0x0000},
			{ 0x053E, 0x056E, 0x0000},
			{ 0x053F, 0x056F, 0x0000},
			{ 0x0540, 0x0570, 0x0000},
			{ 0x0541, 0x0571, 0x0000},
			{ 0x0542, 0x0572, 0x0000},
			{ 0x0543, 0x0573, 0x0000},
			{ 0x0544, 0x0574, 0x0000},
			{ 0x0545, 0x0575, 0x0000},
			{ 0x0546, 0x0576, 0x0000},
			{ 0x0547, 0x0577, 0x0000},
			{ 0x0548, 0x0578, 0x0000},
			{ 0x0549, 0x0579, 0x0000},
			{ 0x054A, 0x057A, 0x0000},
			{ 0x054B, 0x057B, 0x0000},
			{ 0x054C, 0x057C, 0x0000},
			{ 0x054D, 0x057D, 0x0000},
			{ 0x054E, 0x057E, 0x0000},
			{ 0x054F, 0x057F, 0x0000},
			{ 0x0550, 0x0580, 0x0000},
			{ 0x0551, 0x0581, 0x0000},
			{ 0x0552, 0x0582, 0x0000},
			{ 0x0553, 0x0583, 0x0000},
			{ 0x0554, 0x0584, 0x0000},
			{ 0x0555, 0x0585, 0x0000},
			{ 0x0556, 0x0586, 0x0000},
			{ 0x0587, 0x0565, 0x0582},
			{ 0x10A0, 0x2D00, 0x0000},
			{ 0x10A1, 0x2D01, 0x0000},
			{ 0x10A2, 0x2D02, 0x0000},
			{ 0x10A3, 0x2D03, 0x0000},
			{ 0x10A4, 0x2D04, 0x0000},
			{ 0x10A5, 0x2D05, 0x0000},
			{ 0x10A6, 0x2D06, 0x0000},
			{ 0x10A7, 0x2D07, 0x0000},
			{ 0x10A8, 0x2D08, 0x0000},
			{ 0x10A9, 0x2D09, 0x0000},
			{ 0x10AA, 0x2D0A, 0x0000},
			{ 0x10AB, 0x2D0B, 0x0000},
			{ 0x10AC, 0x2D0C, 0x0000},
			{ 0x10AD, 0x2D0D, 0x0000},
			{ 0x10AE, 0x2D0E, 0x0000},
			{ 0x10AF, 0x2D0F, 0x0000},
			{ 0x10B0, 0x2D10, 0x0000},
			{ 0x10B1, 0x2D11, 0x0000},
			{ 0x10B2, 0x2D12, 0x0000},
			{ 0x10B3, 0x2D13, 0x0000},
			{ 0x10B4, 0x2D14, 0x0000},
			{ 0x10B5, 0x2D15, 0x0000},
			{ 0x10B6, 0x2D16, 0x0000},
			{ 0x10B7, 0x2D17, 0x0000},
			{ 0x10B8, 0x2D18, 0x0000},
			{ 0x10B9, 0x2D19, 0x0000},
			{ 0x10BA, 0x2D1A, 0x0000},
			{ 0x10BB, 0x2D1B, 0x0000},
			{ 0x10BC, 0x2D1C, 0x0000},
			{ 0x10BD, 0x2D1D, 0x0000},
			{ 0x10BE, 0x2D1E, 0x0000},
			{ 0x10BF, 0x2D1F, 0x0000},
			{ 0x10C0, 0x2D20, 0x0000},
			{ 0x10C1, 0x2D21, 0x0000},
			{ 0x10C2, 0x2D22, 0x0000},
			{ 0x10C3, 0x2D23, 0x0000},
			{ 0x10C4, 0x2D24, 0x0000},
			{ 0x10C5, 0x2D25, 0x0000},
			{ 0x10C7, 0x2D27, 0x0000},
			{ 0x10CD, 0x2D2D, 0x0000},
			{ 0x13F8, 0x13F0, 0x0000},
			{ 0x13F9, 0x13F1, 0x0000},
			{ 0x13FA, 0x13F2, 0x0000},
			{ 0x13FB, 0x13F3, 0x0000},
			{ 0x13FC, 0x13F4, 0x0000},
			{ 0x13FD, 0x13F5, 0x0000},
			{ 0x1C80, 0x0432, 0x0000},
			{ 0x1C81, 0x0434, 0x0000},
			{ 0x1C82, 0x043E, 0x0000},
			{ 0x1C83, 0x0441, 0x0000},
			{ 0x1C84, 0x0442, 0x0000},
			{ 0x1C85, 0x0442, 0x0000},
			{ 0x1C86, 0x044A, 0x0000},
			{ 0x1C87, 0x0463, 0x0000},
			{ 0x1C88, 0xA64B, 0x0000},
			{ 0x1E00, 0x1E01, 0x0000},
			{ 0x1E02, 0x1E03, 0x0000},
			{ 0x1E04, 0x1E05, 0x0000},
			{ 0x1E06, 0x1E07, 0x0000},
			{ 0x1E08, 0x1E09, 0x0000},
			{ 0x1E0A, 0x1E0B, 0x0000},
			{ 0x1E0C, 0x1E0D, 0x0000},
			{ 0x1E0E, 0x1E0F, 0x0000},
			{ 0x1E10, 0x1E11, 0x0000},
			{ 0x1E12, 0x1E13, 0x0000},
			{ 0x1E14, 0x1E15, 0x0000},
			{ 0x1E16, 0x1E17, 0x0000},
			{ 0x1E18, 0x1E19, 0x0000},
			{ 0x1E1A, 0x1E1B, 0x0000},
			{ 0x1E1C, 0x1E1D, 0x0000},
			{ 0x1E1E, 0x1E1F, 0x0000},
			{ 0x1E20, 0x1E21, 0x0000},
			{ 0x1E22, 0x1E23, 0x0000},
			{ 0x1E24, 0x1E25, 0x0000},
			{ 0x1E26, 0x1E27, 0x0000},
			{ 0x1E28, 0x1E29, 0x0000},
			{ 0x1E2A, 0x1E2B, 0x0000},
			{ 0x1E2C, 0x1E2D, 0x0000},
			{ 0x1E2E, 0x1E2F, 0x0000},
			{ 0x1E30, 0x1E31, 0x0000},
			{ 0x1E32, 0x1E33, 0x0000},
			{ 0x1E34, 0x1E35, 0x0000},
			{ 0x1E36, 0x1E37, 0x0000},
			{ 0x1E38, 0x1E39, 0x0000},
			{ 0x1E3A, 0x1E3B, 0x0000},
			{ 0x1E3C, 0x1E3D, 0x0000},
			{ 0x1E3E, 0x1E3F, 0x0000},
			{ 0x1E40, 0x1E41, 0x0000},
			{ 0x1E42, 0x1E43, 0x0000},
			{ 0x1E44, 0x1E45, 0x0000},
			{ 0x1E46, 0x1E47, 0x0000},
			{ 0x1E48, 0x1E49, 0x0000},
			{ 0x1E4A, 0x1E4B, 0x0000},
			{ 0x1E4C, 0x1E4D, 0x0000},
			{ 0x1E4E, 0x1E4F, 0x0000},
			{ 0x1E50, 0x1E51, 0x0000},
			{ 0x1E52, 0x1E53, 0x0000},
			{ 0x1E54, 0x1E55, 0x0000},
			{ 0x1E56, 0x1E57, 0x0000},
			{ 0x1E58, 0x1E59, 0x0000},
			{ 0x1E5A, 0x1E5B, 0x0000},
			{ 0x1E5C, 0x1E5D, 0x0000},
			{ 0x1E5E, 0x1E5F, 0x0000},
			{ 0x1E60, 0x1E61, 0x0000},
			{ 0x1E62, 0x1E63, 0x0000},
			{ 0x1E64, 0x1E65, 0x0000},
			{ 0x1E66, 0x1E67, 0x0000},
			{ 0x1E68, 0x1E69, 0x0000},
			{ 0x1E6A, 0x1E6B, 0x0000},
			{ 0x1E6C, 0x1E6D, 0x0000},
			{ 0x1E6E, 0x1E6F, 0x0000},
			{ 0x1E70, 0x1E71, 0x0000},
			{ 0x1E72, 0x1E73, 0x0000},
			{ 0x1E74, 0x1E75, 0x0000},
			{ 0x1E76, 0x1E77, 0x0000},
			{ 0x1E78, 0x1E79, 0x0000},
			{ 0x1E7A, 0x1E7B, 0x0000},
			{ 0x1E7C, 0x1E7D, 0x0000},
			{ 0x1E7E, 0x1E7F, 0x0000},
			{ 0x1E80, 0x1E81, 0x0000},
			{ 0x1E82, 0x1E83, 0x0000},
			{ 0x1E84, 0x1E85, 0x0000},
			{ 0x1E86, 0x1E87, 0x0000},
			{ 0x1E88, 0x1E89, 0x0000},
			{ 0x1E8A, 0x1E8B, 0x0000},
			{ 0x1E8C, 0x1E8D, 0x0000},
			{ 0x1E8E, 0x1E8F, 0x0000},
			{ 0x1E90, 0x1E91, 0x0000},
			{ 0x1E92, 0x1E93, 0x0000},
			{ 0x1E94, 0x1E95, 0x0000},
			{ 0x1E96, 0x0068, 0x0331},
			{ 0x1E97, 0x0074, 0x0308},
			{ 0x1E98, 0x0077, 0x030A},
			{ 0x1E99, 0x0079, 0x030A},
			{ 0x1E9A, 0x0061, 0x02BE},
			{ 0x1E9B, 0x1E61, 0x0000},
			{ 0x1E9E, 0x0073, 0x0073},
			{ 0x1EA0, 0x1EA1, 0x0000},
			{ 0x1EA2, 0x1EA3, 0x0000},
			{ 0x1EA4, 0x1EA5, 0x0000},
			{ 0x1EA6, 0x1EA7, 0x0000},
			{ 0x1EA8, 0x1EA9, 0x0000},
			{ 0x1EAA, 0x1EAB, 0x0000},
			{ 0x1EAC, 0x1EAD, 0x0000},
			{ 0x1EAE, 0x1EAF, 0x0000},
			{ 0x1EB0, 0x1EB1, 0x0000},
			{ 0x1EB2, 0x1EB3, 0x0000},
			{ 0x1EB4, 0x1EB5, 0x0000},
			{ 0x1EB6, 0x1EB7, 0x0000},
			{ 0x1EB8, 0x1EB9, 0x0000},
			{ 0x1EBA, 0x1EBB, 0x0000},
			{ 0x1EBC, 0x1EBD, 0x0000},
			{ 0x1EBE, 0x1EBF, 0x0000},
			{ 0x1EC0, 0x1EC1, 0x0000},
			{ 0x1EC2, 0x1EC3, 0x0000},
			{ 0x1EC4, 0x1EC5, 0x0000},
			{ 0x1EC6, 0x1EC7, 0x0000},
			{ 0x1EC8, 0x1EC9, 0x0000},
			{ 0x1ECA, 0x1ECB, 0x0000},
			{ 0x1ECC, 0x1ECD, 0x0000},
			{ 0x1ECE, 0x1ECF, 0x0000},
			{ 0x1ED0, 0x1ED1, 0x0000},
			{ 0x1ED2, 0x1ED3, 0x0000},
			{ 0x1ED4, 0x1ED5, 0x0000},
			{ 0x1ED6, 0x1ED7, 0x0000},
			{ 0x1ED8, 0x1ED9, 0x0000},
			{ 0x1EDA, 0x1EDB, 0x0000},
			{ 0x1EDC, 0x1EDD, 0x0000},
			{ 0x1EDE, 0x1EDF, 0x0000},
			{ 0x1EE0, 0x1EE1, 0x0000},
			{ 0x1EE2, 0x1EE3, 0x0000},
			{ 0x1EE4, 0x1EE5, 0x0000},
			{ 0x1EE6, 0x1EE7, 0x0000},
			{ 0x1EE8, 0x1EE9, 0x0000},
			{ 0x1EEA, 0x1EEB, 0x0000},
			{ 0x1EEC, 0x1EED, 0x0000},
			{ 0x1EEE, 0x1EEF, 0x0000},
			{ 0x1EF0, 0x1EF1, 0x0000},
			{ 0x1EF2, 0x1EF3, 0x0000},
			{ 0x1EF4, 0x1EF5, 0x0000},
			{ 0x1EF6, 0x1EF7, 0x0000},
			{ 0x1EF8, 0x1EF9, 0x0000},
			{ 0x1EFA, 0x1EFB, 0x0000},
			{ 0x1EFC, 0x1EFD, 0x0000},
			{ 0x1EFE, 0x1EFF, 0x0000},
			{ 0x1F08, 0x1F00, 0x0000},
			{ 0x1F09, 0x1F01, 0x0000},
			{ 0x1F0A, 0x1F02, 0x0000},
			{ 0x1F0B, 0x1F03, 0x0000},
			{ 0x1F0C, 0x1F04, 0x0000},
			{ 0x1F0D, 0x1F05, 0x0000},
			{ 0x1F0E, 0x1F06, 0x0000},
			{ 0x1F0F, 0x1F07, 0x0000},
			{ 0x1F18, 0x1F10, 0x0000},
			{ 0x1F19, 0x1F11, 0x0000},
			{ 0x1F1A, 0x1F12, 0x0000},
			{ 0x1F1B, 0x1F13, 0x0000},
			{ 0x1F1C, 0x1F14, 0x0000},
			{ 0x1F1D, 0x1F15, 0x0000},
			{ 0x1F28, 0x1F20, 0x0000},
			{ 0x1F29, 0x1F21, 0x0000},
			{ 0x1F2A, 0x1F22, 0x0000},
			{ 0x1F2B, 0x1F23, 0x0000},
			{ 0x1F2C, 0x1F24, 0x0000},
			{ 0x1F2D, 0x1F25, 0x0000},
			{ 0x1F2E, 0x1F26, 0x0000},
			{ 0x1F2F, 0x1F27, 0x0000},
			{ 0x1F38, 0x1F30, 0x0000},
			{ 0x1F39, 0x1F31, 0x0000},
			{ 0x1F3A, 0x1F32, 0x0000},
			{ 0x1F3B, 0x1F33, 0x0000},
			{ 0x1F3C, 0x1F34, 0x0000},
			{ 0x1F3D, 0x1F35, 0x0000},
			{ 0x1F3E, 0x1F36, 0x0000},
			{ 0x1F3F, 0x1F37, 0x0000},
			{ 0x1F48, 0x1F40, 0x0000},
			{ 0x1F49, 0x1F41, 0x0000},
			{ 0x1F4A, 0x1F42, 0x0000},
			{ 0x1F4B, 0x1F43, 0x0000},
			{ 0x1F4C, 0x1F44, 0x0000},
			{ 0x1F4D, 0x1F45, 0x0000},
			{ 0x1F50, 0x03C5, 0x0313},
			{ 0x1F52, 0x03C5, 0x0313},
			{ 0x1F54, 0x03C5, 0x0313},
			{ 0x1F56, 0x03C5, 0x0313},
			{ 0x1F59, 0x1F51, 0x0000},
			{ 0x1F5B, 0x1F53, 0x0000},
			{ 0x1F5D, 0x1F55, 0x0000},
			{ 0x1F5F, 0x1F57, 0x0000},
			{ 0x1F68, 0x1F60, 0x0000},
			{ 0x1F69, 0x1F61, 0x0000},
			{ 0x1F6A, 0x1F62, 0x0000},
			{ 0x1F6B, 0x1F63, 0x0000},
			{ 0x1F6C, 0x1F64, 0x0000},
			{ 0x1F6D, 0x1F65, 0x0000},
			{ 0x1F6E, 0x1F66, 0x0000},
			{ 0x1F6F, 0x1F67, 0x0000},
			{ 0x1F80, 0x1F00, 0x03B9},
			{ 0x1F81, 0x1F01, 0x03B9},
			{ 0x1F82, 0x1F02, 0x03B9},
			{ 0x1F83, 0x1F03, 0x03B9},
			{ 0x1F84, 0x1F04, 0x03B9},
			{ 0x1F85, 0x1F05, 0x03B9},
			{ 0x1F86, 0x1F06, 0x03B9},
			{ 0x1F87, 0x1F07, 0x03B9},
			{ 0x1F88, 0x1F00, 0x03B9},
			{ 0x1F89, 0x1F01, 0x03B9},
			{ 0x1F8A, 0x1F02, 0x03B9},
			{ 0x1F8B, 0x1F03, 0x03B9},
			{ 0x1F8C, 0x1F04, 0x03B9},
			{ 0x1F8D, 0x1F05, 0x03B9},
			{ 0x1F8E, 0x1F06, 0x03B9},
			{ 0x1F8F, 0x1F07, 0x03B9},
			{ 0x1F90, 0x1F20, 0x03B9},
			{ 0x1F91, 0x1F21, 0x03B9},
			{ 0x1F92, 0x1F22, 0x03B9},
			{ 0x1F93, 0x1F23, 0x03B9},
			{ 0x1F94, 0x1F24, 0x03B9},
			{ 0x1F95, 0x1F25, 0x03B9},
			{ 0x1F96, 0x1F26, 0x03B9},
			{ 0x1F97, 0x1F27, 0x03B9},
			{ 0x1F98, 0x1F20, 0x03B9},
			{ 0x1F99, 0x1F21, 0x03B9},
			{ 0x1F9A, 0x1F22, 0x03B9},
			{ 0x1F9B, 0x1F23, 0x03B9},
			{ 0x1F9C, 0x1F24, 0x03B9},
			{ 0x1F9D, 0x1F25, 0x03B9},
			{ 0x1F9E, 0x1F26, 0x03B9},
			{ 0x1F9F, 0x1F27, 0x03B9},
			{ 0x1FA0, 0x1F60, 0x03B9},
			{ 0x1FA1, 0x1F61, 0x03B9},
			{ 0x1FA2, 0x1F62, 0x03B9},
			{ 0x1FA3, 0x1F63, 0x03B9},
			{ 0x1FA4, 0x1F64, 0x03B9},
			{ 0x1FA5, 0x1F65, 0x03B9},
			{ 0x1FA6, 0x1F66, 0x03B9},
			{ 0x1FA7, 0x1F67, 0x03B9},
			{ 0x1FA8, 0x1F60, 0x03B9},
			{ 0x1FA9, 0x1F61, 0x03B9},
			{ 0x1FAA, 0x1F62, 0x03B9},
			{ 0x1FAB, 0x1F63, 0x03B9},
			{ 0x1FAC, 0x1F64, 0x03B9},
			{ 0x1FAD, 0x1F65, 0x03B9},
			{ 0x1FAE, 0x1F66, 0x03B9},
			{ 0x1FAF, 0x1F67, 0x03B9},
			{ 0x1FB2, 0x1F70, 0x03B9},
			{ 0x1FB3, 0x03B1, 0x03B9},
			{ 0x1FB4, 0x03AC, 0x03B9},
			{ 0x1FB6, 0x03B1, 0x0342},
			{ 0x1FB7, 0x03B1, 0x0342},
			{ 0x1FB8, 0x1FB0, 0x0000},
			{ 0x1FB9, 0x1FB1, 0x0000},
			{ 0x1FBA, 0x1F70, 0x0000},
			{ 0x1FBB, 0x1F71, 0x0000},
			{ 0x1FBC, 0x03B1, 0x03B9},
			{ 0x1FBE, 0x03B9, 0x0000},
			{ 0x1FC2, 0x1F74, 0x03B9},
			{ 0x1FC3, 0x03B7, 0x03B9},
			{ 0x1FC4, 0x03AE, 0x03B9},
			{ 0x1FC6, 0x03B7, 0x0342},
			{ 0x1FC7, 0x03B7, 0x0342},
			{ 0x1FC8, 0x1F72, 0x0000},
			{ 0x1FC9, 0x1F73, 0x0000},
			{ 0x1FCA, 0x1F74, 0x0000},
			{ 0x1FCB, 0x1F75, 0x0000},
			{ 0x1FCC, 0x03B7, 0x03B9},
			{ 0x1FD2, 0x03B9, 0x0308},
			{ 0x1FD3, 0x03B9, 0x0308},
			{ 0x1FD6, 0x03B9, 0x0342},
			{ 0x1FD7, 0x03B9, 0x0308},
			{ 0x1FD8, 0x1FD0, 0x0000},
			{ 0x1FD9, 0x1FD1, 0x0000},
			{ 0x1FDA, 0x1F76, 0x0000},
			{ 0x1FDB, 0x1F77, 0x0000},
			{ 0x1FE2, 0x03C5, 0x0308},
			{ 0x1FE3, 0x03C5, 0x0308},
			{ 0x1FE4, 0x03C1, 0x0313},
			{ 0x1FE6, 0x03C5, 0x0342},
			{ 0x1FE7, 0x03C5, 0x0308},
			{ 0x1FE8, 0x1FE0, 0x0000},
			{ 0x1FE9, 0x1FE1, 0x0000},
			{ 0x1FEA, 0x1F7A, 0x0000},
			{ 0x1FEB, 0x1F7B, 0x0000},
			{ 0x1FEC, 0x1FE5, 0x0000},
			{ 0x1FF2, 0x1F7C, 0x03B9},
			{ 0x1FF3, 0x03C9, 0x03B9},
			{ 0x1FF4, 0x03CE, 0x03B9},
			{ 0x1FF6, 0x03C9, 0x0342},
			{ 0x1FF7, 0x03C9, 0x0342},
			{ 0x1FF8, 0x1F78, 0x0000},
			{ 0x1FF9, 0x1F79, 0x0000},
			{ 0x1FFA, 0x1F7C, 0x0000},
			{ 0x1FFB, 0x1F7D, 0x0000},
			{ 0x1FFC, 0x03C9, 0x03B9},
			{ 0x2126, 0x03C9, 0x0000},
			{ 0x212A, 0x006B, 0x0000},
			{ 0x212B, 0x00E5, 0x0000},
			{ 0x2132, 0x214E, 0x0000},
			{ 0x2160, 0x2170, 0x0000},
			{ 0x2161, 0x2171, 0x0000},
			{ 0x2162, 0x2172, 0x0000},
			{ 0x2163, 0x2173, 0x0000},
			{ 0x2164, 0x2174, 0x0000},
			{ 0x2165, 0x2175, 0x0000},
			{ 0x2166, 0x2176, 0x0000},
			{ 0x2167, 0x2177, 0x0000},
			{ 0x2168, 0x2178, 0x0000},
			{ 0x2169, 0x2179, 0x0000},
			{ 0x216A, 0x217A, 0x0000},
			{ 0x216B, 0x217B, 0x0000},
			{ 0x216C, 0x217C, 0x0000},
			{ 0x216D, 0x217D, 0x0000},
			{ 0x216E, 0x217E, 0x0000},
			{ 0x216F, 0x217F, 0x0000},
			{ 0x2183, 0x2184, 0x0000},
			{ 0x24B6, 0x24D0, 0x0000},
			{ 0x24B7, 0x24D1, 0x0000},
			{ 0x24B8, 0x24D2, 0x0000},
			{ 0x24B9, 0x24D3, 0x0000},
			{ 0x24BA, 0x24D4, 0x0000},
			{ 0x24BB, 0x24D5, 0x0000},
			{ 0x24BC, 0x24D6, 0x0000},
			{ 0x24BD, 0x24D7, 0x0000},
			{ 0x24BE, 0x24D8, 0x0000},
			{ 0x24BF, 0x24D9, 0x0000},
			{ 0x24C0, 0x24DA, 0x0000},
			{ 0x24C1, 0x24DB, 0x0000},
			{ 0x24C2, 0x24DC, 0x0000},
			{ 0x24C3, 0x24DD, 0x0000},
			{ 0x24C4, 0x24DE, 0x0000},
			{ 0x24C5, 0x24DF, 0x0000},
			{ 0x24C6, 0x24E0, 0x0000},
			{ 0x24C7, 0x24E1, 0x0000},
			{ 0x24C8, 0x24E2, 0x0000},
			{ 0x24C9, 0x24E3, 0x0000},
			{ 0x24CA, 0x24E4, 0x0000},
			{ 0x24CB, 0x24E5, 0x0000},
			{ 0x24CC, 0x24E6, 0x0000},
			{ 0x24CD, 0x24E7, 0x0000},
			{ 0x24CE, 0x24E8, 0x0000},
			{ 0x24CF, 0x24E9, 0x0000},
			{ 0x2C00, 0x2C30, 0x0000},
			{ 0x2C01, 0x2C31, 0x0000},
			{ 0x2C02, 0x2C32, 0x0000},
			{ 0x2C03, 0x2C33, 0x0000},
			{ 0x2C04, 0x2C34, 0x0000},
			{ 0x2C05, 0x2C35, 0x0000},
			{ 0x2C06, 0x2C36, 0x0000},
			{ 0x2C07, 0x2C37, 0x0000},
			{ 0x2C08, 0x2C38, 0x0000},
			{ 0x2C09, 0x2C39, 0x0000},
			{ 0x2C0A, 0x2C3A, 0x0000},
			{ 0x2C0B, 0x2C3B, 0x0000},
			{ 0x2C0C, 0x2C3C, 0x0000},
			{ 0x2C0D, 0x2C3D, 0x0000},
			{ 0x2C0E, 0x2C3E, 0x0000},
			{ 0x2C0F, 0x2C3F, 0x0000},
			{ 0x2C10, 0x2C40, 0x0000},
			{ 0x2C11, 0x2C41, 0x0000},
			{ 0x2C12, 0x2C42, 0x0000},
			{ 0x2C13, 0x2C43, 0x0000},
			{ 0x2C14, 0x2C44, 0x0000},
			{ 0x2C15, 0x2C45, 0x0000},
			{ 0x2C16, 0x2C46, 0x0000},
			{ 0x2C17, 0x2C47, 0x0000},
			{ 0x2C18, 0x2C48, 0x0000},
			{ 0x2C19, 0x2C49, 0x0000},
			{ 0x2C1A, 0x2C4A, 0x0000},
			{ 0x2C1B, 0x2C4B, 0x0000},
			{ 0x2C1C, 0x2C4C, 0x0000},
			{ 0x2C1D, 0x2C4D, 0x0000},
			{ 0x2C1E, 0x2C4E, 0x0000},
			{ 0x2C1F, 0x2C4F, 0x0000},
			{ 0x2C20, 0x2C50, 0x0000},
			{ 0x2C21, 0x2C51, 0x0000},
			{ 0x2C22, 0x2C52, 0x0000},
			{ 0x2C23, 0x2C53, 0x0000},
			{ 0x2C24, 0x2C54, 0x0000},
			{ 0x2C25, 0x2C55, 0x0000},
			{ 0x2C26, 0x2C56, 0x0000},
			{ 0x2C27, 0x2C57, 0x0000},
			{ 0x2C28, 0x2C58, 0x0000},
			{ 0x2C29, 0x2C59, 0x0000},
			{ 0x2C2A, 0x2C5A, 0x0000},
			{ 0x2C2B, 0x2C5B, 0x0000},
			{ 0x2C2C, 0x2C5C, 0x0000},
			{ 0x2C2D, 0x2C5D, 0x0000},
			{ 0x2C2E, 0x2C5E, 0x0000},
			{ 0x2C60, 0x2C61, 0x0000},
			{ 0x2C62, 0x026B, 0x0000},
			{ 0x2C63, 0x1D7D, 0x0000},
			{ 0x2C64, 0x027D, 0x0000},
			{ 0x2C67, 0x2C68, 0x0000},
			{ 0x2C69, 0x2C6A, 0x0000},
			{ 0x2C6B, 0x2C6C, 0x0000},
			{ 0x2C6D, 0x0251, 0x0000},
			{ 0x2C6E, 0x0271, 0x0000},
			{ 0x2C6F, 0x0250, 0x0000},
			{ 0x2C70, 0x0252, 0x0000},
			{ 0x2C72, 0x2C73, 0x0000},
			{ 0x2C75, 0x2C76, 0x0000},
			{ 0x2C7E, 0x023F, 0x0000},
			{ 0x2C7F, 0x0240, 0x0000},
			{ 0x2C80, 0x2C81, 0x0000},
			{ 0x2C82, 0x2C83, 0x0000},
			{ 0x2C84, 0x2C85, 0x0000},
			{ 0x2C86, 0x2C87, 0x0000},
			{ 0x2C88, 0x2C89, 0x0000},
			{ 0x2C8A, 0x2C8B, 0x0000},
			{ 0x2C8C, 0x2C8D, 0x0000},
			{ 0x2C8E, 0x2C8F, 0x0000},
			{ 0x2C90, 0x2C91, 0x0000},
			{ 0x2C92, 0x2C93, 0x0000},
			{ 0x2C94, 0x2C95, 0x0000},
			{ 0x2C96, 0x2C97, 0x0000},
			{ 0x2C98, 0x2C99, 0x0000},
			{ 0x2C9A, 0x2C9B, 0x0000},
			{ 0x2C9C, 0x2C9D, 0x0000},
			{ 0x2C9E, 0x2C9F, 0x0000},
			{ 0x2CA0, 0x2CA1, 0x0000},
			{ 0x2CA2, 0x2CA3, 0x0000},
			{ 0x2CA4, 0x2CA5, 0x0000},
			{ 0x2CA6, 0x2CA7, 0x0000},
			{ 0x2CA8, 0x2CA9, 0x0000},
			{ 0x2CAA, 0x2CAB, 0x0000},
			{ 0x2CAC, 0x2CAD, 0x0000},
			{ 0x2CAE, 0x2CAF, 0x0000},
			{ 0x2CB0, 0x2CB1, 0x0000},
			{ 0x2CB2, 0x2CB3, 0x0000},
			{ 0x2CB4, 0x2CB5, 0x0000},
			{ 0x2CB6, 0x2CB7, 0x0000},
			{ 0x2CB8, 0x2CB9, 0x0000},
			{ 0x2CBA, 0x2CBB, 0x0000},
			{ 0x2CBC, 0x2CBD, 0x0000},
			{ 0x2CBE, 0x2CBF, 0x0000},
			{ 0x2CC0, 0x2CC1, 0x0000},
			{ 0x2CC2, 0x2CC3, 0x0000},
			{ 0x2CC4, 0x2CC5, 0x0000},
			{ 0x2CC6, 0x2CC7, 0x0000},
			{ 0x2CC8, 0x2CC9, 0x0000},
			{ 0x2CCA, 0x2CCB, 0x0000},
			{ 0x2CCC, 0x2CCD, 0x0000},
			{ 0x2CCE, 0x2CCF, 0x0000},
			{ 0x2CD0, 0x2CD1, 0x0000},
			{ 0x2CD2, 0x2CD3, 0x0000},
			{ 0x2CD4, 0x2CD5, 0x0000},
			{ 0x2CD6, 0x2CD7, 0x0000},
			{ 0x2CD8, 0x2CD9, 0x0000},
			{ 0x2CDA, 0x2CDB, 0x0000},
			{ 0x2CDC, 0x2CDD, 0x0000},
			{ 0x2CDE, 0x2CDF, 0x0000},
			{ 0x2CE0, 0x2CE1, 0x0000},
			{ 0x2CE2, 0x2CE3, 0x0000},
			{ 0x2CEB, 0x2CEC, 0x0000},
			{ 0x2CED, 0x2CEE, 0x0000},
			{ 0x2CF2, 0x2CF3, 0x0000},
			{ 0xA640, 0xA641, 0x0000},
			{ 0xA642, 0xA643, 0x0000},
			{ 0xA644, 0xA645, 0x0000},
			{ 0xA646, 0xA647, 0x0000},
			{ 0xA648, 0xA649, 0x0000},
			{ 0xA64A, 0xA64B, 0x0000},
			{ 0xA64C, 0xA64D, 0x0000},
			{ 0xA64E, 0xA64F, 0x0000},
			{ 0xA650, 0xA651, 0x0000},
			{ 0xA652, 0xA653, 0x0000},
			{ 0xA654, 0xA655, 0x0000},
			{ 0xA656, 0xA657, 0x0000},
			{ 0xA658, 0xA659, 0x0000},
			{ 0xA65A, 0xA65B, 0x0000},
			{ 0xA65C, 0xA65D, 0x0000},
			{ 0xA65E, 0xA65F, 0x0000},
			{ 0xA660, 0xA661, 0x0000},
			{ 0xA662, 0xA663, 0x0000},
			{ 0xA664, 0xA665, 0x0000},
			{ 0xA666, 0xA667, 0x0000},
			{ 0xA668, 0xA669, 0x0000},
			{ 0xA66A, 0xA66B, 0x0000},
			{ 0xA66C, 0xA66D, 0x0000},
			{ 0xA680, 0xA681, 0x0000},
			{ 0xA682, 0xA683, 0x0000},
			{ 0xA684, 0xA685, 0x0000},
			{ 0xA686, 0xA687, 0x0000},
			{ 0xA688, 0xA689, 0x0000},
			{ 0xA68A, 0xA68B, 0x0000},
			{ 0xA68C, 0xA68D, 0x0000},
			{ 0xA68E, 0xA68F, 0x0000},
			{ 0xA690, 0xA691, 0x0000},
			{ 0xA692, 0xA693, 0x0000},
			{ 0xA694, 0xA695, 0x0000},
			{ 0xA696, 0xA697, 0x0000},
			{ 0xA698, 0xA699, 0x0000},
			{ 0xA69A, 0xA69B, 0x0000},
			{ 0xA722, 0xA723, 0x0000},
			{ 0xA724, 0xA725, 0x0000},
			{ 0xA726, 0xA727, 0x0000},
			{ 0xA728, 0xA729, 0x0000},
			{ 0xA72A, 0xA72B, 0x0000},
			{ 0xA72C, 0xA72D, 0x0000},
			{ 0xA72E, 0xA72F, 0x0000},
			{ 0xA732, 0xA733, 0x0000},
			{ 0xA734, 0xA735, 0x0000},
			{ 0xA736, 0xA737, 0x0000},
			{ 0xA738, 0xA739, 0x0000},
			{ 0xA73A, 0xA73B, 0x0000},
			{ 0xA73C, 0xA73D, 0x0000},
			{ 0xA73E, 0xA73F, 0x0000},
			{ 0xA740, 0xA741, 0x0000},
			{ 0xA742, 0xA743, 0x0000},
			{ 0xA744, 0xA745, 0x0000},
			{ 0xA746, 0xA747, 0x0000},
			{ 0xA748, 0xA749, 0x0000},
			{ 0xA74A, 0xA74B, 0x0000},
			{ 0xA74C, 0xA74D, 0x0000},
			{ 0xA74E, 0xA74F, 0x0000},
			{ 0xA750, 0xA751, 0x0000},
			{ 0xA752, 0xA753, 0x0000},
			{ 0xA754, 0xA755, 0x0000},
			{ 0xA756, 0xA757, 0x0000},
			{ 0xA758, 0xA759, 0x0000},
			{ 0xA75A, 0xA75B, 0x0000},
			{ 0xA75C, 0xA75D, 0x0000},
			{ 0xA75E, 0xA75F, 0x0000},
			{ 0xA760, 0xA761, 0x0000},
			{ 0xA762, 0xA763, 0x0000},
			{ 0xA764, 0xA765, 0x0000},
			{ 0xA766, 0xA767, 0x0000},
			{ 0xA768, 0xA769, 0x0000},
			{ 0xA76A, 0xA76B, 0x0000},
			{ 0xA76C, 0xA76D, 0x0000},
			{ 0xA76E, 0xA76F, 0x0000},
			{ 0xA779, 0xA77A, 0x0000},
			{ 0xA77B, 0xA77C, 0x0000},
			{ 0xA77D, 0x1D79, 0x0000},
			{ 0xA77E, 0xA77F, 0x0000},
			{ 0xA780, 0xA781, 0x0000},
			{ 0xA782, 0xA783, 0x0000},
			{ 0xA784, 0xA785, 0x0000},
			{ 0xA786, 0xA787, 0x0000},
			{ 0xA78B, 0xA78C, 0x0000},
			{ 0xA78D, 0x0265, 0x0000},
			{ 0xA790, 0xA791, 0x0000},
			{ 0xA792, 0xA793, 0x0000},
			{ 0xA796, 0xA797, 0x0000},
			{ 0xA798, 0xA799, 0x0000},
			{ 0xA79A, 0xA79B, 0x0000},
			{ 0xA79C, 0xA79D, 0x0000},
			{ 0xA79E, 0xA79F, 0x0000},
			{ 0xA7A0, 0xA7A1, 0x0000},
			{ 0xA7A2, 0xA7A3, 0x0000},
			{ 0xA7A4, 0xA7A5, 0x0000},
			{ 0xA7A6, 0xA7A7, 0x0000},
			{ 0xA7A8, 0xA7A9, 0x0000},
			{ 0xA7AA, 0x0266, 0x0000},
			{ 0xA7AB, 0x025C, 0x0000},
			{ 0xA7AC, 0x0261, 0x0000},
			{ 0xA7AD, 0x026C, 0x0000},
			{ 0xA7AE, 0x026A, 0x0000},
			{ 0xA7B0, 0x029E, 0x0000},
			{ 0xA7B1, 0x0287, 0x0000},
			{ 0xA7B2, 0x029D, 0x0000},
			{ 0xA7B3, 0xAB53, 0x0000},
			{ 0xA7B4, 0xA7B5, 0x0000},
			{ 0xA7B6, 0xA7B7, 0x0000},
			{ 0xAB70, 0x13A0, 0x0000},
			{ 0xAB71, 0x13A1, 0x0000},
			{ 0xAB72, 0x13A2, 0x0000},
			{ 0xAB73, 0x13A3, 0x0000},
			{ 0xAB74, 0x13A4, 0x0000},
			{ 0xAB75, 0x13A5, 0x0000},
			{ 0xAB76, 0x13A6, 0x0000},
			{ 0xAB77, 0x13A7, 0x0000},
			{ 0xAB78, 0x13A8, 0x0000},
			{ 0xAB79, 0x13A9, 0x0000},
			{ 0xAB7A, 0x13AA, 0x0000},
			{ 0xAB7B, 0x13AB, 0x0000},
			{ 0xAB7C, 0x13AC, 0x0000},
			{ 0xAB7D, 0x13AD, 0x0000},
			{ 0xAB7E, 0x13AE, 0x0000},
			{ 0xAB7F, 0x13AF, 0x0000},
			{ 0xAB80, 0x13B0, 0x0000},
			{ 0xAB81, 0x13B1, 0x0000},
			{ 0xAB82, 0x13B2, 0x0000},
			{ 0xAB83, 0x13B3, 0x0000},
			{ 0xAB84, 0x13B4, 0x0000},
			{ 0xAB85, 0x13B5, 0x0000},
			{ 0xAB86, 0x13B6, 0x0000},
			{ 0xAB87, 0x13B7, 0x0000},
			{ 0xAB88, 0x13B8, 0x0000},
			{ 0xAB89, 0x13B9, 0x0000},
			{ 0xAB8A, 0x13BA, 0x0000},
			{ 0xAB8B, 0x13BB, 0x0000},
			{ 0xAB8C, 0x13BC, 0x0000},
			{ 0xAB8D, 0x13BD, 0x0000},
			{ 0xAB8E, 0x13BE, 0x0000},
			{ 0xAB8F, 0x13BF, 0x0000},
			{ 0xAB90, 0x13C0, 0x0000},
			{ 0xAB91, 0x13C1, 0x0000},
			{ 0xAB92, 0x13C2, 0x0000},
			{ 0xAB93, 0x13C3, 0x0000},
			{ 0xAB94, 0x13C4, 0x0000},
			{ 0xAB95, 0x13C5, 0x0000},
			{ 0xAB96, 0x13C6, 0x0000},
			{ 0xAB97, 0x13C7, 0x0000},
			{ 0xAB98, 0x13C8, 0x0000},
			{ 0xAB99, 0x13C9, 0x0000},
			{ 0xAB9A, 0x13CA, 0x0000},
			{ 0xAB9B, 0x13CB, 0x0000},
			{ 0xAB9C, 0x13CC, 0x0000},
			{ 0xAB9D, 0x13CD, 0x0000},
			{ 0xAB9E, 0x13CE, 0x0000},
			{ 0xAB9F, 0x13CF, 0x0000},
			{ 0xABA0, 0x13D0, 0x0000},
			{ 0xABA1, 0x13D1, 0x0000},
			{ 0xABA2, 0x13D2, 0x0000},
			{ 0xABA3, 0x13D3, 0x0000},
			{ 0xABA4, 0x13D4, 0x0000},
			{ 0xABA5, 0x13D5, 0x0000},
			{ 0xABA6, 0x13D6, 0x0000},
			{ 0xABA7, 0x13D7, 0x0000},
			{ 0xABA8, 0x13D8, 0x0000},
			{ 0xABA9, 0x13D9, 0x0000},
			{ 0xABAA, 0x13DA, 0x0000},
			{ 0xABAB, 0x13DB, 0x0000},
			{ 0xABAC, 0x13DC, 0x0000},
			{ 0xABAD, 0x13DD, 0x0000},
			{ 0xABAE, 0x13DE, 0x0000},
			{ 0xABAF, 0x13DF, 0x0000},
			{ 0xABB0, 0x13E0, 0x0000},
			{ 0xABB1, 0x13E1, 0x0000},
			{ 0xABB2, 0x13E2, 0x0000},
			{ 0xABB3, 0x13E3, 0x0000},
			{ 0xABB4, 0x13E4, 0x0000},
			{ 0xABB5, 0x13E5, 0x0000},
			{ 0xABB6, 0x13E6, 0x0000},
			{ 0xABB7, 0x13E7, 0x0000},
			{ 0xABB8, 0x13E8, 0x0000},
			{ 0xABB9, 0x13E9, 0x0000},
			{ 0xABBA, 0x13EA, 0x0000},
			{ 0xABBB, 0x13EB, 0x0000},
			{ 0xABBC, 0x13EC, 0x0000},
			{ 0xABBD, 0x13ED, 0x0000},
			{ 0xABBE, 0x13EE, 0x0000},
			{ 0xABBF, 0x13EF, 0x0000},
			{ 0xFB00, 0x0066, 0x0066},
			{ 0xFB01, 0x0066, 0x0069},
			{ 0xFB02, 0x0066, 0x006C},
			{ 0xFB03, 0x0066, 0x0066},
			{ 0xFB04, 0x0066, 0x0066},
			{ 0xFB05, 0x0073, 0x0074},
			{ 0xFB06, 0x0073, 0x0074},
			{ 0xFB13, 0x0574, 0x0576},
			{ 0xFB14, 0x0574, 0x0565},
			{ 0xFB15, 0x0574, 0x056B},
			{ 0xFB16, 0x057E, 0x0576},
			{ 0xFB17, 0x0574, 0x056D},
			{ 0xFF21, 0xFF41, 0x0000},
			{ 0xFF22, 0xFF42, 0x0000},
			{ 0xFF23, 0xFF43, 0x0000},
			{ 0xFF24, 0xFF44, 0x0000},
			{ 0xFF25, 0xFF45, 0x0000},
			{ 0xFF26, 0xFF46, 0x0000},
			{ 0xFF27, 0xFF47, 0x0000},
			{ 0xFF28, 0xFF48, 0x0000},
			{ 0xFF29, 0xFF49, 0x0000},
			{ 0xFF2A, 0xFF4A, 0x0000},
			{ 0xFF2B, 0xFF4B, 0x0000},
			{ 0xFF2C, 0xFF4C, 0x0000},
			{ 0xFF2D, 0xFF4D, 0x0000},
			{ 0xFF2E, 0xFF4E, 0x0000},
			{ 0xFF2F, 0xFF4F, 0x0000},
			{ 0xFF30, 0xFF50, 0x0000},
			{ 0xFF31, 0xFF51, 0x0000},
			{ 0xFF32, 0xFF52, 0x0000},
			{ 0xFF33, 0xFF53, 0x0000},
			{ 0xFF34, 0xFF54, 0x0000},
			{ 0xFF35, 0xFF55, 0x0000},
			{ 0xFF36, 0xFF56, 0x0000},
			{ 0xFF37, 0xFF57, 0x0000},
			{ 0xFF38, 0xFF58, 0x0000},
			{ 0xFF39, 0xFF59, 0x0000},
			{ 0xFF3A, 0xFF5A, 0x0000},
			{ 0x10400, 0x10428, 0x0000},
			{ 0x10401, 0x10429, 0x0000},
			{ 0x10402, 0x1042A, 0x0000},
			{ 0x10403, 0x1042B, 0x0000},
			{ 0x10404, 0x1042C, 0x0000},
			{ 0x10405, 0x1042D, 0x0000},
			{ 0x10406, 0x1042E, 0x0000},
			{ 0x10407, 0x1042F, 0x0000},
			{ 0x10408, 0x10430, 0x0000},
			{ 0x10409, 0x10431, 0x0000},
			{ 0x1040A, 0x10432, 0x0000},
			{ 0x1040B, 0x10433, 0x0000},
			{ 0x1040C, 0x10434, 0x0000},
			{ 0x1040D, 0x10435, 0x0000},
			{ 0x1040E, 0x10436, 0x0000},
			{ 0x1040F, 0x10437, 0x0000},
			{ 0x10410, 0x10438, 0x0000},
			{ 0x10411, 0x10439, 0x0000},
			{ 0x10412, 0x1043A, 0x0000},
			{ 0x10413, 0x1043B, 0x0000},
			{ 0x10414, 0x1043C, 0x0000},
			{ 0x10415, 0x1043D, 0x0000},
			{ 0x10416, 0x1043E, 0x0000},
			{ 0x10417, 0x1043F, 0x0000},
			{ 0x10418, 0x10440, 0x0000},
			{ 0x10419, 0x10441, 0x0000},
			{ 0x1041A, 0x10442, 0x0000},
			{ 0x1041B, 0x10443, 0x0000},
			{ 0x1041C, 0x10444, 0x0000},
			{ 0x1041D, 0x10445, 0x0000},
			{ 0x1041E, 0x10446, 0x0000},
			{ 0x1041F, 0x10447, 0x0000},
			{ 0x10420, 0x10448, 0x0000},
			{ 0x10421, 0x10449, 0x0000},
			{ 0x10422, 0x1044A, 0x0000},
			{ 0x10423, 0x1044B, 0x0000},
			{ 0x10424, 0x1044C, 0x0000},
			{ 0x10425, 0x1044D, 0x0000},
			{ 0x10426, 0x1044E, 0x0000},
			{ 0x10427, 0x1044F, 0x0000},
			{ 0x104B0, 0x104D8, 0x0000},
			{ 0x104B1, 0x104D9, 0x0000},
			{ 0x104B2, 0x104DA, 0x0000},
			{ 0x104B3, 0x104DB, 0x0000},
			{ 0x104B4, 0x104DC, 0x0000},
			{ 0x104B5, 0x104DD, 0x0000},
			{ 0x104B6, 0x104DE, 0x0000},
			{ 0x104B7, 0x104DF, 0x0000},
			{ 0x104B8, 0x104E0, 0x0000},
			{ 0x104B9, 0x104E1, 0x0000},
			{ 0x104BA, 0x104E2, 0x0000},
			{ 0x104BB, 0x104E3, 0x0000},
			{ 0x104BC, 0x104E4, 0x0000},
			{ 0x104BD, 0x104E5, 0x0000},
			{ 0x104BE, 0x104E6, 0x0000},
			{ 0x104BF, 0x104E7, 0x0000},
			{ 0x104C0, 0x104E8, 0x0000},
			{ 0x104C1, 0x104E9, 0x0000},
			{ 0x104C2, 0x104EA, 0x0000},
			{ 0x104C3, 0x104EB, 0x0000},
			{ 0x104C4, 0x104EC, 0x0000},
			{ 0x104C5, 0x104ED, 0x0000},
			{ 0x104C6, 0x104EE, 0x0000},
			{ 0x104C7, 0x104EF, 0x0000},
			{ 0x104C8, 0x104F0, 0x0000},
			{ 0x104C9, 0x104F1, 0x0000},
			{ 0x104CA, 0x104F2, 0x0000},
			{ 0x104CB, 0x104F3, 0x0000},
			{ 0x104CC, 0x104F4, 0x0000},
			{ 0x104CD, 0x104F5, 0x0000},
			{ 0x104CE, 0x104F6, 0x0000},
			{ 0x104CF, 0x104F7, 0x0000},
			{ 0x104D0, 0x104F8, 0x0000},
			{ 0x104D1, 0x104F9, 0x0000},
			{ 0x104D2, 0x104FA, 0x0000},
			{ 0x104D3, 0x104FB, 0x0000},
			{ 0x10C80, 0x10CC0, 0x0000},
			{ 0x10C81, 0x10CC1, 0x0000},
			{ 0x10C82, 0x10CC2, 0x0000},
			{ 0x10C83, 0x10CC3, 0x0000},
			{ 0x10C84, 0x10CC4, 0x0000},
			{ 0x10C85, 0x10CC5, 0x0000},
			{ 0x10C86, 0x10CC6, 0x0000},
			{ 0x10C87, 0x10CC7, 0x0000},
			{ 0x10C88, 0x10CC8, 0x0000},
			{ 0x10C89, 0x10CC9, 0x0000},
			{ 0x10C8A, 0x10CCA, 0x0000},
			{ 0x10C8B, 0x10CCB, 0x0000},
			{ 0x10C8C, 0x10CCC, 0x0000},
			{ 0x10C8D, 0x10CCD, 0x0000},
			{ 0x10C8E, 0x10CCE, 0x0000},
			{ 0x10C8F, 0x10CCF, 0x0000},
			{ 0x10C90, 0x10CD0, 0x0000},
			{ 0x10C91, 0x10CD1, 0x0000},
			{ 0x10C92, 0x10CD2, 0x0000},
			{ 0x10C93, 0x10CD3, 0x0000},
			{ 0x10C94, 0x10CD4, 0x0000},
			{ 0x10C95, 0x10CD5, 0x0000},
			{ 0x10C96, 0x10CD6, 0x0000},
			{ 0x10C97, 0x10CD7, 0x0000},
			{ 0x10C98, 0x10CD8, 0x0000},
			{ 0x10C99, 0x10CD9, 0x0000},
			{ 0x10C9A, 0x10CDA, 0x0000},
			{ 0x10C9B, 0x10CDB, 0x0000},
			{ 0x10C9C, 0x10CDC, 0x0000},
			{ 0x10C9D, 0x10CDD, 0x0000},
			{ 0x10C9E, 0x10CDE, 0x0000},
			{ 0x10C9F, 0x10CDF, 0x0000},
			{ 0x10CA0, 0x10CE0, 0x0000},
			{ 0x10CA1, 0x10CE1, 0x0000},
			{ 0x10CA2, 0x10CE2, 0x0000},
			{ 0x10CA3, 0x10CE3, 0x0000},
			{ 0x10CA4, 0x10CE4, 0x0000},
			{ 0x10CA5, 0x10CE5, 0x0000},
			{ 0x10CA6, 0x10CE6, 0x0000},
			{ 0x10CA7, 0x10CE7, 0x0000},
			{ 0x10CA8, 0x10CE8, 0x0000},
			{ 0x10CA9, 0x10CE9, 0x0000},
			{ 0x10CAA, 0x10CEA, 0x0000},
			{ 0x10CAB, 0x10CEB, 0x0000},
			{ 0x10CAC, 0x10CEC, 0x0000},
			{ 0x10CAD, 0x10CED, 0x0000},
			{ 0x10CAE, 0x10CEE, 0x0000},
			{ 0x10CAF, 0x10CEF, 0x0000},
			{ 0x10CB0, 0x10CF0, 0x0000},
			{ 0x10CB1, 0x10CF1, 0x0000},
			{ 0x10CB2, 0x10CF2, 0x0000},
			{ 0x118A0, 0x118C0, 0x0000},
			{ 0x118A1, 0x118C1, 0x0000},
			{ 0x118A2, 0x118C2, 0x0000},
			{ 0x118A3, 0x118C3, 0x0000},
			{ 0x118A4, 0x118C4, 0x0000},
			{ 0x118A5, 0x118C5, 0x0000},
			{ 0x118A6, 0x118C6, 0x0000},
			{ 0x118A7, 0x118C7, 0x0000},
			{ 0x118A8, 0x118C8, 0x0000},
			{ 0x118A9, 0x118C9, 0x0000},
			{ 0x118AA, 0x118CA, 0x0000},
			{ 0x118AB, 0x118CB, 0x0000},
			{ 0x118AC, 0x118CC, 0x0000},
			{ 0x118AD, 0x118CD, 0x0000},
			{ 0x118AE, 0x118CE, 0x0000},
			{ 0x118AF, 0x118CF, 0x0000},
			{ 0x118B0, 0x118D0, 0x0000},
			{ 0x118B1, 0x118D1, 0x0000},
			{ 0x118B2, 0x118D2, 0x0000},
			{ 0x118B3, 0x118D3, 0x0000},
			{ 0x118B4, 0x118D4, 0x0000},
			{ 0x118B5, 0x118D5, 0x0000},
			{ 0x118B6, 0x118D6, 0x0000},
			{ 0x118B7, 0x118D7, 0x0000},
			{ 0x118B8, 0x118D8, 0x0000},
			{ 0x118B9, 0x118D9, 0x0000},
			{ 0x118BA, 0x118DA, 0x0000},
			{ 0x118BB, 0x118DB, 0x0000},
			{ 0x118BC, 0x118DC, 0x0000},
			{ 0x118BD, 0x118DD, 0x0000},
			{ 0x118BE, 0x118DE, 0x0000},
			{ 0x118BF, 0x118DF, 0x0000},
			{ 0x1E900, 0x1E922, 0x0000},
			{ 0x1E901, 0x1E923, 0x0000},
			{ 0x1E902, 0x1E924, 0x0000},
			{ 0x1E903, 0x1E925, 0x0000},
			{ 0x1E904, 0x1E926, 0x0000},
			{ 0x1E905, 0x1E927, 0x0000},
			{ 0x1E906, 0x1E928, 0x0000},
			{ 0x1E907, 0x1E929, 0x0000},
			{ 0x1E908, 0x1E92A, 0x0000},
			{ 0x1E909, 0x1E92B, 0x0000},
			{ 0x1E90A, 0x1E92C, 0x0000},
			{ 0x1E90B, 0x1E92D, 0x0000},
			{ 0x1E90C, 0x1E92E, 0x0000},
			{ 0x1E90D, 0x1E92F, 0x0000},
			{ 0x1E90E, 0x1E930, 0x0000},
			{ 0x1E90F, 0x1E931, 0x0000},
			{ 0x1E910, 0x1E932, 0x0000},
			{ 0x1E911, 0x1E933, 0x0000},
			{ 0x1E912, 0x1E934, 0x0000},
			{ 0x1E913, 0x1E935, 0x0000},
			{ 0x1E914, 0x1E936, 0x0000},
			{ 0x1E915, 0x1E937, 0x0000},
			{ 0x1E916, 0x1E938, 0x0000},
			{ 0x1E917, 0x1E939, 0x0000},
			{ 0x1E918, 0x1E93A, 0x0000},
			{ 0x1E919, 0x1E93B, 0x0000},
			{ 0x1E91A, 0x1E93C, 0x0000},
			{ 0x1E91B, 0x1E93D, 0x0000},
			{ 0x1E91C, 0x1E93E, 0x0000},
			{ 0x1E91D, 0x1E93F, 0x0000},
			{ 0x1E91E, 0x1E940, 0x0000},
			{ 0x1E91F, 0x1E941, 0x0000},
			{ 0x1E920, 0x1E942, 0x0000},
			{ 0x1E921, 0x1E943, 0x0000},

		};
	};

	inline bool FindCaseFoldingData(char32_t sourceCodePoint, char32_t * destCodePoint1, char32_t * destCodePoint2)
	{
		int32_t lower = 0;
		int32_t upper = static_cast<int32_t>(countof(Impl::FoldTable::caseFoldingTable)) - 1;
		while (lower <= upper)
		{
			int32_t split = (lower + upper) / 2;
			char32_t currPoint = Impl::FoldTable::caseFoldingTable[split].sourceCodePoint;
			if (currPoint < sourceCodePoint)
				lower = split + 1;
			else if (currPoint > sourceCodePoint)
				upper = split - 1;
			else
			{
				const auto & result = Impl::FoldTable::caseFoldingTable[split];
				if (destCodePoint1)
					*destCodePoint1 = result.destCodePoint1;
				if (destCodePoint2)
					*destCodePoint2 = result.destCodePoint2;
				return true;
			}
		}
		return false;
	}

} // namespace Jinx::Impl



// end --- JxUnicodeCaseFolding.cpp --- 



// begin --- JxVariableStackFrame.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

namespace Jinx::Impl
{

	inline VariableStackFrame::VariableStackFrame()
	{
		m_frames.push_back(FrameData());
		m_frames.back().stack.push_back(VariableSet());
	}

	inline void VariableStackFrame::CalculateMaxVariableParts()
	{
		size_t maxVarParts = 0;

		// Retrieve the current frame data
		FrameData & frame = m_frames.back();

		// Check the entire variable stack
		for (auto itr = frame.stack.begin(); itr != frame.stack.end(); ++itr)
		{
			// Check each variable within each set
			for (auto itr2 = itr->begin(); itr2 != itr->end(); ++itr2)
			{
				size_t varParts = GetNamePartCount(*itr2);
				if (varParts > maxVarParts)
					maxVarParts = varParts;
			}
		}

		// Set current max var parts
		SetMaxVariableParts(maxVarParts);
	}

	inline size_t VariableStackFrame::GetMaxVariableParts() const
	{
		// Retrieve the current frame data and return max variable parts
		const FrameData & frame = m_frames.back();
		return frame.maxVariableParts;
	}

	inline void VariableStackFrame::SetMaxVariableParts(size_t varParts)
	{
		// Retrieve the current frame data and set max variable parts
		FrameData & frame = m_frames.back();
		frame.maxVariableParts = varParts;
	}

	inline size_t VariableStackFrame::GetStackDepthFromName(const String & name) const
	{
		// Sanity check
		if (m_frames.empty())
			return 0;

		// Find the variable in the current frame
		const FrameData & frame = m_frames.back();
		size_t stackDepth = frame.stack.size();
		for (auto ritr = frame.stack.rbegin(); ritr != frame.stack.rend(); ++ritr)
		{
			auto itr = ritr->find(name);
			if (itr != ritr->end())
				return stackDepth;
			--stackDepth;
		}
		return 0;
	}

	inline bool VariableStackFrame::VariableAssign(const String & name)
	{
		// Sanity check
		if (m_frames.empty())
		{
			m_errorMessage = "Unexpected empty variable frame";
			return false;
		}

		// Retrieve the current frame data
		FrameData & frame = m_frames.back();
		if (frame.stack.empty())
		{
			m_errorMessage = "Attempting to assign a variable to an empty stack";
			return false;
		}

		// Work from the top of the stack down, and attempt to find an existing name
		for (auto ritr = frame.stack.rbegin(); ritr != frame.stack.rend(); ++ritr)
		{
			const auto & itr = ritr->find(name);
			if (itr != ritr->end())
				return true;
		}

		// If we don't find the name, create a new variable on the top of the stack
		auto & variableSet = frame.stack.back();
		variableSet.insert(name);

		// Adjust the max variable parts value if necessary
		size_t varParts = GetNamePartCount(name);
		if (varParts > GetMaxVariableParts())
			SetMaxVariableParts(varParts);

		return true;
	}

	inline bool VariableStackFrame::VariableExists(const String & name) const
	{
		// Sanity check
		if (m_frames.empty())
			return false;

		// Find the variable in the current frame
		const FrameData & frame = m_frames.back();
		for (auto ritr = frame.stack.rbegin(); ritr != frame.stack.rend(); ++ritr)
		{
			auto itr = ritr->find(name);
			if (itr != ritr->end())
				return true;
		}
		return false;
	}

	inline void VariableStackFrame::FrameBegin()
	{
		m_frames.push_back(FrameData());
		m_frames.back().stack.push_back(VariableSet());
	}

	inline bool VariableStackFrame::FrameEnd()
	{
		if (m_frames.empty())
		{
			m_errorMessage = "Attempted to pop empty variable frame";
			return false;
		}
		m_frames.pop_back();
		return true;
	}

	inline bool VariableStackFrame::ScopeBegin()
	{
		if (m_frames.empty())
		{
			m_errorMessage = "Unexpected empty variable frame";
			return false;
		}
		FrameData & frame = m_frames.back();
		frame.stack.push_back(VariableSet());
		return true;
	}

	inline bool VariableStackFrame::ScopeEnd()
	{
		if (m_frames.empty())
		{
			m_errorMessage = "Unexpected empty variable frame";
			return false;
		}
		FrameData & frame = m_frames.back();
		if (frame.stack.empty())
		{
			m_errorMessage = "Attempted to pop empty variable stack";
			return false;
		}
		frame.stack.pop_back();
		CalculateMaxVariableParts();
		return true;
	}

	inline bool VariableStackFrame::IsRootScope() const
	{
		return (m_frames.back().stack.size() == 1) ? true : false;
	}

	inline bool VariableStackFrame::IsRootFrame() const
	{
		return (m_frames.size() == 1) ? true : false;
	}

} // namespace Jinx::Impl



// end --- JxVariableStackFrame.cpp --- 



// begin --- JxVariant.cpp --- 

/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

namespace Jinx
{

	namespace Impl
	{
		struct VarCmpTable
		{
			static inline bool allowValueCompare[] =
			{
				false,	// Null
				true,	// Number
				true,	// Integer
				false,	// Boolean
				true,	// String
				false,	// Collection
				false,	// CollectionItr
				false,	// UserObject
				false,	// Buffer
				true,	// Guid
				false,	// ValType
			};

			static_assert(countof(allowValueCompare) == static_cast<size_t>(ValueType::NumValueTypes), "Value compare flags don't match enum count");

		};

		inline bool CheckValueTypeForCompare(ValueType type)
		{
			return VarCmpTable::allowValueCompare[static_cast<size_t>(type)];
		}

	} // namespace Impl

	inline Variant::Variant(const Variant & copy)
	{
		m_type = copy.m_type;
		switch (m_type)
		{
			case ValueType::Null:
				break;
			case ValueType::Number:
				m_number = copy.m_number;
				break;
			case ValueType::Integer:
				m_integer = copy.m_integer;
				break;
			case ValueType::Boolean:
				m_boolean = copy.m_boolean;
				break;
			case ValueType::String:
				new(&m_string) String();
				m_string = copy.m_string;
				break;
			case ValueType::Collection:
				new(&m_collection) CollectionPtr();
				m_collection = copy.m_collection;
				break;
			case ValueType::CollectionItr:
				new(&m_collectionItrPair) CollectionItrPair();
				m_collectionItrPair = copy.m_collectionItrPair;
				break;
			case ValueType::UserObject:
				new(&m_userObject) UserObjectPtr();
				m_userObject = copy.m_userObject;
				break;
			case ValueType::Buffer:
				new(&m_buffer) BufferPtr();
				m_buffer = copy.m_buffer;
				break;
			case ValueType::Guid:
				m_guid = copy.m_guid;
				break;
			case ValueType::ValType:
				m_valType = copy.m_valType;
				break;
			default:
				assert(!"Unknown variant type!");
		};
	}

	inline Variant::~Variant()
	{
		Destroy();
	}

	inline Variant & Variant::operator= (const Variant & copy)
	{
		Destroy();
		m_type = copy.m_type;
		switch (m_type)
		{
			case ValueType::Null:
				break;
			case ValueType::Number:
				m_number = copy.m_number;
				break;
			case ValueType::Integer:
				m_integer = copy.m_integer;
				break;
			case ValueType::Boolean:
				m_boolean = copy.m_boolean;
				break;
			case ValueType::String:
				new(&m_string) String();
				m_string = copy.m_string;
				break;
			case ValueType::Collection:
				new(&m_collection) CollectionPtr();
				m_collection = copy.m_collection;
				break;
			case ValueType::CollectionItr:
				new(&m_collectionItrPair) CollectionItrPair();
				m_collectionItrPair = copy.m_collectionItrPair;
				break;
			case ValueType::UserObject:
				new(&m_userObject) UserObjectPtr();
				m_userObject = copy.m_userObject;
				break;
			case ValueType::Buffer:
				new(&m_buffer) BufferPtr();
				m_buffer = copy.m_buffer;
				break;
			case ValueType::Guid:
				m_guid = copy.m_guid;
				break;
			case ValueType::ValType:
				m_valType = copy.m_valType;
				break;
			default:
				assert(!"Unknown variant type!");
		};
		return *this;
	}

	inline Variant & Variant::operator ++()
	{
		switch (m_type)
		{
			case ValueType::Number:
				++m_number;
				break;
			case ValueType::Integer:
				++m_integer;
				break;
			case ValueType::CollectionItr:
				++m_collectionItrPair.first;
				break;
			default:
				break;
		};
		return *this;
	}

	inline Variant & Variant::operator += (const Variant & right)
	{
		switch (m_type)
		{
			case ValueType::Number:
				m_number += right.GetNumber();
				break;
			case ValueType::Integer:
				m_integer += right.GetInteger();
				break;
			case ValueType::String:
				m_string += right.GetString();
				break;
			default:
				break;
		}
		return *this;
	}

	inline Variant & Variant::operator --()
	{
		switch (m_type)
		{
			case ValueType::Number:
				--m_number;
				break;
			case ValueType::Integer:
				--m_integer;
				break;
			default:
				break;
		};
		return *this;
	}

	inline Variant & Variant::operator -= (const Variant & right)
	{
		switch (m_type)
		{
			case ValueType::Number:
				m_number -= right.GetNumber();
				break;
			case ValueType::Integer:
				m_integer -= right.GetInteger();
				break;
			default:
				break;
		}
		return *this;
	}

	inline bool Variant::CanConvertTo(ValueType type) const
	{
		if (m_type == type || type == ValueType::Null)
			return true;
		Variant v = *this;
		return v.ConvertTo(type);
	}

	inline bool Variant::ConvertTo(ValueType type)
	{
		if (m_type == type)
			return true;
		if (type == ValueType::Null)
		{
			SetNull();
			return true;
		}

		switch (m_type)
		{
			case ValueType::Null:
			{
				switch (type)
				{
					case ValueType::Number:
						SetNumber(Impl::NullToNumber());
						return true;
					case ValueType::Integer:
						SetInteger(Impl::NullToInteger());
						return true;
					case ValueType::Boolean:
						SetBoolean(Impl::NullToBoolean());
						return true;
					case ValueType::String:
						SetString(Impl::NullToString());
						return true;
					default:
						break;
				};
			}
			break;
			case ValueType::Number:
			{
				switch (type)
				{
					case ValueType::Integer:
						SetInteger(Impl::NumberToInteger(m_number));
						return true;
					case ValueType::Boolean:
						SetBoolean(Impl::NumberToBoolean(m_number));
						return true;
					case ValueType::String:
						SetString(Impl::NumberToString(m_number));
						return true;
					default:
						break;
				};
			}
			break;
			case ValueType::Integer:
			{
				switch (type)
				{
					case ValueType::Number:
						SetNumber(Impl::IntegerToNumber(m_integer));
						return true;
					case ValueType::Boolean:
						SetBoolean(Impl::IntegerToBoolean(m_integer));
						return true;
					case ValueType::String:
						SetString(Impl::IntegerToString(m_integer));
						return true;
					default:
						break;
				};
			}
			break;
			case ValueType::Boolean:
			{
				switch (type)
				{
					case ValueType::Number:
						SetNumber(Impl::BooleanToNumber(m_boolean));
						return true;
					case ValueType::Integer:
						SetInteger(Impl::BooleanToInteger(m_boolean));
						return true;
					case ValueType::String:
						SetString(Impl::BooleanToString(m_boolean));
						return true;
					default:
						break;
				};
			}
			break;
			case ValueType::String:
			{
				switch (type)
				{
					case ValueType::Number:
					{
						double number;
						if (!Impl::StringToNumber(m_string, &number))
						{
							Impl::LogWriteLine(LogLevel::Error, "Error converting string %s to number", m_string.c_str());
							SetNull();
							return false;
						}
						SetNumber(number);
						return true;
					}
					case ValueType::Integer:
					{
						int64_t integer;
						if (!Impl::StringToInteger(m_string, &integer))
						{
							Impl::LogWriteLine(LogLevel::Error, "Error converting string %s to integer", m_string.c_str());
							SetNull();
							return false;
						}
						SetInteger(integer);
						return true;
					}
					case ValueType::Boolean:
					{
						bool boolean;
						if (!Impl::StringToBoolean(m_string, &boolean))
						{
							Impl::LogWriteLine(LogLevel::Error, "Error converting string %s to boolean", m_string.c_str());
							SetNull();
							return false;
						}
						SetBoolean(boolean);
						return true;
					}
					case ValueType::Collection:
					{
						auto collection = CreateCollection();
						if (!Impl::StringToCollection(m_string, &collection))
						{
							Impl::LogWriteLine(LogLevel::Error, "Error converting string to collection type", m_string.c_str());
							SetNull();
							return false;
						}
						SetCollection(collection);
						return true;
					}
					case ValueType::Guid:
					{
						Guid guid;
						if (!Impl::StringToGuid(m_string, &guid))
						{
							Impl::LogWriteLine(LogLevel::Error, "Error converting string %s to Guid", m_string.c_str());
							SetNull();
							return false;
						}
						SetGuid(guid);
						return true;
					}
					case ValueType::ValType:
					{
						ValueType valType;
						if (!Impl::StringToValueType(m_string, &valType))
						{
							Impl::LogWriteLine(LogLevel::Error, "Error converting string %s to value type", m_string.c_str());
							SetNull();
							return false;
						}
						SetValType(valType);
						return true;
					}
					default:
						break;
				};
			}
			break;
			case ValueType::Collection:
				switch (type)
				{
					case ValueType::Boolean:
						SetBoolean(!m_collection->empty());
						return true;
					default:
						break;
				};
				break;
			case ValueType::Guid:
				switch (type)
				{
					case ValueType::String:
						SetString(Impl::GuidToString(m_guid));
						return true;
					default:
						break;
				};
				break;
			case ValueType::ValType:
				switch (type)
				{
					case ValueType::String:
						SetString(Impl::GetValueTypeName(m_valType));
						return true;
					default:
						break;
				};
				break;
			default:
				break;
		};
		Impl::LogWriteLine(LogLevel::Error, "Error converting %s to %s", Impl::GetValueTypeName(m_type), Impl::GetValueTypeName(type));
		SetNull();
		return false;
	}

	inline void Variant::Destroy()
	{
		// Optimize for common case
		if (m_type == ValueType::Null)
			return;

		// Explicitly call destructors for object types
		switch (m_type)
		{
			case ValueType::String:
				m_string.~String();
				break;
			case ValueType::Collection:
				m_collection.~CollectionPtr();
				break;
			case ValueType::CollectionItr:
				m_collectionItrPair.~CollectionItrPair();
				break;
			case ValueType::UserObject:
				m_userObject.~UserObjectPtr();
				break;
			case ValueType::Buffer:
				m_buffer.~BufferPtr();
				break;
			default:
				break;
		};

		m_type = ValueType::Null;
	}

	inline bool Variant::GetBoolean() const
	{
		if (IsBoolean())
			return m_boolean;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::Boolean))
			return false;
		return v.GetBoolean();
	}

	inline CollectionPtr Variant::GetCollection() const
	{
		if (IsCollection())
			return m_collection;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::Collection))
			return nullptr;
		return v.GetCollection();
	}

	inline CollectionItrPair Variant::GetCollectionItr() const
	{
		if (IsCollectionItr())
			return m_collectionItrPair;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::CollectionItr))
			return CollectionItrPair();
		return v.GetCollectionItr();
	}

	inline UserObjectPtr Variant::GetUserObject() const
	{
		if (IsUserObject())
			return m_userObject;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::UserObject))
			return nullptr;
		return v.GetUserObject();
	}

	inline BufferPtr Variant::GetBuffer() const
	{
		if (IsBuffer())
			return m_buffer;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::Buffer))
			return nullptr;
		return v.GetBuffer();
	}

	inline Guid Variant::GetGuid() const
	{
		if (IsGuid())
			return m_guid;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::Guid))
			return NullGuid;
		return v.GetGuid();
	}

	inline int64_t Variant::GetInteger() const
	{
		if (IsInteger())
			return m_integer;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::Integer))
			return 0;
		return v.GetInteger();
	}

	inline double Variant::GetNumber() const
	{
		if (IsNumber())
			return m_number;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::Number))
			return 0.0;
		return v.GetNumber();
	}

	inline String Variant::GetString() const
	{
		if (IsString())
			return m_string;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::String))
			return String();
		return v.GetString();
	}

	inline StringU16 Variant::GetStringU16() const
	{
		return Impl::ConvertUtf8ToUtf16(GetString());
	}

	inline WString Variant::GetWString() const
	{
		return Impl::ConvertUtf8ToWString(GetString());
	}

	inline ValueType Variant::GetValType() const
	{
		if (IsValType())
			return m_valType;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::ValType))
			return ValueType::Null;
		return v.GetValType();
	}

	inline bool Variant::IsKeyType() const
	{
		switch (m_type)
		{
			case ValueType::Number:
			case ValueType::Integer:
			case ValueType::Boolean:
			case ValueType::String:
			case ValueType::Guid:
				return true;
			default:
				break;
		};
		return false;
	}

	inline bool Variant::IsNumericType() const
	{
		if (m_type == ValueType::Integer || m_type == ValueType::Number)
			return true;
		return false;
	}

	inline void Variant::SetBuffer(const BufferPtr & value)
	{
		Destroy();
		if (!value)
		{
			m_type = ValueType::Null;
			m_integer = 0;
		}
		m_type = ValueType::Buffer;
		new(&m_buffer) BufferPtr();
		m_buffer = value;
	}

	inline void Variant::SetBoolean(bool value)
	{
		Destroy();
		m_type = ValueType::Boolean;
		m_boolean = value;
	}

	inline void Variant::SetCollection(const CollectionPtr & value)
	{
		Destroy();
		if (!value)
		{
			m_type = ValueType::Null;
			m_integer = 0;
		}
		m_type = ValueType::Collection;
		new(&m_collection) CollectionPtr();
		m_collection = value;
	}

	inline void Variant::SetCollectionItr(const CollectionItrPair & value)
	{
		assert(value.second);
		Destroy();
		m_type = ValueType::CollectionItr;
		new(&m_collectionItrPair) CollectionItrPair();
		m_collectionItrPair = value;
	}

	inline void Variant::SetGuid(const Guid & value)
	{
		Destroy();
		m_type = ValueType::Guid;
		m_guid = value;
	}

	inline void Variant::SetInteger(int64_t value)
	{
		Destroy();
		m_type = ValueType::Integer;
		m_integer = value;
	}

	inline void Variant::SetNull()
	{
		Destroy();
		m_type = ValueType::Null;
	}

	inline void Variant::SetNumber(double value)
	{
		Destroy();
		m_type = ValueType::Number;
		m_number = value;
	}

	inline void Variant::SetUserObject(const UserObjectPtr & value)
	{
		Destroy();
		m_type = ValueType::UserObject;
		new(&m_userObject) UserObjectPtr();
		m_userObject = value;
	}

	inline void Variant::SetString(const String & value)
	{
		Destroy();
		m_type = ValueType::String;
		new(&m_string) String();
		m_string = value;
	}

	inline void Variant::SetString(const StringU16 & value)
	{
		SetString(Impl::ConvertUtf16ToUtf8(value));
	}

	inline void Variant::SetString(const WString & value)
	{
		SetString(Impl::ConvertWStringToUtf8(value));
	}

	inline void Variant::SetValType(ValueType value)
	{
		Destroy();
		m_type = ValueType::ValType;
		m_valType = value;
	}

	inline void Variant::Write(BinaryWriter & writer) const
	{

		// Write out the type
		uint8_t t = Impl::ValueTypeToByte(m_type);
		writer.Write(t);

		switch (m_type)
		{
			case ValueType::Null:
				break;
			case ValueType::Number:
				writer.Write(m_number);
				break;
			case ValueType::Integer:
				writer.Write(m_integer);
				break;
			case ValueType::Boolean:
				writer.Write(m_boolean);
				break;
			case ValueType::String:
				writer.Write(m_string);
				break;
			case ValueType::Collection:
				break;
			case ValueType::CollectionItr:
				break;
			case ValueType::UserObject:
				break;
			case ValueType::Buffer:
				writer.Write(m_buffer);
				break;
			case ValueType::Guid:
				writer.Write(&m_guid, sizeof(m_guid));
				break;
			case ValueType::ValType:
				writer.Write(Impl::ValueTypeToByte(m_valType));
				break;
			default:
				assert(!"Unknown variant type!");
		};

	}

	inline void Variant::Read(BinaryReader & reader)
	{
		Destroy();
		uint8_t t;
		reader.Read(&t);
		m_type = Impl::ByteToValueType(t);

		switch (m_type)
		{
			case ValueType::Null:
				break;
			case ValueType::Number:
				reader.Read(&m_number);
				break;
			case ValueType::Integer:
				reader.Read(&m_integer);
				break;
			case ValueType::Boolean:
				reader.Read(&m_boolean);
				break;
			case ValueType::String:
				new(&m_string) String();
				reader.Read(&m_string);
				break;
			case ValueType::Collection:
				break;
			case ValueType::CollectionItr:
				break;
			case ValueType::UserObject:
				break;
			case ValueType::Buffer:
				new(&m_buffer) BufferPtr();
				reader.Read(m_buffer);
				break;
			case ValueType::Guid:
				reader.Read(&m_guid, sizeof(m_guid));
				break;
			case ValueType::ValType:
			{
				uint8_t vt;
				reader.Read(&vt);
				m_valType = Impl::ByteToValueType(vt);
				break;
			}
			default:
				assert(!"Unknown variant type!");
		};

	}

	inline Variant operator + (const Variant & left, const Variant & right)
	{
		if (left.GetType() == ValueType::String)
		{
			Variant result;
			result.SetString(left.GetString() + right.GetString());
			return result;
		}
		if (left.GetType() != ValueType::Number && left.GetType() != ValueType::Integer)
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid left operand for addition");
			return Variant();
		}
		if (right.GetType() != ValueType::Number && right.GetType() != ValueType::Integer)
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid right operand for addition");
			return Variant();
		}
		if (left.GetType() == ValueType::Integer && right.GetType() == ValueType::Integer)
		{
			Variant result;
			result.SetInteger(left.GetInteger() + right.GetInteger());
			return result;
		}
		else
		{
			Variant result;
			result.SetNumber(left.GetNumber() + right.GetNumber());
			return result;
		}
	}

	inline Variant operator - (const Variant & left, const Variant & right)
	{
		if (!left.IsNumericType())
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid left operand for subtraction");
			return Variant();
		}
		if (!right.IsNumericType())
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid right operand for subtraction");
			return Variant();
		}
		if (left.GetType() == ValueType::Integer && right.GetType() == ValueType::Integer)
		{
			Variant result;
			result.SetInteger(left.GetInteger() - right.GetInteger());
			return result;
		}
		else
		{
			Variant result;
			result.SetNumber(left.GetNumber() - right.GetNumber());
			return result;
		}
	}

	inline Variant operator * (const Variant & left, const Variant & right)
	{
		if (!left.IsNumericType())
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid left operand for multiplication");
			return Variant();
		}
		if (!right.IsNumericType())
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid right operand for multiplication");
			return Variant();
		}
		if (left.GetType() == ValueType::Integer && right.GetType() == ValueType::Integer)
		{
			Variant result;
			result.SetInteger(left.GetInteger() * right.GetInteger());
			return result;
		}
		else
		{
			Variant result;
			result.SetNumber(left.GetNumber() * right.GetNumber());
			return result;
		}
	}

	inline Variant operator / (const Variant & left, const Variant & right)
	{
		if (!left.IsNumericType())
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid left operand for division");
			return Variant();
		}
		if (!right.IsNumericType())
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid right operand for division");
			return Variant();
		}
		if (left.GetType() == ValueType::Integer && right.GetType() == ValueType::Integer)
		{
			Variant result;
			int64_t l = left.GetInteger();
			int64_t r = right.GetInteger();
			if (l % r == 0)
				result.SetInteger(l / r);
			else
				result.SetNumber(double(l) / (double)r);
			return result;
		}
		else
		{
			Variant result;
			result.SetNumber(left.GetNumber() / right.GetNumber());
			return result;
		}
	}

	inline Variant operator % (const Variant & left, const Variant & right)
	{
		// Handle floating-point numbers with fmod function
		if (left.GetType() == ValueType::Number || right.GetType() == ValueType::Number)
		{
			auto l = left.GetNumber();
			auto r = right.GetNumber();
			return fmod(fmod(l, r) + r, r);
		}

		// Check for non-integer types, especially since co-erced right values will be zero
		if (left.GetType() != ValueType::Integer)
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid left operand for mod");
			return Variant();
		}
		if (right.GetType() != ValueType::Integer)
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid right operand for mod");
			return Variant();
		}

		// Return result from integer mod operation
		auto l = left.GetInteger();
		auto r = right.GetInteger();
		return ((l % r) + r) % r;
	}

	inline bool operator == (const Variant & left, const Variant & right)
	{
		switch (left.GetType())
		{
			case ValueType::Null:
			{
				return right.IsNull();
			}
			case ValueType::Number:
			{
				return left.GetNumber() == right.GetNumber();
			}
			case ValueType::Integer:
			{
				if (!right.IsNumericType())
					return false;
				if (right.IsNumber())
					return left.GetNumber() == right.GetNumber();
				else
					return left.GetInteger() == right.GetInteger();
			}
			case ValueType::Boolean:
			{
				if (!right.IsBoolean())
					return false;
				return left.GetBoolean() == right.GetBoolean();
			}
			case ValueType::String:
			{
				if (!right.IsString())
					return false;
				return left.GetString() == right.GetString();
			}
			case ValueType::Collection:
			{
				if (!right.IsCollection())
					return false;
				return left.GetCollection() == right.GetCollection();
			}
			case ValueType::CollectionItr:
			{
				if (!right.IsCollectionItr())
					return false;
				return left.GetCollectionItr() == right.GetCollectionItr();
			}
			case ValueType::UserObject:
			{
				if (!right.IsUserObject())
					return false;
				return left.GetUserObject() == right.GetUserObject();
			}
			case ValueType::Buffer:
			{
				if (!right.IsBuffer())
					return false;
				return left.GetBuffer() == right.GetBuffer();
			}
			case ValueType::Guid:
			{
				if (!right.IsGuid())
					return false;
				return left.GetGuid() == right.GetGuid();
			}
			case ValueType::ValType:
			{
				if (!right.IsValType())
					return false;
				return left.GetValType() == right.GetValType();
			}
			default:
			{
				assert(!"Unknown variant type!");
			}
		};

		return false;
	}

	inline bool operator < (const Variant & left, const Variant & right)
	{
		switch (left.GetType())
		{
			case ValueType::Null:
			{
				Impl::LogWriteLine(LogLevel::Error, "Error comparing null type with < operator");
				return false;
			}
			case ValueType::Number:
			{
				if (!right.IsNumericType())
					break;
				return left.GetNumber() < right.GetNumber();
			}
			case ValueType::Integer:
			{
				if (!right.IsNumericType())
					break;
				if (right.IsNumber())
					return left.GetNumber() < right.GetNumber();
				else
					return left.GetInteger() < right.GetInteger();
			}
			case ValueType::Boolean:
			{
				if (!right.IsBoolean())
					break;
				return left.GetBoolean() < right.GetBoolean();
			}
			case ValueType::String:
			{
				if (!right.IsString())
					break;
				return left.GetString() < right.GetString();
			}
			case ValueType::Collection:
			{
				Impl::LogWriteLine(LogLevel::Error, "Error comparing collection type with < operator");
				return false;
			}
			case ValueType::CollectionItr:
			{
				Impl::LogWriteLine(LogLevel::Error, "Error comparing collectionitr type with < operator");
				return false;
			}
			case ValueType::UserObject:
			{
				if (!right.IsUserObject())
					break;
				return left.GetUserObject() < right.GetUserObject();
			}
			case ValueType::Buffer:
			{
				Impl::LogWriteLine(LogLevel::Error, "Error comparing buffer type with < operator");
				return false;
			}
			case ValueType::Guid:
			{
				if (!right.IsGuid())
					break;
				return left.GetGuid() < right.GetGuid();
			}
			case ValueType::ValType:
			{
				if (!right.IsValType())
					break;
				return left.GetValType() < right.GetValType();
			}
			default:
			{
				assert(!"Unknown variant type!");
			}
		};

		Impl::LogWriteLine(LogLevel::Error, "Type error in right operand when using < operator");
		return false;
	}

	inline bool operator <= (const Variant & left, const Variant & right)
	{
		switch (left.GetType())
		{
			case ValueType::Null:
			{
				Impl::LogWriteLine(LogLevel::Error, "Error comparing null type with < operator");
				return false;
			}
			case ValueType::Number:
			{
				if (!right.IsNumericType())
					break;
				return left.GetNumber() <= right.GetNumber();
			}
			case ValueType::Integer:
			{
				if (!right.IsNumericType())
					break;
				if (right.IsNumber())
					return left.GetNumber() <= right.GetNumber();
				else
					return left.GetInteger() <= right.GetInteger();
			}
			case ValueType::Boolean:
			{
				if (!right.IsBoolean())
					break;
				return left.GetBoolean() <= right.GetBoolean();
			}
			case ValueType::String:
			{
				if (!right.IsNumericType())
					break;
				return left.GetString() <= right.GetString();
			}
			case ValueType::Collection:
			{
				Impl::LogWriteLine(LogLevel::Error, "Error comparing collection type with < operator");
				return false;
			}
			case ValueType::CollectionItr:
			{
				Impl::LogWriteLine(LogLevel::Error, "Error comparing collectionitr type with < operator");
				return false;
			}
			case ValueType::UserObject:
			{
				if (!right.IsUserObject())
					break;
				return left.GetUserObject() <= right.GetUserObject();
			}
			case ValueType::Buffer:
			{
				Impl::LogWriteLine(LogLevel::Error, "Error comparing buffer type with < operator");
				return false;
			}
			case ValueType::Guid:
			{
				if (!right.IsGuid())
					break;
				return left.GetGuid() <= right.GetGuid();
			}
			case ValueType::ValType:
			{
				if (!right.IsValType())
					break;
				return left.GetValType() <= right.GetValType();
			}
			default:
			{
				assert(!"Unknown variant type!");
			}
		};

		Impl::LogWriteLine(LogLevel::Error, "Type error in right operand when using < operator");
		return false;
	}

	inline bool ValidateValueComparison(const Variant & left, const Variant & right)
	{
		if (!Impl::CheckValueTypeForCompare(left.GetType()))
			return false;
		if (!Impl::CheckValueTypeForCompare(right.GetType()))
			return false;
		if (left.GetType() != right.GetType())
		{
			if (!left.IsNumericType() || !right.IsNumericType())
				return false;
		}
		return true;
	}

} // namespace Jinx



// end --- JxVariant.cpp --- 

