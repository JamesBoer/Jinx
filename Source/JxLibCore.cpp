/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

using namespace Jinx;

static void DebugWriteInternal(const Variant & var)
{
	if (var.IsCollection())
	{
		const auto & coll = *var.GetCollection();
		for (const auto & v : coll)
		{
			DebugWriteInternal(v.second);
		}
	}
	else
	{
		auto str = var.GetString();
		auto cstr = str.c_str();
		if (cstr)
			LogWrite(cstr);
	}
}

static Variant Write(ScriptPtr, Parameters params)
{
	if (params.empty())
		return nullptr;
	DebugWriteInternal(params[0]);
	return nullptr;
}

static Variant WriteLine(ScriptPtr, Parameters params)
{
	if (params.empty())
	{
		LogWrite("\n");
		return nullptr;
	}
	DebugWriteInternal(params[0]);
	LogWrite("\n");
	return nullptr;
}

static Variant GetSize(ScriptPtr, Parameters params)
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

static Variant IsEmpty(ScriptPtr, Parameters params)
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

static Variant GetKey(ScriptPtr, Parameters params)
{
	if (!params[0].IsCollectionItr())
	{
		LogWriteLine("'get key' called with non-iterator param");
		return nullptr;
	}
	return params[0].GetCollectionItr().first->first;
}

static Variant GetValue(ScriptPtr, Parameters params)
{
	if (!params[0].IsCollectionItr())
	{
		LogWriteLine("'get value' called with non-iterator param");
		return nullptr;
	}
	return params[0].GetCollectionItr().first->second;
}

void Jinx::RegisterLibCore(RuntimePtr runtime)
{
	auto library = runtime->GetLibrary("core");

	// Register core functions
	library->RegisterFunction(Visibility::Public, { "write", "{}" }, Write);
	library->RegisterFunction(Visibility::Public, { "write", "line", "{}" }, WriteLine);
	library->RegisterFunction(Visibility::Public, { "{}", "(get)", "size" }, GetSize);
	library->RegisterFunction(Visibility::Public, { "{}", "(is)", "empty" }, IsEmpty);
	library->RegisterFunction(Visibility::Public, { "{}", "(get)", "key" }, GetKey);
	library->RegisterFunction(Visibility::Public, { "{}", "(get)", "value" }, GetValue);

	// Register core properties
	library->RegisterProperty(Visibility::Public, Access::ReadOnly, { "newline" }, "\n");
}

