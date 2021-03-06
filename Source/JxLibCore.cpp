/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

namespace Jinx::Impl
{

	inline_t void DebugWriteInternal(LogLevel level, const Variant & var)
	{
		if (var.IsCollection())
		{
			auto coll = var.GetCollection();
			for (const auto & v : *coll)
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

	inline_t Variant Write(ScriptPtr, const Parameters & params)
	{
		if (params.empty())
			return nullptr;
		DebugWriteInternal(LogLevel::Info, params[0]);
		return nullptr;
	}

	inline_t Variant WriteLine(ScriptPtr, const Parameters & params)
	{
		if (!params.empty())
			DebugWriteInternal(LogLevel::Info, params[0]);
		LogWrite(LogLevel::Info, "\n");
		return nullptr;
	}

	inline_t Variant GetSize(ScriptPtr, const Parameters & params)
	{
		switch (params[0].GetType())
		{
		case ValueType::Collection:
			return static_cast<int64_t>(params[0].GetCollection()->size());
		case ValueType::String:
			return static_cast<int64_t>(GetStringCount(params[0].GetString()));
		case ValueType::Buffer:
			return static_cast<int64_t>(params[0].GetBuffer()->Size());
		default:
			break;
		}
		return nullptr;
	}

	inline_t Variant IsEmpty(ScriptPtr, const Parameters & params)
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

	inline_t Variant GetKey(ScriptPtr script, const Parameters & params)
	{
		ScriptIPtr s = std::static_pointer_cast<Script>(script);
		if (!params[0].IsCollectionItr())
		{
			s->Error("'get key' called with non-iterator param");
			return nullptr;
		}
		return params[0].GetCollectionItr().first->first;
	}

	inline_t Variant GetValue(ScriptPtr script, const Parameters & params)
	{
		ScriptIPtr s = std::static_pointer_cast<Script>(script);
		if (params[0].IsCollectionItr())
		{
			return params[0].GetCollectionItr().first->second;
		}
		else if (params[0].IsCoroutine())
		{
			return params[0].GetCoroutine()->GetReturnValue();
		}
		s->Error("'get value' called with invalid param type");
		return nullptr;
	}

	inline_t Variant GetCallStack(ScriptPtr script, [[maybe_unused]] const Parameters & params)
	{
		ScriptIPtr s = std::static_pointer_cast<Script>(script);
		auto functions = s->GetCallStack();
		auto var = CreateCollection();
		int64_t index = 1;
		for (const auto & fnName : functions)
			var->insert(std::make_pair(index++, fnName));
		return var;
	}

	inline_t Variant Call(ScriptPtr script, const Parameters & params)
	{
		ScriptIPtr s = std::static_pointer_cast<Script>(script);
		if (params.empty())
		{
			s->Error("'call' function invoked with no parameters");
			return nullptr;
		}
		if (!params[0].IsFunction())
		{
			s->Error("'call' function requires valid function variable as parameter");
			return nullptr;
		}
		return s->CallFunction(params[0].GetFunction(), Parameters());
	}

	inline_t Variant CallWith(ScriptPtr script, const Parameters & params)
	{
		ScriptIPtr s = std::static_pointer_cast<Script>(script);
		if (params.empty())
		{
			s->Error("'call with' function invoked with no parameters");
			return nullptr;
		}
		if (!params[0].IsFunction())
		{
			s->Error("Invalid parameters to 'call with' function");
			return nullptr;
		}
		Parameters fnParams;
		if (params[1].IsCollection())
		{
			auto collPtr = params[1].GetCollection();
			auto & coll = *collPtr;
			for (const auto & pair : coll)
				fnParams.push_back(pair.second);
		}
		else
		{
			fnParams.push_back(params[1]);
		}
		return s->CallFunction(params[0].GetFunction(), fnParams);
	}

	inline_t Variant AsyncCall(ScriptPtr script, const Parameters & params)
	{
		ScriptIPtr s = std::static_pointer_cast<Script>(script);
		if (params.empty())
		{
			s->Error("'async call' function invoked with no parameters");
			return nullptr;
		}
		if (!params[0].IsFunction())
		{
			s->Error("'async call' function requires valid function variable as parameter");
			return nullptr;
		}
		return CreateCoroutine(s, params[0].GetFunction(), Parameters());
	}

	inline_t Variant AsyncCallWith(ScriptPtr script, const Parameters & params)
	{
		ScriptIPtr s = std::static_pointer_cast<Script>(script);
		if (params.empty())
		{
			s->Error("'async call with' function invoked with no parameters");
			return nullptr;
		}
		if (!params[0].IsFunction())
		{
			s->Error("Invalid parameters to 'async call with' function");
			return nullptr;
		}
		Parameters fnParams;
		if (params[1].IsCollection())
		{
			auto collPtr = params[1].GetCollection();
			auto & coll = *collPtr;
			for (const auto & pair : coll)
				fnParams.push_back(pair.second);
		}
		else
		{
			fnParams.push_back(params[1]);
		}
		return CreateCoroutine(s, params[0].GetFunction(), fnParams);
	}

	inline_t Variant IsFinished(ScriptPtr script, const Parameters & params)
	{
		ScriptIPtr s = std::static_pointer_cast<Script>(script);
		if (params.empty())
		{
			s->Error("'is finished' function invoked with no parameters");
			return nullptr;
		}
		if (!params[0].IsCoroutine())
		{
			s->Error("Invalid parameters to 'is finished' function");
			return nullptr;
		}
		return params[0].GetCoroutine()->IsFinished();
	}

	inline_t Variant AllAreFinished(ScriptPtr script, const Parameters & params)
	{
		auto collPtr = params[0].GetCollection();
		bool allFinished = true;
		for (const auto & pair : *collPtr)
		{
			if (!pair.second.IsCoroutine())
			{
				ScriptIPtr s = std::static_pointer_cast<Script>(script);
				s->Error("Invalid parameters to 'all (of) {} (are) finished' function");
				return false;
			}
			if (!pair.second.GetCoroutine()->IsFinished())
				allFinished = false;
		}
		return allFinished;
	}

	inline_t Variant AnyIsFinished(ScriptPtr script, const Parameters & params)
	{
		auto collPtr = params[0].GetCollection();
		bool anyFinished = false;
		for (const auto & pair : *collPtr)
		{
			if (!pair.second.IsCoroutine())
			{
				ScriptIPtr s = std::static_pointer_cast<Script>(script);
				s->Error("Invalid parameters to 'any (of) {} (is) finished' function");
				return false;
			}
			if (pair.second.GetCoroutine()->IsFinished())
			{
				anyFinished = true;
				break;
			}
		}
		return anyFinished;
	}

	inline_t void RegisterLibCore(RuntimePtr runtime)
	{
		auto library = runtime->GetLibrary("core");

		// Register core functions
		library->RegisterFunction(Visibility::Public, { "write {}" }, Write);
		library->RegisterFunction(Visibility::Public, { "write line {}" }, WriteLine);
		library->RegisterFunction(Visibility::Public, { "{} (get) size" }, GetSize);
		library->RegisterFunction(Visibility::Public, { "{} (is) empty" }, IsEmpty);
		library->RegisterFunction(Visibility::Public, { "{iterator} (get) key" }, GetKey);
		library->RegisterFunction(Visibility::Public, { "{} (get) value" }, GetValue);
		library->RegisterFunction(Visibility::Public, { "(get) call stack" }, GetCallStack);
		library->RegisterFunction(Visibility::Public, { "call {function}" }, Call);
		library->RegisterFunction(Visibility::Public, { "call {function} with {}" }, CallWith);
		library->RegisterFunction(Visibility::Public, { "async call {function}" }, AsyncCall);
		library->RegisterFunction(Visibility::Public, { "async call {function} with {}" }, AsyncCallWith);
		library->RegisterFunction(Visibility::Public, { "{coroutine} (is) finished" }, IsFinished);
		library->RegisterFunction(Visibility::Public, { "all (of) {collection} (are) finished" }, AllAreFinished);
		library->RegisterFunction(Visibility::Public, { "any (of) {collection} (is) finished" }, AnyIsFinished);

		// Register core properties
		library->RegisterProperty(Visibility::Public, Access::ReadOnly, { "newline" }, "\n");
	}

} // namespace Jinx::Impl

