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
		struct HashData
		{
			char hs[16];
			uint64_t h1;
			uint64_t h2;
		};

		struct CommonData
		{
			static inline GlobalParams globalParams;
			static inline std::atomic<uint64_t> uniqueId = { 0 };
			static inline const char * opcodeName[] =
			{
				"add",
				"and",
				"callfunc",
				"cast",
				"decrement",
				"divide",
				"equals",
				"eraseitr",
				"erasepropkeyval",
				"erasevarkeyval",
				"exit",
				"function",
				"greater",
				"greatereq",
				"increment",
				"jump",
				"jumpfalse",
				"jumpfalsecheck",
				"jumptrue",
				"jumptruecheck",
				"less",
				"lesseq",
				"library",
				"loopcount",
				"loopover",
				"mod",
				"multiply",
				"negate",
				"not",
				"notequals",
				"or",
				"pop",
				"popcount",
				"property",
				"pushcoll",
				"pushitr",
				"pushkeyval",
				"pushlist",
				"pushprop",
				"pushtop",
				"pushvar",
				"pushval",
				"return",
				"scopebegin",
				"scopeend",
				"setindex",
				"setprop",
				"setpropkeyval",
				"setvar",
				"setvarkeyval",
				"subtract",
				"type",
				"wait",
			};
			
			static_assert(countof(opcodeName) == static_cast<size_t>(Opcode::NumOpcodes), "Opcode descriptions don't match enum count");

			static inline const char * symbolTypeName[] =
			{
				"none",
				"invalid",
				"newline",
				"name value",
				"string value",
				"number value",
				"integer value",
				"boolean value",
				"/",
				"*",
				"+",
				"-",
				"=",
				"!=",
				"%",
				",",
				"(",
				")",
				"{",
				"}",
				"[",
				"]",
				"...",
				"'",
				"<",
				"<=",
				">",
				">=",
				"and",
				"as",
				"begin",
				"boolean",
				"break",
				"by",
				"collection",
				"decrement",
				"else",
				"end",
				"erase",
				"external",
				"from",
				"function",
				"guid",
				"if",
				"import",
				"increment",
				"integer",
				"is",
				"library",
				"loop",
				"not",
				"null",
				"number",
				"object",
				"or",
				"over",
				"private",
				"public",
				"readonly",
				"return",
				"set",
				"string",
				"to",
				"type",
				"until",
				"wait",
				"while",
			};

			static_assert(countof(symbolTypeName) == static_cast<size_t>(SymbolType::NumSymbols), "SymbolType descriptions don't match enum count");

			static inline const char * valueTypeName[] =
			{
				"null",
				"number",
				"integer",
				"boolean",
				"string",
				"collection",
				"collectionitr",
				"userobject",
				"buffer",
				"guid",
				"valtype",
				"any",
			};

			static_assert(countof(valueTypeName) == (static_cast<size_t>(ValueType::NumValueTypes) + 1), "ValueType names don't match enum count");
		};

		inline_t const char * GetOpcodeText(Opcode opcode)
		{
			return CommonData::opcodeName[static_cast<size_t>(opcode)];
		}

		inline_t const char * GetSymbolTypeText(SymbolType symbol)
		{
			assert(static_cast<int>(symbol) < static_cast<int>(SymbolType::NumSymbols));
			return CommonData::symbolTypeName[static_cast<size_t>(symbol)];
		}

		inline_t bool IsConstant(SymbolType symbol)
		{
			assert(static_cast<int>(symbol) < static_cast<int>(SymbolType::NumSymbols));
			return (static_cast<int>(symbol) > static_cast<int>(SymbolType::NameValue)) && (static_cast<int>(symbol) < static_cast<int>(SymbolType::ForwardSlash));
		}

		inline_t bool IsOperator(SymbolType symbol)
		{
			assert(static_cast<int>(symbol) < static_cast<int>(SymbolType::NumSymbols));
			return (static_cast<int>(symbol) >= static_cast<int>(SymbolType::ForwardSlash)) && (static_cast<int>(symbol) < static_cast<int>(SymbolType::And));
		}

		inline_t const char * GetValueTypeName(ValueType valueType)
		{
			assert(static_cast<int>(valueType) <= static_cast<int>(ValueType::NumValueTypes));
			return CommonData::valueTypeName[static_cast<size_t>(valueType)];
		}

		inline_t bool IsKeyword(SymbolType symbol)
		{
			assert(static_cast<int>(symbol) < static_cast<int>(SymbolType::NumSymbols));
			return static_cast<int>(symbol) >= static_cast<int>(SymbolType::And);
		}

		inline_t size_t GetNamePartCount(const String & name)
		{
			size_t parts = 1;
			for (auto itr = name.begin(); itr != name.end(); ++itr)
			{
				if (*itr == ' ')
					++parts;
			}
			return parts;
		}

		inline_t RuntimeID GetVariableId(const char * name, size_t nameLen, size_t stackDepth)
		{
			RuntimeID id = GetHash(name, nameLen);
			id += static_cast<RuntimeID>(stackDepth);
			return id;
		}

		inline_t RuntimeID GetRandomId()
		{
			// Create hash source of current time, a unique id, and a string
			HashData hd;
			memset(&hd, 0, sizeof(hd));
			StrCopy(hd.hs, 16, "0@@@@UniqueName");
			hd.h1 = std::chrono::high_resolution_clock::time_point().time_since_epoch().count();
			hd.h2 = CommonData::uniqueId++;

			// Return a new random Id from unique hash source
			return GetHash(&hd, sizeof(hd));
		}

		inline_t uint32_t MaxInstructions()
		{
			return CommonData::globalParams.maxInstructions;
		}

		inline_t bool ErrorOnMaxInstrunction()
		{
			return CommonData::globalParams.errorOnMaxInstrunctions;
		}

		inline_t bool EnableDebugInfo()
		{
			return CommonData::globalParams.enableDebugInfo;
		}

	} // namespace Impl

	inline_t String GetVersionString()
	{
		char buffer[32];
		snprintf(buffer, Impl::countof(buffer), "%i.%i.%i", Jinx::MajorVersion, Jinx::MinorVersion, Jinx::PatchNumber);
		return buffer;
	}

	inline_t void Initialize(const GlobalParams & params)
	{
		Impl::CommonData::globalParams = params;
		InitializeMemory(params);
		Impl::InitializeLogging(params);
	}

	inline_t void ShutDown()
	{
		ShutDownMemory();
	}


} // namespace Jinx

