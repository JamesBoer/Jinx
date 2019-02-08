/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_COMMON_H__
#define JX_COMMON_H__

namespace Jinx::Impl
{

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
		EraseItr,
		ErasePropKeyVal,
		EraseVarKeyVal,
		Exit,
		Function,
		Greater,
		GreaterEq,
		Increment,
		Jump,
		JumpFalse,
		JumpFalseCheck,
		JumpTrue,
		JumpTrueCheck,
		Less,
		LessEq,
		Library,
		LoopCount,
		LoopOver,
		Mod,
		Multiply,
		Negate,
		Not,
		NotEquals,
		Or,
		Pop,
		PopCount,
		Property,
		PushColl,
		PushItr,
		PushKeyVal,
		PushList,
		PushProp,
		PushTop,
		PushVar,
		PushVal,
		Return,
		ScopeBegin,
		ScopeEnd,
		SetIndex,
		SetProp,
		SetPropKeyVal,
		SetVar,
		SetVarKeyVal,
		Subtract,
		Type,
		Wait,
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
		Ellipse,
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
		Collection,
		Decrement,
		Else,
		End,
		Erase,
		External,
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
		Set,
		String,
		To,
		Type,
		Until,
		Wait,
		While,
		NumSymbols,
	};

	const char * GetSymbolTypeText(SymbolType symbol);

	// Check to see if a symbol is a particular category
	bool IsConstant(SymbolType symbol);
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
	const uint32_t BytecodeVersion = 1;

	struct BytecodeHeader
	{
		BytecodeHeader() :
			signature(BytecodeSignature),
			version(BytecodeVersion),
			dataSize(0)
		{}
		uint32_t signature;
		uint32_t version;
		uint32_t dataSize;
	};

	static_assert(sizeof(BytecodeHeader) == 12, "BytecodeHeader struct is not properly aligned on this platform");

	const uint32_t DebugSignature = MakeFourCC('J', 'D', 'B', 'G');

	struct DebugHeader
	{
		DebugHeader() :
			signature(DebugSignature),
			lineEntryCount(0),
			dataSize(0)
		{}
		uint32_t signature;
		uint32_t lineEntryCount;
		uint32_t dataSize;
	};

	static_assert(sizeof(DebugHeader) == 12, "DebugHeader struct is not properly aligned on this platform");

	struct DebugLineEntry
	{
		uint32_t opcodePosition;
		uint32_t lineNumber;
	};

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

	// Get number of parts in name
	size_t GetNamePartCount(const String & name);

	// Generate runtime id from unique name information
	RuntimeID GetVariableId(const char * name, size_t nameLen, size_t stackDepth);

	RuntimeID GetRandomId();
	uint32_t MaxInstructions();
	bool ErrorOnMaxInstrunction();
	bool EnableDebugInfo();

	// Forward declarations
	class Runtime;
	using RuntimeIPtr = std::shared_ptr<Runtime>;
	using RuntimeWPtr = std::weak_ptr<Runtime>;

	// Shared aliases
	using SymbolTypeMap = std::map<String, SymbolType, std::less<String>, Allocator<std::pair<const String, SymbolType>>>;

} // namespace Jinx::Impl




#endif // JX_COMMON_H__
