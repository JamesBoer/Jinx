/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_LEXER_H__
#define JX_LEXER_H__


namespace Jinx::Impl
{

	struct Symbol
	{
		Symbol() :
			Symbol(SymbolType::None, 0, 0)
		{}
		Symbol(SymbolType t, int32_t ln, int32_t cn) :
			type(t),
			numVal(0),
			lineNumber(ln),
			columnNumber(cn)
		{}
		SymbolType type;
		String text;
		union
		{
			double numVal;
			int64_t intVal;
			bool boolVal;
		};
		uint32_t lineNumber;
		uint32_t columnNumber;
	};

	using SymbolList = std::list<Symbol, Allocator<Symbol>>;
	using SymbolListCItr = SymbolList::const_iterator;

	class Lexer
	{
	public:
		// Lex the script text
		Lexer(const SymbolTypeMap & symbolTypeMap, const char * start, const char * end, const String & name);

		// Do lexing pass to create token list
		bool Execute();

		// Retrieve the finished symbol list
		const SymbolList & GetSymbolList() const { return m_symbolList; }

	private:

		// Log an error
		template<class... Args>
		void Error(const char * format, Args&&... args)
		{
			if (m_error)
				return;
			if (m_name.empty())
				LogWrite(LogLevel::Error, "Error at line %i, column %i:", m_lineNumber, m_columnNumber);
			else
				LogWrite(LogLevel::Error, "Error in '%s' at line %i, column %i: ", m_name.c_str(), m_lineNumber, m_columnNumber);
			LogWriteLine(LogLevel::Error, format, std::forward<Args>(args)...);
			m_error = true;
		}

		// Create a new symbol
		void CreateSymbol(SymbolType type);
		void CreateSymbol(double number);
		void CreateSymbol(int64_t integer);
		void CreateSymbol(const String & name);
		void CreateSymbolString(String && text);

		// Character queries
		inline bool IsEndOfText() const { return (!(*m_current) || m_current > m_end) ? true : false; }
		bool IsName(const char * ptr) const;
		bool IsNameStart(const char * ptr) const;
		inline bool IsNewline(unsigned char c) const { return (c == '\r' || c == '\n') ? true : false; }
		inline bool IsWhitespace(unsigned char c) const { return (c == ' ' || c == '\t') ? true : false; }
		bool IsNextCharacter(unsigned char c) const;
		bool IsNextDigit() const;
		bool IsNumber(unsigned char c) const { return std::isdigit(c) != 0 || c == '.'; }
		bool IsNumberStart(unsigned char c) const { return std::isdigit(c) != 0 || c == '.' || c == '-'; }

		// Text parsing functions
		void AdvanceCurrent();
		void ParseEndOfLine();
		void ParseComment();
		void ParseEllipse();
		void ParseName();
		void ParseNumber();
		void ParseString();
		void ParseWhitespace();
		void ParseWhitespaceAndNewlines();

	private:
		String m_name;
		SymbolList m_symbolList;
		const char * m_start;
		const char * m_end;
		const char * m_current;
		uint32_t m_columnNumber;
		uint32_t m_columnMarker;
		uint32_t m_lineNumber;
		bool m_error;
		const SymbolTypeMap & m_symbolTypeMap;
	};

} // namespace Jinx::Impl

#endif // JX_LEXER_H__
