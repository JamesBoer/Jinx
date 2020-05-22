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
			return static_cast<int64_t>(params[0].GetString().length());
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

	inline_t Variant GetKey(ScriptPtr, const Parameters & params)
	{
		if (!params[0].IsCollectionItr())
		{
			LogWriteLine(LogLevel::Error, "'get key' called with non-iterator param");
			return nullptr;
		}
		return params[0].GetCollectionItr().first->first;
	}

	inline_t Variant GetValue(ScriptPtr, const Parameters & params)
	{
		if (!params[0].IsCollectionItr())
		{
			LogWriteLine(LogLevel::Error, "'get value' called with non-iterator param");
			return nullptr;
		}
		return params[0].GetCollectionItr().first->second;
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

	inline_t void RegisterLibCore(RuntimePtr runtime)
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

