/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

namespace Jinx::Impl
{

	inline_t Script::Script(RuntimeIPtr runtime, BufferPtr bytecode, Any userContext) :
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

	inline_t Script::~Script()
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

	inline_t void Script::Error(const char * message)
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

	inline_t bool Script::Execute()
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

	inline_t RuntimeID Script::FindFunction(LibraryPtr library, const String & name)
	{
		if (library == nullptr)
			library = m_library;
		auto libraryInt = std::static_pointer_cast<Library>(library);
		return libraryInt->FindFunctionSignature(Visibility::Public, name).GetId();
	}

	inline_t Variant Script::CallFunction(RuntimeID id, Parameters params)
	{
		for (const auto & param : params)
			Push(param);
		return CallFunction(id);
	}

	inline_t Variant Script::CallFunction(RuntimeID id)
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

	inline_t std::vector<String, Allocator<String>> Script::GetCallStack() const
	{
		std::vector<String, Allocator<String>> strings;
		for (const auto & frame : m_execution)
			strings.push_back(frame.name);
		return strings;
	}

	inline_t Variant Script::GetVariable(const String & name) const
	{
		const auto & foldedName = FoldCase(name);
		RuntimeID id = GetVariableId(foldedName.c_str(), foldedName.size(), 1);
		return GetVariable(id);
	}

	inline_t Variant Script::GetVariable(RuntimeID id) const
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

	inline_t bool Script::IsFinished() const
	{
		return m_finished || m_error;
	}

	inline_t Variant Script::Pop()
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

	inline_t void Script::Push(const Variant & value)
	{
		m_stack.push_back(value);
	}

	inline_t void Script::SetVariable(const String & name, const Variant & value)
	{
		const auto & foldedName = FoldCase(name);
		RuntimeID id = GetVariableId(foldedName.c_str(), foldedName.size(), 1);
		SetVariable(id, value);
	}

	inline_t void Script::SetVariable(RuntimeID id, const Variant & value)
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

	inline_t void Script::SetVariableAtIndex(RuntimeID id, size_t index)
	{
		assert(index < m_stack.size());
		m_execution.back().ids.insert(std::make_pair(id, index));
	}

	inline_t std::pair<CollectionPtr, Variant> Script::WalkSubscripts(uint32_t subscripts, CollectionPtr collection)
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

