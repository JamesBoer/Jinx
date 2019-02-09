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
