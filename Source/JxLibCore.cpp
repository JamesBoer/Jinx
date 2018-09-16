/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

namespace Jinx
{
	namespace Impl
	{
		inline_t void DebugWriteInternal(const Variant & var, bool writeNewLine)
		{
			if (var.IsCollection())
			{
				const auto & coll = *var.GetCollection();
				for (const auto & v : coll)
				{
					DebugWriteInternal(v.second, writeNewLine);
				}
			}
			else
			{
				auto str = var.GetString();
				auto cstr = str.c_str();
				if (cstr)
					LogWrite(cstr);
				if (writeNewLine)
					LogWrite("\n");
			}
		}

		inline_t Variant Write(ScriptPtr, Parameters params)
		{
			if (params.empty())
				return nullptr;
			DebugWriteInternal(params[0], false);
			return nullptr;
		}

		inline_t Variant WriteLine(ScriptPtr, Parameters params)
		{
			if (params.empty())
			{
				LogWrite("\n");
				return nullptr;
			}
			DebugWriteInternal(params[0], true);
			return nullptr;
		}

		inline_t Variant GetSize(ScriptPtr, Parameters params)
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

		inline_t Variant IsEmpty(ScriptPtr, Parameters params)
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

		inline_t Variant GetKey(ScriptPtr, Parameters params)
		{
			if (!params[0].IsCollectionItr())
			{
				LogWriteLine("'get key' called with non-iterator param");
				return nullptr;
			}
			return params[0].GetCollectionItr().first->first;
		}

		inline_t Variant GetValue(ScriptPtr, Parameters params)
		{
			if (!params[0].IsCollectionItr())
			{
				LogWriteLine("'get value' called with non-iterator param");
				return nullptr;
			}
			return params[0].GetCollectionItr().first->second;
		}

		inline_t Variant GetCallStack(ScriptPtr script, Parameters params)
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
			library->RegisterFunction(Visibility::Public, { "write", "{}" }, Impl::Write);
			library->RegisterFunction(Visibility::Public, { "write", "line", "{}" }, Impl::WriteLine);
			library->RegisterFunction(Visibility::Public, { "{}", "(get)", "size" }, Impl::GetSize);
			library->RegisterFunction(Visibility::Public, { "{}", "(is)", "empty" }, Impl::IsEmpty);
			library->RegisterFunction(Visibility::Public, { "{}", "(get)", "key" }, Impl::GetKey);
			library->RegisterFunction(Visibility::Public, { "{}", "(get)", "value" }, Impl::GetValue);
			library->RegisterFunction(Visibility::Public, { "(get)", "call", "stack" }, Impl::GetCallStack);

			// Register core properties
			library->RegisterProperty(Visibility::Public, Access::ReadOnly, { "newline" }, "\n");
		}

	} // namespace Impl


} // namespace Jinx

