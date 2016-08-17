/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_COMMON_H__
#define JX_COMMON_H__

namespace Jinx
{

	typedef uint64_t RuntimeID;
	const RuntimeID InvalidID = 0;

	const uint32_t LogTabWidth = 4;

	// All script opcodes
	enum class Opcode
	{
		Add,
		And,
		CallFunc,
		Cast,
		Decrement,
		Divide,
		Equals,
		Exit,
		Function,
		Global,
		Greater,
		GreaterEq,
		Increment,
		Jump,
		JumpFalse,
		JumpTrue,
		Less,
		LessEq,
		Library,
		LoopCount,
		LoopOver,
		Mod,
		Multiply,
		Not,
		NotEquals,
		Or,
		Pop,
		PopCount,
		Property,
		PushColl,
		PushItr,
		PushItrVal,
		PushList,
		PushProp,
		PushPropKey,
		PushTop,
		PushVar,
		PushVarKey,
		PushVal,
		Return,
		ReturnValue,
		ScopeBegin,
		ScopeEnd,
		SetIndex,
		SetProp,
		SetPropKey,
		SetVar,
		SetVarKey,
		Subtract,
		Type,
		Yield,
		NumOpcodes,
	};

	const char * GetOpcodeText(Opcode opcode);

	// Symbols
	enum class SymbolType
	{
		None,
		Invalid,
		NewLine,
		NameValue,			// Value types begin
		StringValue,
		NumberValue,
		IntegerValue,
		BooleanValue,
		ForwardSlash,		// Operator begin
		Asterisk,
		Plus,
		Minus,
		Equals,
		NotEquals,
		Percent,
		Comma,
		ParenOpen,
		ParenClose,
		CurlyOpen,
		CurlyClose,
		SquareOpen,
		SquareClose,
		SingleQuote,
		LessThan,
		LessThanEquals,
		GreaterThan,
		GreaterThanEquals, 
		And,				// Keyword begin.  
		As,
		Begin,
		Boolean,
		Break,
		By,
		Class,
		Collection,
		Constructor,
		Count,
		Decrement,
		Destructor,
		Else,
		End,
		From,
		Function,
		Guid,
		If,
		Import,
		Increment,
		Integer,
		Is,
		Library,
		Loop,
		New,
		Not,
		Null,
		Number,
		Object,
		Or,
		Over,
		Private,
		Public,
		Readonly,
		Return,
		Self,
		String,
		To,
		Type,
		While,
		Yield,
		NumSymbols,
	};

	const char * GetSymbolTypeText(SymbolType symbol);

	// Check to see if a symbol is a particular category
	bool IsValue(SymbolType symbol);
	bool IsOperator(SymbolType symbol);
	bool IsKeyword(SymbolType symbol);

	// Get the name of a value type
	const char * GetValueTypeName(ValueType valueType);

	// Visibility type 
	enum class VisibilityType
	{
		Local,
		Private,
		Public,
	};
	
	constexpr uint32_t MakeFourCC(char ch0, char ch1, char ch2, char ch3)
	{
		return ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8 |
			((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24)));
	}

	const uint32_t BytecodeSignature = MakeFourCC('J', 'I', 'N', 'X');
	const uint16_t BytecodeMajorVersion = 0;
	const uint16_t BytecodeMinorVersion = 1;

	struct BytecodeHeader
	{
		BytecodeHeader() :
			signature(BytecodeSignature),
			majorVer(BytecodeMajorVersion),
			minorVer(BytecodeMinorVersion)
		{}
		uint32_t signature;
		uint16_t majorVer;
		uint16_t minorVer;
	};

	static_assert(sizeof(BytecodeHeader) == 8, "BytecodeHeader struct is not properly aligned on this platform");

	template <typename T>
	inline T NextHighestMultiple(T val, T multiple)
	{
		return val + ((multiple - (val % multiple)) % multiple);
	}

	template<typename T, size_t s>
	constexpr size_t countof(T(&)[s])
	{
		return s;
	}

	inline const char * StrCopy(char * dest, size_t destBufferSize, const char * source)
	{
#if defined(JINX_WINDOWS)
		strncpy_s(dest, destBufferSize, source, ((size_t)-1));
		return dest;
#else
		strncpy(dest, source, destBufferSize);
		return dest;
#endif
	}

	RuntimeID GetRandomId();
	uint32_t MaxInstructions();
	bool ErrorOnMaxInstrunction();

	// Forward declarations
	class Runtime;
	typedef std::shared_ptr<Runtime> RuntimeIPtr;
	typedef std::weak_ptr<Runtime> RuntimeWPtr;
};




#endif // JX_COMMON_H__
