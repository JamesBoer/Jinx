/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

namespace Jinx::Impl
{

	inline_t Runtime::Runtime()
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

	inline_t Runtime::~Runtime()
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

	inline_t void Runtime::AddPerformanceParams(bool finished, uint64_t timeNs, uint64_t instCount)
	{
		std::lock_guard<std::mutex> lock(m_perfMutex);
		m_perfStats.executionTimeNs += timeNs;
		m_perfStats.instructionCount += instCount;
		m_perfStats.scriptExecutionCount++;
		if (finished)
			m_perfStats.scriptCompletionCount++;
	}

	inline_t BufferPtr Runtime::Compile(BufferPtr scriptBuffer, String name, std::initializer_list<String> libraries)
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

	inline_t BufferPtr Runtime::Compile(const char * scriptText, String name, std::initializer_list<String> libraries)
	{
		auto scriptBuffer = CreateBuffer();
		scriptBuffer->Write(scriptText, strlen(scriptText) + 1);
		return Compile(scriptBuffer, name, libraries);
	}

	inline_t ScriptPtr Runtime::CreateScript(BufferPtr bytecode, Any userContext)
	{
		return std::allocate_shared<Script>(Allocator<Script>(), shared_from_this(), std::static_pointer_cast<Buffer>(bytecode), userContext);
	}

	inline_t ScriptPtr Runtime::CreateScript(const char * scriptText, Any userContext, String name, std::initializer_list<String> libraries)
	{
		// Compile script text to bytecode
		auto bytecode = Compile(scriptText, name, libraries);
		if (!bytecode)
			return nullptr;

		// Create and return the script
		return CreateScript(bytecode, userContext);
	}

	inline_t ScriptPtr Runtime::ExecuteScript(const char * scriptcode, Any userContext, String name, std::initializer_list<String> libraries)
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

	inline_t FunctionDefinitionPtr Runtime::FindFunction(RuntimeID id) const
	{
		std::lock_guard<std::mutex> lock(m_functionMutex[id % NumMutexes]);
		auto itr = m_functionMap.find(id);
		if (itr == m_functionMap.end())
			return nullptr;
		return itr->second;
	}

	inline_t PerformanceStats Runtime::GetScriptPerformanceStats(bool resetStats)
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

	inline_t Variant Runtime::GetProperty(RuntimeID id) const
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex[id % NumMutexes]);
		auto itr = m_propertyMap.find(id);
		if (itr == m_propertyMap.end())
			return Variant();
		return itr->second;
	}

	inline_t LibraryPtr Runtime::GetLibrary(const String & name)
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

	inline_t bool Runtime::LibraryExists(const String & name) const
	{
		std::lock_guard<std::mutex> lock(m_libraryMutex);
		return m_libraryMap.find(name) != m_libraryMap.end();
	}

	inline_t void Runtime::LogBytecode(const Parser & parser) const
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

	inline_t void Runtime::LogSymbols(const SymbolList & symbolList) const
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

	inline_t bool Runtime::PropertyExists(RuntimeID id) const
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex[id % NumMutexes]);
		return m_propertyMap.find(id) != m_propertyMap.end();
	}

	inline_t void Runtime::RegisterFunction(const FunctionSignature & signature, BufferPtr bytecode, size_t offset)
	{
		std::mutex & mutex = m_functionMutex[signature.GetId() % NumMutexes];
		std::lock_guard<std::mutex> lock(mutex);
		auto functionDefPtr = std::allocate_shared<FunctionDefinition>(Allocator<FunctionDefinition>(), signature, bytecode, offset);
		m_functionMap.insert(std::make_pair(signature.GetId(), functionDefPtr));
	}

	inline_t void Runtime::RegisterFunction(const FunctionSignature & signature, FunctionCallback function)
	{
		std::mutex & mutex = m_functionMutex[signature.GetId() % NumMutexes];
		std::lock_guard<std::mutex> lock(mutex);
		auto functionDefPtr = std::allocate_shared<FunctionDefinition>(Allocator<FunctionDefinition>(), signature, function);
		m_functionMap.insert(std::make_pair(signature.GetId(), functionDefPtr));
	}

	inline_t void Runtime::SetProperty(RuntimeID id, std::function<void(Variant &)> fn)
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex[id % NumMutexes]);
		auto& prop = m_propertyMap[id];
		fn(prop);
	}

	inline_t void Runtime::SetProperty(RuntimeID id, const Variant & value)
	{
		std::lock_guard<std::mutex> lock(m_propertyMutex[id % NumMutexes]);
		m_propertyMap[id] = value;
	}

	inline_t BufferPtr Runtime::StripDebugInfo(BufferPtr bytecode) const
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

	inline_t RuntimePtr CreateRuntime()
	{
		auto runtime = std::allocate_shared<Impl::Runtime>(Allocator<Impl::Runtime>());
		Impl::RegisterLibCore(runtime);
		return runtime;
	}

} // namespace Jinx

