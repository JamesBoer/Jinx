/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

using namespace Jinx;

Runtime::Runtime()
{
	m_perfStartTime = std::chrono::high_resolution_clock::now();
}

Runtime::~Runtime()
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

void Runtime::AddPerformanceParams(bool finished, uint64_t timeNs, uint64_t instCount)
{
	std::lock_guard<Mutex> lock(m_perfMutex);
	m_perfStats.executionTimeNs += timeNs;
	m_perfStats.instructionCount += instCount;
	m_perfStats.scriptExecutionCount++;
	if (finished)
		m_perfStats.scriptCompletionCount++;
}

BufferPtr Runtime::Compile(BufferPtr scriptBuffer, String uniqueName, std::initializer_list<String> libraries)
{
	// Mark script execution start time
	auto begin = std::chrono::high_resolution_clock::now();

	// Lex script text into tokens
	Lexer lexer(scriptBuffer, uniqueName);

	// Exit if errors when lexing
	if (!lexer.Execute())
		return nullptr;

	// Log all lexed symbols for development and debug purposes
	if (IsLogSymbolsEnabled())
		LogSymbols(lexer.GetSymbolList());

	// Create parser with symbol list
	Parser parser(shared_from_this(), lexer.GetSymbolList(), uniqueName, libraries);

	// Generate bytecode from symbol list
	if (!parser.Execute())
		return nullptr;

	// Log bytecode for development and debug purposes
	if (IsLogBytecodeEnabled())
		LogBytecode(parser);

	// Track accumulated script compilation time and count
	auto end = std::chrono::high_resolution_clock::now();
	uint64_t compilationTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
	std::lock_guard<Mutex> lock(m_perfMutex);
	m_perfStats.scriptCompilationCount++;
	m_perfStats.compilationTimeNs += compilationTimeNs;

	// Return the bytecode
	return parser.GetBytecode();
}

BufferPtr Runtime::Compile(const char * scriptText, String uniqueName, std::initializer_list<String> libraries)
{
	auto scriptBuffer = CreateBuffer();
	scriptBuffer->Write(scriptText, strlen(scriptText) + 1);
	return Compile(scriptBuffer, uniqueName, libraries);
}

ScriptPtr Runtime::CreateScript(BufferPtr bytecode, void * userContext)
{
	return std::allocate_shared<Script>(Allocator<Script>(), shared_from_this(), std::static_pointer_cast<Buffer>(bytecode), userContext);
}

ScriptPtr Runtime::CreateScript(const char * scriptText, void * userContext, String uniqueName, std::initializer_list<String> libraries)
{
	// Compile script text to bytecode
	auto bytecode = Compile(scriptText, uniqueName, libraries);
	if (!bytecode)
		return nullptr;

	// Create and return the script
	return CreateScript(bytecode, userContext);
}

ScriptPtr Runtime::ExecuteScript(const char * scriptcode, void * userContext, String uniqueName, std::initializer_list<String> libraries)
{
	// Compile the text to bytecode
	auto bytecode = Compile(scriptcode, uniqueName, libraries);
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

FunctionDefinitionPtr Runtime::FindFunction(RuntimeID id) const
{
	std::lock_guard<Mutex> lock(m_functionMutex[id % NumMutexes]);
	auto itr = m_functionMap.find(id);
	if (itr == m_functionMap.end())
		return nullptr;
	return itr->second;
}

PerformanceStats Runtime::GetScriptPerformanceStats(bool resetStats)
{
	std::lock_guard<Mutex> lock(m_perfMutex);
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

Variant Runtime::GetProperty(RuntimeID id) const
{
	std::lock_guard<Mutex> lock(m_propertyMutex[id % NumMutexes]);
	auto itr = m_propertyMap.find(id);
	if (itr == m_propertyMap.end())
		return Variant();
	return itr->second;
}

Variant Runtime::GetPropertyKeyValue(RuntimeID id, const Variant & key)
{
	std::lock_guard<Mutex> lock(m_propertyMutex[id % NumMutexes]);
	auto itr = m_propertyMap.find(id);
	if (itr == m_propertyMap.end())
		return Variant();
	auto & var = itr->second;
	if (!var.IsCollection())
		return Variant();
	auto collPtr = var.GetCollection();
	auto vitr = collPtr->find(key);
	if (vitr == collPtr->end())
		return Variant();
	return vitr->second;
}

LibraryPtr Runtime::GetLibrary(const String & name)
{
	std::lock_guard<Mutex> lock(m_libraryMutex);
	auto itr = m_libraryMap.find(name);
	if (itr == m_libraryMap.end())
	{
		auto library = std::allocate_shared<Library>(Allocator<Library>(), shared_from_this(), name);
		m_libraryMap.insert(std::make_pair(name, library));
		return library;
	}
	return itr->second;
}

bool Runtime::LibraryExists(const String & name) const
{
	std::lock_guard<Mutex> lock(m_libraryMutex);
	return m_libraryMap.find(name) != m_libraryMap.end();
}

void Runtime::LogBytecode(const Parser & parser) const
{
	LogWriteLine("\nBytecode:\n====================");
	const size_t columnWidth = 16;
	auto buffer = parser.GetBytecode();
	BinaryReader reader(buffer);
	BytecodeHeader header;
	reader.Read(&header, sizeof(header));
	int instructionCount = 0;
	while (reader.Tell() < buffer->Size())
	{
		// Read opcode instruction
		uint8_t opByte;
		reader.Read(&opByte);
		if (opByte >= static_cast<uint32_t>(Opcode::NumOpcodes))
		{
			LogWriteLine("Invalid operation in bytecode");
			return;
		}
		Opcode opcode = static_cast<Opcode>(opByte);

		++instructionCount;

		const char * opcodeName = GetOpcodeText(opcode);
		size_t opcodeNameLength = strlen(opcodeName);
		LogWrite(opcodeName);

		// Advance to column offset
		assert(opcodeNameLength < columnWidth);
		for (size_t i = 0; i < (columnWidth - opcodeNameLength); ++i)
			LogWrite(" ");
		
		// Read and log opcode arguments
		switch (opcode)
		{
			case Opcode::CallFunc:
			case Opcode::EraseProp:
			case Opcode::ErasePropElem:
			case Opcode::EraseVar:
			case Opcode::EraseVarElem:
			case Opcode::PushProp:
			case Opcode::PushPropKeyVal:
			case Opcode::PushVar:
			case Opcode::PushVarKey:
			case Opcode::SetProp:
			case Opcode::SetPropKeyVal:
			case Opcode::SetVar:
			case Opcode::SetVarKey:
			{
				RuntimeID id;
				reader.Read(&id);
				LogWrite("%s", parser.GetNameFromID(id).c_str());
			}
			break;
			case Opcode::Cast:
			{
				uint8_t b;
				reader.Read(&b);
				auto type = ByteToValueType(b);
				LogWrite("%s", GetValueTypeName(type));
			}
			break;
			case Opcode::Library:
			{
				String name;
				reader.Read(&name);
				LogWrite("%s", name.c_str());
			}
			break;
			case Opcode::Function:
			{
				FunctionSignature signature;
				signature.Read(reader);
				LogWrite("%s", parser.GetNameFromID(signature.GetId()).c_str());
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
				LogWrite("%i", count);
			}
			break;
			case Opcode::PushVal:
			{
				Variant val;
				val.Read(reader);
				LogWrite("%s", val.GetString().c_str());
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
				LogWrite("%s %i %s", parser.GetNameFromID(id).c_str(), stackIndex, GetValueTypeName(type));
			}
			break;
			default:
			{
			}
			break;
		}
		LogWrite("\n");
	}
	LogWrite("\nInstruction Count: %i\n\n", instructionCount);
}

void Runtime::LogSymbols(const SymbolList & symbolList) const
{
	LogWriteLine("\nSymbols:\n====================");
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
			for (int32_t i = 1; i < symbol->columnNumber - offset; ++i)
				LogWrite(" ");
			newLine = false;
		}

		// Write to the log based on the symbol type
		switch (symbol->type)
		{
		case SymbolType::None:
			LogWrite("(None) ");
			break;
		case SymbolType::Invalid:
			LogWrite("(Invalid) ");
			break;
		case SymbolType::NewLine:
			LogWrite("\n");
			newLine = true;
			++lineCount;
			break;
		case SymbolType::NameValue:
			// Display names with spaces as surrounded by single quotes to help delineate them
			// from surrounding symbols.
			if (strstr(symbol->text.c_str(), " "))
				LogWrite("'%s' ", symbol->text.c_str());
			else
				LogWrite("%s ", symbol->text.c_str());
			break;
		case SymbolType::StringValue:
			LogWrite("\"%s\" ", symbol->text.c_str());
			break;
		case SymbolType::NumberValue:
			LogWrite("%f ", symbol->numVal);
			break;
		case SymbolType::IntegerValue:
			LogWrite("%" PRId64 " ", static_cast<int64_t>(symbol->intVal));
			break;
		case SymbolType::BooleanValue:
			LogWrite("%s ", symbol->boolVal ? "true" : "false");
			break;
		default:
			LogWrite("%s ", GetSymbolTypeText(symbol->type));
			break;
		};
	}
	LogWrite("\nLine Count: %i\n\n", lineCount);
}

bool Runtime::PropertyExists(RuntimeID id) const
{
	std::lock_guard<Mutex> lock(m_propertyMutex[id % NumMutexes]);
	return m_propertyMap.find(id) != m_propertyMap.end();
}

void Runtime::RegisterFunction(const FunctionSignature & signature, BufferPtr bytecode, size_t offset)
{
	std::lock_guard<Mutex> lock(m_functionMutex[signature.GetId() % NumMutexes]);
	auto functionDefPtr = std::allocate_shared<FunctionDefinition>(Allocator<FunctionDefinition>(), signature, bytecode, offset);
	m_functionMap.insert(std::make_pair(signature.GetId(), functionDefPtr));
}

void Runtime::RegisterFunction(const FunctionSignature & signature, FunctionCallback function)
{
	std::lock_guard<Mutex> lock(m_functionMutex[signature.GetId() % NumMutexes]);
	auto functionDefPtr = std::allocate_shared<FunctionDefinition>(Allocator<FunctionDefinition>(), signature, function);
	m_functionMap.insert(std::make_pair(signature.GetId(), functionDefPtr));
}

void Runtime::SetProperty(RuntimeID id, const Variant & value)
{
	std::lock_guard<Mutex> lock(m_propertyMutex[id % NumMutexes]);
	m_propertyMap[id] = value;
}

bool Runtime::SetPropertyKeyValue(RuntimeID id, const Variant & key, const Variant & value)
{
	std::lock_guard<Mutex> lock(m_propertyMutex[id % NumMutexes]);
	auto itr = m_propertyMap.find(id);
	if (itr == m_propertyMap.end())
		return false;
	auto & variant = itr->second;
	if (!variant.IsCollection())
		return false;
	auto collPtr = variant.GetCollection();
	(*collPtr)[key] = value;
	return true;
}

RuntimePtr Jinx::CreateRuntime()
{
	auto runtime = std::allocate_shared<Runtime>(Allocator<Runtime>());
	RegisterLibCore(runtime);
	return runtime;
}
