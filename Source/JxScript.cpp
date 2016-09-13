/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

using namespace Jinx;


Script::Script(RuntimeIPtr runtime, BufferPtr bytecode) :
	m_runtime(runtime),
	m_finished(false),
	m_error(false)
{
	m_execution.reserve(6);
	m_execution.push_back(ExecutionFrame(bytecode));

	// Assume default unnamed library unless explicitly overridden
	m_library = m_runtime->GetLibraryInternal("");

	// Read bytecode header
	BytecodeHeader header;
	auto & reader = m_execution.back().reader;
	reader.Read(&header, sizeof(header));
	if (header.signature != BytecodeSignature || header.majorVer != BytecodeMajorVersion || header.minorVer != BytecodeMinorVersion)
	{
		Error("Invalid bytecode");
	}
}

Script::~Script()
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

void Script::Error(const char * message)
{
	LogWriteLine("%s", message);
	m_error = true;
	m_finished = true;
}

bool Script::Execute()
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
		m_finished = false;
		m_execution.back().reader.Seek(sizeof(BytecodeHeader));
	}

	// Mark script execution start time
	auto begin = std::chrono::high_resolution_clock::now();

	uint32_t tickInstCount = 0;
	uint32_t maxInstCount = MaxInstructions();

	Opcode opcode;
	do
	{
		// Read opcode instruction
		uint8_t opByte;
		m_execution.back().reader.Read(&opByte);
		if (opByte >= static_cast<uint32_t>(Opcode::NumOpcodes))
		{
			Error("Invalid operation in bytecode");
			return false;
		}
		opcode = static_cast<Opcode>(opByte);
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

		// Execute the current opcode
		switch (opcode)
		{
			case Opcode::Add:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				auto result = op1 + op2;
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
				auto functionDef = m_runtime->FindFunction(id);
				if (!functionDef)
				{
					Error("Could not find function definition");
					return false;
				}
				// Check to see if this is a bytecode function
				if (functionDef->GetBytecode())
				{
					m_execution.push_back(ExecutionFrame(functionDef->GetBytecode()));
					m_execution.back().reader.Seek(functionDef->GetOffset());
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
					Variant retVal = functionDef->GetCallback()(shared_from_this(), params);
					if (functionDef->HasReturnParameter())
						Push(retVal);
				}
				else
				{
					Error("Error in function definition");
					return false;
				}
				m_execution.back().stackTop = m_stack.size() - functionDef->GetParameterCount();
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
			case Opcode::Exit:
			{
				m_finished = true;
			}
			break;
			case Opcode::Function:
			{
				FunctionSignature signature;	
				signature.Read(m_execution.back().reader);
				if (signature.GetVisibility() != VisibilityType::Local)
					m_library->Functions().Register(signature, false);
				// Note: we add 5 bytes to the current position to skip over the jump command and offset value
				m_runtime->RegisterFunction(signature, m_execution.back().bytecode, m_execution.back().reader.Tell() + 5);
			}
			break;
			case Opcode::Greater:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				auto result = op1 > op2;
				Push(result);
			}
			break;
			case Opcode::GreaterEq:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				auto result = op1 >= op2;
				Push(result);
			}
			break;
			case Opcode::Increment:
			{
				auto op1 = Pop();
				auto op2 = Pop();
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
			case Opcode::Less:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				auto result = op1 < op2;
				Push(result);
			}
			break;
			case Opcode::LessEq:
			{
				auto op2 = Pop();
				auto op1 = Pop();
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
				auto op1 = m_stack[top - 2];
				auto op2 = m_stack[top - 1];
				auto op3 = m_stack[top];
				if (op3.IsNull())
				{
					if (op1 > op2)
						op3 = -1;
					else
						op3 = 1;
				}
				op1 += op3;
				m_stack[top - 2] = op1;
				auto incVal = op3.GetNumber();
				if (incVal > 0)
					Push(op1 <= op2);
				else if (incVal < 0)
					Push(op1 >= op2);
				else
				{
					Error("Incremented loop counter by zero");
					return false;
				}
			}
			break;
			case Opcode::LoopOver:
			{
				assert(m_stack.size() >= 3);
				auto top = m_stack.size() - 1;
				auto itr = m_stack[top - 1];
				assert(itr.IsCollectionItr());
				auto coll = m_stack[top - 2];
				assert(coll.IsCollection() && coll.GetCollection());
				++itr;
				bool finished = itr.GetCollectionItr() == coll.GetCollection()->end();
				m_stack[top - 1] = itr;
				if (!finished)
					m_stack[top] = itr.GetCollectionItr()->second;
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
				Push(result);
			}
			break;
			case Opcode::Multiply:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				auto result = op1 * op2;
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
				Variant itr = coll.GetCollection()->begin();
				Push(itr);
			}
			break;
			case Opcode::PushItrVal:
			{
				assert(m_stack.size() >= 1);
				auto top = m_stack.size() - 1;
				auto coll = m_stack[top];
				if (!coll.IsCollectionItr())
				{
					Error("Expected collection iterator type");
					return false;
				}
				Variant itr = coll.GetCollectionItr();
				Push(itr.GetCollectionItr()->second);
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
			case Opcode::PushPropKey:
			{
				uint64_t id;
				m_execution.back().reader.Read(&id);
				auto var = m_runtime->GetProperty(id);
				auto key = Pop();
				if (!var.IsCollection())
				{
					Error("Expected collection when accessing by key");
				}
				else
				{
					auto coll = var.GetCollection();
					auto itr = coll->find(key);
					if (itr == coll->end())
					{
						Error("Specified key does not exist in collection");
					}
					else
					{
						Push(itr->second);
					}
				}
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
				String name;
				m_execution.back().reader.Read(&name);
				auto var = GetVariable(name);
				Push(var);
			}
			break;
			case Opcode::PushVarKey:
			{
				String name;
				m_execution.back().reader.Read(&name);
				auto var = GetVariable(name);
				auto key = Pop();
				if (!var.IsCollection())
				{
					Error("Expected collection when accessing by key");
				}
				else
				{
					auto coll = var.GetCollection();
					auto itr = coll->find(key);
					if (itr == coll->end())
					{
						Error("Specified key does not exist in collection");
					}
					else
					{
						Push(itr->second);
					}
				}
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
				assert(!m_execution.empty());
				size_t targetSize = m_execution.back().stackTop;
				m_execution.pop_back();
				assert(!m_execution.empty());
				while (m_stack.size() > targetSize)
					m_stack.pop_back();
			}
			break;
			case Opcode::ReturnValue:
			{
				auto val = Pop();
				assert(!m_execution.empty());
				size_t targetSize = m_execution.back().stackTop;
				m_execution.pop_back();
				assert(!m_execution.empty());
				while (m_stack.size() > targetSize)
					m_stack.pop_back();
				Push(val);
			}
			break;
			case Opcode::ScopeBegin:
			{
				ScopeFrame frame;
				frame.stackTop = m_stack.size();
				m_execution.back().names.push_back(frame);
			}
			break;
			case Opcode::ScopeEnd:
			{
				auto stackTop = m_execution.back().names.back().stackTop;
				m_execution.back().names.pop_back();
				while (m_stack.size() > stackTop)
					m_stack.pop_back();
			}
			break;
			case Opcode::SetIndex:
			{
				assert(!m_stack.empty());
				String name;
				m_execution.back().reader.Read(&name);
				int32_t stackIndex;
				m_execution.back().reader.Read(&stackIndex);
				ValueType type;
				m_execution.back().reader.Read<ValueType, uint8_t>(&type);
				size_t index = m_stack.size() + stackIndex;
				if (type != ValueType::Any)
					m_stack[index].ConvertTo(type);
				SetVariableAtIndex(name, index);
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
			case Opcode::SetPropKey:
			{
				RuntimeID id;
				m_execution.back().reader.Read(&id);
				Variant val = Pop();
				Variant key = Pop();
				if (!key.IsKeyType())
				{
					Error("Invalid key type");
					break;
				}
				Variant prop = m_runtime->GetProperty(id);
				if (!prop.IsCollection())
				{
					Error("Expected collection when accessing by key");
					break;
				}
				auto collection = prop.GetCollection();
				(*collection)[key] = val;
			}
			break;
			case Opcode::SetVar:
			{
				String name;
				m_execution.back().reader.Read(&name);
				Variant val = Pop();
				SetVariable(name, val);
			}
			break;
			case Opcode::SetVarKey:
			{
				String name;
				m_execution.back().reader.Read(&name);
				Variant val = Pop();
				Variant key = Pop();
				if (!key.IsKeyType())
				{
					Error("Invalid key type");
					break;
				}
				Variant prop = GetVariable(name);
				if (!prop.IsCollection())
				{
					Error("Expected collection when accessing by key");
					break;
				}
				auto collection = prop.GetCollection();
				(*collection)[key] = val;
			}
			break;
			case Opcode::Subtract:
			{
				auto op2 = Pop();
				auto op1 = Pop();
				auto result = op1 - op2;
				Push(result);
			}
			break;
			case Opcode::Type:
			{
				auto val = Pop();
				Push(val.GetType());
			}
			break;
			case Opcode::Yield:
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
	while (opcode != Opcode::Exit && opcode != Opcode::Yield);

	// Track accumulated script execution time
	auto end = std::chrono::high_resolution_clock::now();
	uint64_t executionTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
	m_runtime->AddScriptExecutionTime(executionTimeNs);

	return true;
}

Variant Script::GetVariable(const String & name) const
{
	auto & names = m_execution.back().names;
	for (auto ritr = names.rbegin(); ritr != names.rend(); ++ritr)
	{
		auto itr = ritr->nameMap.find(name);
		if (itr != ritr->nameMap.end())
		{
			auto index = itr->second;
			if (index >= m_stack.size())
			{
				LogWriteLine("Attempted to access stack at invalid index");
				return Variant();
			}
			return m_stack[itr->second];
		}
	}
	return Variant();
}

bool Script::IsFinished() const
{
	return m_finished || m_error;
}

Variant Script::Pop()
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

void Script::Push(const Variant & value)
{
	m_stack.push_back(value);
}

void Script::SetVariable(const String & name, const Variant & value)
{
	// Search down the variable stack for the variable
	auto & names = m_execution.back().names;
	for (auto ritr = names.rbegin(); ritr != names.rend(); ++ritr)
	{
		auto itr = ritr->nameMap.find(name);
		if (itr != ritr->nameMap.end())
		{
			auto index = itr->second;
			if (index >= m_stack.size())
			{
				LogWriteLine("Attempted to access stack at invalid index");
				return ;
			}
			m_stack[itr->second] = value;
			return;
		}
	}

	// If we don't find the name, create a new variable on the top of the stack
	names.back().nameMap.insert(std::make_pair(name, m_stack.size()));
	m_stack.push_back(value);
}

void Script::SetVariableAtIndex(const String & name, size_t index)
{
	assert(index < m_stack.size());
	m_execution.back().names.back().nameMap.insert(std::make_pair(name, index));
}

