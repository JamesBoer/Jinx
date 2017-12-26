/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

using namespace Jinx;

static GlobalParams s_globalParams;
static std::atomic<uint64_t> s_uniqueId = {0};

struct HashData
{
	char hs[16];
	uint64_t h1;
	uint64_t h2;
};

static const char * s_opcodeName[] =
{
	"add",
	"and",
	"callfunc",
	"cast",
	"decrement",
	"divide",
	"equals",
	"eraseprop",
	"erasepropelem",
	"erasevar",
	"erasevarelem",
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
	"not",
	"notequals",
	"or",
	"pop",
	"popcount",
	"property",
	"pushcoll",
	"pushitr",
	"pushlist",
	"pushprop",
	"pushpropkey",
	"pushtop",
	"pushvar",
	"pushvarkey",
	"pushval",
	"pushvalkey",
	"return",
	"scopebegin",
	"scopeend",
	"setindex",
	"setprop",
	"setpropkey",
	"setvar",
	"setvarkey",
	"subtract",
	"type",
	"wait",
};

static_assert(countof(s_opcodeName) == static_cast<size_t>(Opcode::NumOpcodes), "Opcode descriptions don't match enum count");

const char * Jinx::GetOpcodeText(Opcode opcode)
{
	return s_opcodeName[static_cast<size_t>(opcode)];
}

static const char * s_symbolTypeName[] =
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

static_assert(countof(s_symbolTypeName) == static_cast<size_t>(SymbolType::NumSymbols), "SymbolType descriptions don't match enum count");

const char * Jinx::GetSymbolTypeText(SymbolType symbol)
{
	assert(static_cast<int>(symbol) < static_cast<int>(SymbolType::NumSymbols));
	return s_symbolTypeName[static_cast<size_t>(symbol)];
}

bool Jinx::IsConstant(SymbolType symbol)
{
	assert(static_cast<int>(symbol) < static_cast<int>(SymbolType::NumSymbols));
	return (static_cast<int>(symbol) > static_cast<int>(SymbolType::NameValue)) && (static_cast<int>(symbol) < static_cast<int>(SymbolType::ForwardSlash));
}

bool Jinx::IsOperator(SymbolType symbol)
{
	assert(static_cast<int>(symbol) < static_cast<int>(SymbolType::NumSymbols));
	return (static_cast<int>(symbol) >= static_cast<int>(SymbolType::ForwardSlash)) && (static_cast<int>(symbol) < static_cast<int>(SymbolType::And));
}

static const char * s_valueTypeName[] =
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

static_assert(countof(s_valueTypeName) == (static_cast<size_t>(ValueType::NumValueTypes) + 1), "ValueType names don't match enum count");

const char * Jinx::GetValueTypeName(ValueType valueType)
{
	assert(static_cast<int>(valueType) <= static_cast<int>(ValueType::NumValueTypes));
	return s_valueTypeName[static_cast<size_t>(valueType)];
}

bool Jinx::IsKeyword(SymbolType symbol) 
{ 
	assert(static_cast<int>(symbol) < static_cast<int>(SymbolType::NumSymbols));
	return static_cast<int>(symbol) >= static_cast<int>(SymbolType::And);
}

size_t Jinx::GetNamePartCount(const String & name)
{
	size_t parts = 1;
	for (auto itr = name.begin(); itr != name.end(); ++itr)
	{
		if (*itr == ' ')
			++parts;
	}
	return parts;
}

RuntimeID Jinx::GetRandomId()
{
	// Create hash source of current time, a unique id, and a string
	HashData hd;
	memset(&hd, 0, sizeof(hd));
	StrCopy(hd.hs, 16, "0@@@@UniqueName");
	hd.h1 = std::chrono::high_resolution_clock::time_point().time_since_epoch().count();
	hd.h2 = s_uniqueId++;

	// Return a new random Id from unique hash source
	return GetHash(&hd, sizeof(hd));
}

uint32_t Jinx::MaxInstructions()
{
	return s_globalParams.maxInstructions;
}

bool Jinx::ErrorOnMaxInstrunction()
{
	return s_globalParams.errorOnMaxInstrunctions;
}

void Jinx::Initialize(const GlobalParams & params)
{
	s_globalParams = params;
	InitializeMemory(params);
	InitializeLogging(params);
}

void Jinx::ShutDown()
{
	ShutDownMemory();
}