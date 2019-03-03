/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

namespace Jinx::Impl
{

	inline_t Lexer::Lexer(const SymbolTypeMap & symbolTypeMap, const char * start, const char * end, const String & name) :
		m_name(name),
		m_start(start),
		m_end(end),
		m_current(nullptr),
		m_columnNumber(1),
		m_columnMarker(1),
		m_lineNumber(1),
		m_error(false),
		m_symbolTypeMap(symbolTypeMap)
	{
	}

	inline_t void Lexer::AdvanceCurrent()
	{
		m_current += GetUtf8CharSize(m_current);
		++m_columnNumber;
	}

	inline_t void Lexer::CreateSymbol(SymbolType type)
	{
		Symbol symbol(type, m_lineNumber, m_columnMarker);
		m_symbolList.push_back(symbol);
		m_columnMarker = m_columnNumber;
	}

	inline_t void Lexer::CreateSymbol(double number)
	{
		Symbol symbol(SymbolType::NumberValue, m_lineNumber, m_columnMarker);
		symbol.numVal = number;
		m_symbolList.push_back(symbol);
		m_columnMarker = m_columnNumber;
	}

	inline_t void Lexer::CreateSymbol(int64_t integer)
	{
		Symbol symbol(SymbolType::IntegerValue, m_lineNumber, m_columnMarker);
		symbol.intVal = integer;
		m_symbolList.push_back(symbol);
		m_columnMarker = m_columnNumber;
	}

	inline_t void Lexer::CreateSymbol(const String & name)
	{
		Symbol symbol(SymbolType::NameValue, m_lineNumber, m_columnMarker);
		symbol.text = FoldCase(name);
		auto itr = m_symbolTypeMap.find(symbol.text);
		if (itr != m_symbolTypeMap.end())
		{
			symbol.type = itr->second;
		}
		else
		{
			// Special case detection of boolean 'true' and 'false' values.  We don't
			// want to make these symbol types.  Instead, they need to be a BooleanValue
			// type.
			if (symbol.text == "true")
			{
				symbol.type = SymbolType::BooleanValue;
				symbol.boolVal = true;
			}
			else if (symbol.text == "false")
			{
				symbol.type = SymbolType::BooleanValue;
				symbol.boolVal = false;
			}
		}
		m_symbolList.push_back(symbol);
		m_columnMarker = m_columnNumber;
	}

	inline_t void Lexer::CreateSymbolString(String && text)
	{
		Symbol symbol(SymbolType::StringValue, m_lineNumber, m_columnMarker);
		symbol.text = std::move(text);
		m_symbolList.push_back(symbol);
		m_columnMarker = m_columnNumber;
	}

	inline_t bool Lexer::Execute()
	{
		m_current = m_start;

		// Create a list of tokens for the parser to analyze
		while (!IsEndOfText())
		{
			// Advance past any whitespace
			ParseWhitespace();

			// Check for end of line character
			if (IsNewline(*m_current))
			{
				// Advance past end of line
				ParseEndOfLine();
				continue;
			}

			// May have reached end of text after parsing whitespace or end of line
			if (IsEndOfText())
				break;

			// Check for operators first
			unsigned char c = *m_current;
			switch (c)
			{
			case '-':
				if (IsNextCharacter('-'))
				{
					ParseComment();
					continue;
				}
				else if (IsNextDigit())
				{
					ParseNumber();
					continue;
				}
				else
					CreateSymbol(SymbolType::Minus);
				break;
			case '<':
				if (IsNextCharacter('='))
				{
					CreateSymbol(SymbolType::LessThanEquals);
					AdvanceCurrent();
				}
				else
				{
					CreateSymbol(SymbolType::LessThan);
				}
				break;
			case '>':
				if (IsNextCharacter('='))
				{
					CreateSymbol(SymbolType::GreaterThanEquals);
					AdvanceCurrent();
				}
				else
				{
					CreateSymbol(SymbolType::GreaterThan);
				}
				break;
			case '"':
				ParseString();
				break;
			case '*':
				CreateSymbol(SymbolType::Asterisk);
				break;
			case '/':
				CreateSymbol(SymbolType::ForwardSlash);
				break;
			case '+':
				if (IsNextDigit())
				{
					ParseNumber();
					continue;
				}
				else
					CreateSymbol(SymbolType::Plus);
				break;
			case '=':
				CreateSymbol(SymbolType::Equals);
				break;
			case '!':
				if (IsNextCharacter('='))
				{
					CreateSymbol(SymbolType::NotEquals);
					AdvanceCurrent();
				}
				else
				{
					Error("Invalid symbol '!'");
					break;
				}
				break;
			case '%':
				CreateSymbol(SymbolType::Percent);
				break;
			case ',':
				CreateSymbol(SymbolType::Comma);
				break;
			case '(':
				CreateSymbol(SymbolType::ParenOpen);
				break;
			case ')':
				CreateSymbol(SymbolType::ParenClose);
				break;
			case '{':
				CreateSymbol(SymbolType::CurlyOpen);
				break;
			case '}':
				CreateSymbol(SymbolType::CurlyClose);
				break;
			case '[':
				CreateSymbol(SymbolType::SquareOpen);
				break;
			case ']':
				CreateSymbol(SymbolType::SquareClose);
				break;
			default:
			{
				if (c == '.' && IsNextCharacter('.'))
					ParseEllipse();
				else
				{
					if (IsNumberStart(c))
						ParseNumber();
					else
						ParseName();
				}
				continue;
			}
			};

			// Check for errors
			if (m_error)
				break;

			// Advance one UTF-8 character
			AdvanceCurrent();
		}

		// Make sure programs always end with a new line marker for parsing consistency
		if (!m_symbolList.empty() && m_symbolList.back().type != SymbolType::NewLine)
			CreateSymbol(SymbolType::NewLine);

		// Return error status
		return !m_error;
	}

	inline_t bool Lexer::IsName(const char * ptr) const
	{
		char c = *ptr;
		if (IsWhitespace(c) || IsNewline(c))
			return false;
		if (static_cast<unsigned char>(c) <= 32)
			return false;
		if (c == ',' || c == '.' || c == '[' || c == ']' || c == '(' || c == ')' || c == '{' || c == '}' || c == '/')
			return false;
		return true;
	}

	inline_t bool Lexer::IsNameStart(const char * ptr) const
	{
		char c = *ptr;
		if (IsNumberStart(c) || !IsName(ptr))
			return false;
		return true;
	}

	inline_t bool Lexer::IsNextCharacter(unsigned char c) const
	{
		if (IsEndOfText())
			return false;
		const char ch = *(m_current + 1);
		if (IsNewline(c))
			return false;
		if (ch != c)
			return false;
		return true;
	}

	inline_t bool Lexer::IsNextDigit() const
	{
		if (IsEndOfText())
			return false;
		const char ch = *(m_current + 1);
		if (ch < 0)
			return false;
		if (IsNewline(ch))
			return false;
		if (!std::isdigit(ch))
			return false;
		return true;
	}

	inline_t void Lexer::ParseComment()
	{
		AdvanceCurrent();
		bool blockComment = IsNextCharacter('-');

		// This is a block comment
		if (blockComment)
		{
			AdvanceCurrent();

			// Advance until the end of the line or until we stop seeing dashes
			while (!IsEndOfText())
			{
				if (*m_current != '-')
					break;
				else if (IsNewline(*m_current))
				{
					ParseEndOfLine();
					break;
				}
				AdvanceCurrent();
			}

			// Find minimum run of three dashes to closed block comment
			while (!IsEndOfText())
			{
				uint32_t dashCount = 0;
				while (!IsEndOfText() && *m_current == '-')
				{
					dashCount++;
					AdvanceCurrent();
				}
				if (dashCount >= 3)
					return;
				else if (IsNewline(*m_current))
					ParseEndOfLine();
				else
					AdvanceCurrent();
			}
			Error("Mismatched block comments");
		}
		// This is a single line comment
		else
		{
			// Advance until the end of the line, then return
			while (!IsEndOfText())
			{
				if (IsNewline(*m_current))
				{
					ParseEndOfLine();
					return;
				}
				AdvanceCurrent();
			}
		}
	}

	inline_t void Lexer::ParseEllipse()
	{
		int count = 0;
		while (!IsEndOfText() && count < 3)
		{
			char c = *m_current;
			if (c != '.')
			{
				Error("Expected ellipse");
				return;
			}
			++count;
			AdvanceCurrent();
		}
		ParseWhitespaceAndNewlines();
		if (*m_current == '\r')
			AdvanceCurrent();
		if (IsEndOfText())
			return;
		if (*m_current == '\n')
			AdvanceCurrent();
	}

	inline_t void Lexer::ParseEndOfLine()
	{
		// Handle both Unix and Windows style line ends.
		if (*m_current == '\r')
			AdvanceCurrent();
		if (IsEndOfText())
			return;
		if (*m_current == '\n')
			AdvanceCurrent();
		// No need to add redundant end of line symbols
		if (!m_symbolList.empty() && m_symbolList.back().type != SymbolType::NewLine)
			CreateSymbol(SymbolType::NewLine);
		++m_lineNumber;
		m_columnNumber = 1;
		m_columnMarker = 1;
	}

	inline_t void Lexer::ParseName()
	{
		bool quotedName = false;
		if (*m_current == '\'')
		{
			quotedName = true;
			AdvanceCurrent();
		}

		const char * startName = m_current;

		if (!IsNameStart(m_current))
			Error("Invalid character in name");
		AdvanceCurrent();

		// Advance until we reach the end of the text or hit a non-name character
		while (!IsEndOfText())
		{
			if (quotedName)
			{
				if (*m_current == '\'')
					break;
			}
			else
			{
				if (!IsName(m_current))
					break;
			}
			AdvanceCurrent();
		}
		size_t count = m_current - startName;
		if (quotedName)
			AdvanceCurrent();
		auto name = String(startName, count);
		CreateSymbol(name);
	}

	inline_t void Lexer::ParseNumber()
	{
		const char * startNum = m_current;
		int points = 0;
		if (*startNum == '.')
			points = 1;
		AdvanceCurrent();
		while (!IsEndOfText())
		{
			char c = *m_current;
			if (!IsNumber(c))
			{
				if (IsName(m_current))
				{
					Error("Invalid number format");
					break;
				}
				else
				{
					break;
				}
			}
			if (c == '.')
				++points;
			AdvanceCurrent();
		}
		if (points > 1)
			Error("Invalid number format");
		else if (points == 0)
		{
			int64_t integer = atol(reinterpret_cast<const char *>(startNum));
			CreateSymbol(integer);
		}
		else
		{
			double number = atof(reinterpret_cast<const char *>(startNum));
			CreateSymbol(number);
		}
	}

	inline_t void Lexer::ParseString()
	{
		// Advance past double quote character
		AdvanceCurrent();

		// Mark the start of the string
		const char * startName = m_current;

		// Check for matching end string token
		bool validString = false;
		while (!IsEndOfText())
		{
			if (IsNewline(*m_current))
				break;
			if (*m_current == '"')
			{
				validString = true;
				break;
			}
			AdvanceCurrent();
		}
		if (!validString)
		{
			Error("Could not find matching quote");
			return;
		}
		size_t count = m_current - startName;
		auto str = String(startName, count);
		CreateSymbolString(std::move(str));
	}

	inline_t void Lexer::ParseWhitespace()
	{
		while (!IsEndOfText())
		{
			if (!IsWhitespace(*m_current))
				break;
			if (*m_current == '\t')
				m_columnNumber += (LogTabWidth - 1);
			AdvanceCurrent();
		}
		m_columnMarker = m_columnNumber;
	}


	inline_t void Lexer::ParseWhitespaceAndNewlines()
	{
		while (!IsEndOfText())
		{
			if (!IsWhitespace(*m_current) && !IsNewline(*m_current))
				break;
			AdvanceCurrent();
		}
		m_columnMarker = m_columnNumber;
	}

} // namespace Jinx::Impl

