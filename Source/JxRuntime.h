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
		void RegisterFunction(const FunctionSignature & signature, const BufferPtr & bytecode, size_t offset);
		void RegisterFunction(const FunctionSignature & signature, FunctionCallback function);
		Variant GetProperty(RuntimeID id) const;
		bool PropertyExists(RuntimeID id) const;
		bool SetProperty(RuntimeID id, std::function<bool(Variant &)> fn);
		void SetProperty(RuntimeID id, const Variant & value);
		void SetProperty(RuntimeID id, Variant && value);
		void AddPerformanceParams(bool finished, uint64_t timeNs, uint64_t instCount);
		const SymbolTypeMap & GetSymbolTypeMap() const { return m_symbolTypeMap; }

	private:
		using LibraryMap = std::map<String, LibraryIPtr, std::less<String>, StaticAllocator<std::pair<const String, LibraryIPtr>, RuntimeArenaSize>>;
		using FunctionMap = std::map<RuntimeID, FunctionDefinitionPtr, std::less<RuntimeID>, StaticAllocator<std::pair<const RuntimeID, FunctionDefinitionPtr>, RuntimeArenaSize>>;
		using PropertyMap = std::map<RuntimeID, Variant, std::less<RuntimeID>, StaticAllocator<std::pair<const RuntimeID, Variant>, RuntimeArenaSize>>;
		void LogBytecode(const Parser & parser) const;
		void LogSymbols(const SymbolList & symbolList) const;

	private:
		StaticArena<RuntimeArenaSize> m_staticArena;
		static const size_t NumMutexes = 8;
		mutable std::mutex m_libraryMutex;
		LibraryMap m_libraryMap{ m_staticArena };
		mutable std::mutex m_functionMutex[NumMutexes];
		FunctionMap m_functionMap{ m_staticArena };
		mutable std::mutex m_propertyMutex[NumMutexes];
		PropertyMap m_propertyMap{ m_staticArena };
		std::mutex m_perfMutex;
		PerformanceStats m_perfStats;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_perfStartTime;
		SymbolTypeMap m_symbolTypeMap{ m_staticArena };
	};

} // namespace Jinx::Impl

#endif // JX_RUNTIME_H__
