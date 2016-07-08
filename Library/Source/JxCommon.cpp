/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

using namespace Jinx;

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
	"callobjfunc",
	"cast",
	"decrement",
	"divide",
	"equals",
	"exit",
	"function",
	"global",
	"greater",
	"greatereq",
	"increment",
	"jump",
	"jumpfalse",
	"jumptrue",
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
	"pushitrval",
	"pushlist",
	"pushprop",
	"pushpropkey",
	"pushtop",
	"pushvar",
	"pushvarkey",
	"pushval",
	"return",
	"returnvalue",
	"scopebegin",
	"scopeend",
	"setindex",
	"setprop",
	"setpropkey",
	"setvar",
	"setvarkey",
	"subtract",
	"type",
	"yield",
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
	",",
	"(", 
	")", 
	"{", 
	"}", 
	"[", 
	"]", 
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
	"class",
	"collection",
	"constructor",
	"count",
	"decrement",
	"destructor",
	"else",
	"end",
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
	"new",
	"mod",
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
	"self",
	"string",
	"to",
	"type",
	"while",
	"yield",
};

static_assert(countof(s_symbolTypeName) == static_cast<size_t>(SymbolType::NumSymbols), "SymbolType descriptions don't match enum count");

const char * Jinx::GetSymbolTypeText(SymbolType symbol)
{
	assert(static_cast<int>(symbol) < static_cast<int>(SymbolType::NumSymbols));
	return s_symbolTypeName[static_cast<size_t>(symbol)];
}

bool Jinx::IsValue(SymbolType symbol)
{
	assert(static_cast<int>(symbol) < static_cast<int>(SymbolType::NumSymbols));
	return (static_cast<int>(symbol) >= static_cast<int>(SymbolType::NameValue)) && (static_cast<int>(symbol) < static_cast<int>(SymbolType::ForwardSlash));
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

RuntimeID Jinx::GetRandomId()
{
	// Create hash source of current time, a unique id, and a string
	HashData hd;
	memset(&hd, 0, sizeof(hd));
	StrCopy(hd.hs, 16, "0@@@@UniqueName");
	hd.h1 = std::chrono::high_resolution_clock::time_point().time_since_epoch().count();
	hd.h2 = s_uniqueId++;

	// Return a new random Id from unique hash source
	return GetHash(reinterpret_cast<const uint8_t *>(&hd), sizeof(hd));
}

void Jinx::Initialize(const GlobalParams & params)
{
	InitializeMemory(params);
	InitializeLogging(params);
}

void Jinx::ShutDown()
{
	ShutDownMemory();
}