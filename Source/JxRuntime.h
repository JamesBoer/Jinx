/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_RUNTIME_H__
#define JX_RUNTIME_H__


namespace Jinx
{

	class Runtime : public IRuntime, public std::enable_shared_from_this<Runtime>
	{
	public:
		virtual ~Runtime();

		// IRuntime interface
		BufferPtr Compile(const char * scriptText, String uniqueName, std::initializer_list<String> libraries) override;
		ScriptPtr CreateScript(BufferPtr bytecode) override;
		ScriptPtr CreateScript(const char * scriptText, String uniqueName, std::initializer_list<String> libraries) override;
		ScriptPtr ExecuteScript(const char * scriptcode, String uniqueName, std::initializer_list<String> libraries) override;
		LibraryPtr GetLibrary(const String & name) override;

		// Internal interface
		BufferPtr Compile(BufferPtr scriptBuffer, String uniqueName, std::initializer_list<String> libraries);
		inline LibraryIPtr GetLibraryInternal(const String & name) { return std::static_pointer_cast<Library>(GetLibrary(name)); }
		FunctionDefinitionPtr FindFunction(RuntimeID id) const;
		bool LibraryExists(const String & name) const;
		void RegisterFunction(const FunctionSignature & signature, BufferPtr bytecode, size_t offset);
		void RegisterFunction(const FunctionSignature & signature, FunctionCallback function);
		Variant GetProperty(RuntimeID id) const;
		Variant GetPropertyKeyValue(RuntimeID id, const Variant & key);
		bool PropertyExists(RuntimeID id) const;
		void SetProperty(RuntimeID id, const Variant & value);
		bool SetPropertyKeyValue(RuntimeID id, const Variant & key, const Variant & value);
		PerformanceStats GetScriptPerformanceStats(bool resetStats = true) override;
		void AddScriptExecutionTime(uint64_t timeNs);

	private:

		typedef std::map<String, LibraryIPtr, std::less<String>, Allocator<std::pair<String, LibraryIPtr>>> LibraryMap;
		typedef std::map<RuntimeID, FunctionDefinitionPtr, std::less<RuntimeID>, Allocator<std::pair<RuntimeID, FunctionDefinitionPtr>>> FunctionMap;
		typedef std::map<RuntimeID, Variant, std::less<RuntimeID>, Allocator<std::pair<RuntimeID, Variant>>> PropertyMap;
		void LogBytecode(const BufferPtr & buffer) const;
		void LogSymbols(const SymbolList & symbolList) const;

	private:

		mutable Mutex m_libraryMutex;
		LibraryMap m_libraryMap;
		mutable Mutex m_functionMutex;
		FunctionMap m_functionMap;
		mutable Mutex m_propertyMutex;
		PropertyMap m_propertyMap;
		Mutex m_perfMutex;
		PerformanceStats m_perfStats;
	};


};

#endif // JX_RUNTIME_H__
