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

static Variant Size(ScriptPtr, Parameters params)
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

static void AddToCollection(CollectionPtr collection, Variant value)
{
	auto index = collection->size() + 1;
	while (true)
	{
		Variant key = static_cast<int64_t>(index);
		if (collection->find(key) == collection->end())
		{
			collection->insert(std::make_pair(key, value));
			return;
		}
		++index;
	}
}

static Variant AddTo(ScriptPtr, Parameters params)
{
	if (!params[1].IsCollection())
		return nullptr;

	// If the first parameter is a collection, then we merge the maps
	if (params[0].IsCollection())
	{
		auto collToInsert = params[0].GetCollection();
		auto collPtr = params[1].GetCollection();
		for (auto itr = collToInsert->begin(); itr != collToInsert->end(); ++itr)
		{
			if (collPtr->find(itr->first) != collPtr->end())
				AddToCollection(collPtr, itr->second);
			else
				collPtr->insert(std::make_pair(itr->first, itr->second));
		}
		collPtr->insert(collToInsert->begin(), collToInsert->end());
	}
	else
	{
		AddToCollection(params[1].GetCollection(), params[0]);
	}
	return nullptr;
}

static Variant RemoveFrom(ScriptPtr, Parameters params)
{
	if (!params[1].IsCollection())
		return nullptr;

	// If the first param is a collection, then erase all values, assuming they
	// are indices in the second parameter.
	if (params[0].IsCollection())
	{
		auto collTarget = params[1].GetCollection();
		const auto & coll = *params[0].GetCollection();
		for (const auto & v : coll)
			collTarget->erase(v.second);
	}
	else
	{
		auto collTarget = params[1].GetCollection();
		collTarget->erase(params[0]);
	}
	return nullptr;
}

static Variant RemoveValuesFrom(ScriptPtr, Parameters params)
{
	if (!params[1].IsCollection())
		return nullptr;

	if (params[0].IsCollection())
	{
		auto & collTarget = *params[1].GetCollection();
		auto & coll = *params[0].GetCollection();
		for (const auto & val : coll)
		{
			for (auto itr = collTarget.begin(); itr != collTarget.end();)
			{
				if (val.second == itr->second)
					itr = collTarget.erase(itr);
				else
					++itr;
			}
		}
	}
	else
	{
		auto & collTarget = *params[1].GetCollection();
		auto val = params[0];
		for (auto itr = collTarget.begin(); itr != collTarget.end();)
		{
			if (val == itr->second)
				itr = collTarget.erase(itr);
			else
				++itr;
		}
	}
	return nullptr;
}

static Variant Variable(ScriptPtr script, Parameters params)
{
	return script->GetVariable(params[0].GetString());
}

static Variant SetVariable(ScriptPtr script, Parameters params)
{
	script->SetVariable(params[0].GetString(), params[1]);
	return nullptr;
}

void Jinx::RegisterLibCore(RuntimePtr runtime)
{
	auto library = runtime->GetLibrary("core");

	// Register core functions
	library->RegisterFunction(true, false, { "write", "{}" }, Write);
	library->RegisterFunction(true, false, { "write", "line", "{}" }, WriteLine);
	library->RegisterFunction(true, true, { "{}", "size" }, Size);
	library->RegisterFunction(true, true, { "{}", "is", "empty" }, IsEmpty);
	library->RegisterFunction(true, false, { "add", "{}", "to", "{}" }, AddTo);
	library->RegisterFunction(true, false, { "remove", "{}", "from", "{}" }, RemoveFrom);
	library->RegisterFunction(true, false, { "remove", "value/values", "{}", "from", "{}" }, RemoveValuesFrom);
	library->RegisterFunction(true, true, { "variable", "{}" }, Variable);
	library->RegisterFunction(true, false, { "set", "variable", "{}", "to", "{}" }, SetVariable);

	// Register core properties
	library->RegisterProperty(true, true, { "newline" }, "\n");
}



