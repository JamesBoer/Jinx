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

#include <memory>
#include <functional>
#include <vector>
#include <map>
#include <string>
#include <cstddef>
#include <limits>
#include <cstring>

#include "JxMemory.h"
#include "JxBuffer.h"
#include "JxCollection.h"
#include "JxGuid.h"
#include "JxVariant.h"

// Platform definitions
#if defined(_WIN32) || defined(_WIN64)
#define JINX_WINDOWS
#endif

#if defined(__linux__) || defined(__linux)
#define JINX_LINUX
#endif

#ifdef __APPLE__
#ifdef __MACH__
#define JINX_MACOS
#endif
#endif

/*! \namespace */
namespace Jinx
{
	/// Major version number
	static const uint32_t MajorVersion = 0;

	/// Minor version number
	static const uint32_t MinorVersion = 15;

	/// Patch number
	static const uint32_t PatchNumber = 0;

	/// Version string
	static const char * VersionString = "0.15.0";

	// Forward declaration
	class IScript;

	// Shared pointer to script object
	typedef std::shared_ptr<IScript> ScriptPtr;

	// Function parameters
	typedef std::vector<Variant, Allocator<Variant>> Parameters;

	// Signature for native function callback
	typedef std::function<Variant(ScriptPtr, const Parameters &)> FunctionCallback;

	enum class Visibility
	{
		Public,
		Private
	};

	enum class Access
	{
		ReadWrite,
		ReadOnly,
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
		\param visibility Indicates whether property is public or private.
		\param name A list of names and parameters.  Parameters are indicated with a "{}" string, while names are expected to conform to 
		standard Jinx identifier naming rules.
		\param function The callback function executed by the script.
		\return Returns true on success or false on failure.
		*/
		virtual bool RegisterFunction(Visibility visibility, std::initializer_list<String> name, FunctionCallback function) = 0;

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
	typedef std::shared_ptr<ILibrary> LibraryPtr;
	

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

		/// Get a user context pointer
		/**
		\return void pointer optionally passed at script creation.  This is intended to be
		used by native library functions to retrieve user-specific data or objects.
		*/
		virtual void * GetUserContext() const = 0;

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
		\param uniqueName The name of the script, typically the filename, used for debugging
		and diagnostic purposes.
		\param libraries A list of libraries to import by default.
		\return A BufferPtr containing compiled bytecode on success or a nullptr on failure.
		\sa CreateScript()
		*/
		virtual BufferPtr Compile(const char * scriptText, String uniqueName = String(), std::initializer_list<String> libraries = {}) = 0;

		/// Create a script from bytecode
		/**
		Create script from compiled bytecode retrieved from the Compile() function.
		\param bytecode A BufferPtr containing the compiled bytecode generated by
		the output of the Compile() function.
		\param userContext A void pointer containing per-script user-defined data or object.
		\return A ScriptPtr ready for execution.
		\sa Compile(), IScript
		*/
		virtual ScriptPtr CreateScript(BufferPtr bytecode, void * userContext = nullptr) = 0;

		/// Compile and create script from text
		/**
		Compiles script text to bytecode, then creates and returns a script if successful.
		\param scriptText A C string containing text to compile to bytecode
		\param userContext A void pointer containing per-script user-defined data or object.
		\param uniqueName The name of the script, typically the filename, used for debugging
		and diagnostic purposes.
		\param libraries A list of libraries to import by default.
		\return A ScriptPtr containing compiled bytecode on success or a nullptr on failure.
		*/
		virtual ScriptPtr CreateScript(const char * scriptText, void * userContext = nullptr, String uniqueName = String(), std::initializer_list<String> libraries = {}) = 0;

		/// Compile, create, and execute a script
		/**
		Compiles script text to bytecode, then creates and execute script, and returns a 
		script if successful.
		\param scriptText A C string containing text to compile to bytecode
		\param userContext A void pointer containing per-script user-defined data or object.
		\param uniqueName The name of the script, typically the filename, used for debugging
		and diagnostic purposes.
		\param libraries A list of libraries to import by default.
		\return A ScriptPtr containing compiled bytecode on success or a nullptr on failure.
		*/
		virtual ScriptPtr ExecuteScript(const char * scriptText, void * userContext = nullptr, String uniqueName = String(), std::initializer_list<String> libraries = {}) = 0;

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

	protected:
		virtual ~IRuntime() {}
	};

	/// Shared pointer to runtime object
	typedef std::shared_ptr<IRuntime> RuntimePtr;


	/// Create a runtime object
	/**
	\sa IRuntime
	*/
	RuntimePtr CreateRuntime();


	/// Prototype for global memory allocation function callback
	typedef std::function<void *(size_t)> AllocFn;

	/// Prototype for global memory re-allocation function callback
	typedef std::function<void *(void *, size_t)> ReallocFn;

	/// Prototype for global memory free function callback
	typedef std::function<void(void *)> FreeFn;

	/// Prototype for global logging function callback
	typedef std::function<void(const char *)> LogFn;


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

};

#endif // JINX_H__
