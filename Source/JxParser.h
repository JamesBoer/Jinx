/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_PARSER_H__
#define JX_PARSER_H__


namespace Jinx
{

	class Parser
	{
	public:
		Parser(RuntimeIPtr runtime, const SymbolList &symbolList, const String & uniqueName, std::initializer_list<String> libraries);
		
		// Convert the symbol list into bytecode
		bool Execute();

		BufferPtr GetBytecode() const { return m_bytecode; }

	private:

		enum class FunctionParse
		{
			AsExpression,
			AsExpressionMinusFirstParam,
			AsStatement,
		};

		// Log an error
		template<class... Args>
		void Error(const char * format, Args&&... args) 
		{
			if (m_error)
				return;
			m_error = true;
			if (m_currentSymbol == m_symbolList.end())
			{
				LogWriteLine("Error: Unexpected end of script");
				return;
			}
			if (m_uniqueName.empty())
				LogWrite("Error at line %i, column %i: ", m_currentSymbol->lineNumber, m_currentSymbol->columnNumber);
			else
				LogWrite("Error in '%s' at line %i, column %i: ", m_uniqueName.c_str(), m_currentSymbol->lineNumber, m_currentSymbol->columnNumber);
			LogWriteLine(format, std::forward<Args>(args)...);
		}

		// Assign a variable or check that it exists
		void VariableAssign(const String & name);
		bool VariableExists(const String & name) const;

		// Begin or end a new execution frame, like with a function
		void FrameBegin();
		void FrameEnd();

		// Begin or end current execution scope
		void ScopeBegin();
		void ScopeEnd();

		// Retrieve a precedence value for the specified operator
		uint32_t GetOperatorPrecedence(Opcode opcode) const;

		// Check to see if the symbol is a newline or at the end of the list
		bool IsSymbolValid(SymbolListCItr symbol) const;

		// Check a string to see if it's a library name
		bool IsLibraryName(const String & name) const;

		// Check to see if this is a property name
		bool IsPropertyName(const String & libraryName, const String & propertyName) const;

		// Emit functions write to internal bytecode buffer
		void EmitAddress(size_t address);
		size_t EmitAddressPlaceholder();
		void EmitAddressBackfill(size_t address);
		void EmitCount(uint32_t count);
		void EmitName(const String & name); 
		void EmitOpcode(Opcode opcode);
		void EmitValue(const Variant & value);
		void EmitId(RuntimeID id);
		void EmitIndex(int32_t index);
		void EmitValueType(ValueType type);

		// Advance to next sumbol
		void NextSymbol();

		// Returns false and flags an error if the current symbol does not match param.  NextSymbol() is
		// called and true is returned on success.
		bool Expect(SymbolType symbol);

		// If the current symbol matches the parameter, NextSymbol() is called and the function returns true.
		bool Accept(SymbolType symbol);

		// Check for existance of the specified symbol type or content at the current position.  
		// The state of the parser is guaranteed not to be altered.  Returns true or non-null on success.
		bool Check(SymbolType symbol) const;
		bool CheckBinaryOperator() const;
		bool CheckName() const;
		bool CheckValue() const;
		bool CheckValueType() const;
		bool CheckFunctionNamePart() const;
		bool CheckVariable(SymbolListCItr currSym, size_t * symCount = nullptr) const;
		bool CheckVariable() const;
		bool CheckProperty(SymbolListCItr currSym, size_t * symCount = nullptr) const;
		bool CheckProperty(size_t * symCount = nullptr) const;
		bool CheckPropertyName(LibraryIPtr library, SymbolListCItr currSym, size_t * symCount) const;
		String CheckLibraryName() const;
		const FunctionSignature * CheckFunctionCall() const;

		// Parsing functions advance the current symbol, looking for a pattern of symbols
		// and injecting the compiled results into the bytecode buffer.
		VisibilityType ParseScope();
		Opcode ParseLogicalOperator();
		Opcode ParseBinaryOperator();
		Variant ParseValue();
		ValueType ParseValueType();
		String ParseName();
		String ParseMultiName(std::initializer_list<SymbolType> symbols);
		String ParseVariable();
		bool ParseSubscript();
		void ParsePropertyDeclaration(VisibilityType scope, bool readOnly);
		PropertyName ParsePropertyName();
		PropertyName ParsePropertyNameParts(LibraryIPtr library);
		String ParseFunctionNamePart();
		FunctionSignature ParseFunctionSignature(VisibilityType access);
		void ParseFunctionDefinition(VisibilityType scope);
		void ParseFunctionCall(const FunctionSignature * signature);
		void ParseCast();
		void ParseSubexpressionOperand(bool suppressFunctionCall = false);
		void ParseSubexpressionOperation(bool suppressFunctionCall = false);
		void ParseSubexpression(bool suppressFunctionCall = false);
		void ParseExpression(bool suppressFunctionCall = false);
		void ParseErase();
		void ParseIncDec();
		void ParseIfElse();
		void ParseLoop();
		void ParseStatement();
		void ParseBlock();
		void ParseLibraryImports();
		void ParseLibraryDeclaration();
		void ParseScript();

	private:
		// Runtime object
		RuntimeIPtr m_runtime;

		// Unique name
		String m_uniqueName;

		// Symbol list to parse
		const SymbolList & m_symbolList;

		// Current symbol being parsed
		SymbolListCItr m_currentSymbol;

		// Signal an error
		bool m_error;

		// Break jump backfill address
		size_t m_breakAddress;

		// Bytecode data buffer
		BufferPtr m_bytecode;

		// Writes data to an output buffer
		BinaryWriter m_writer;

		// Current library;
		LibraryIPtr m_library;

		// Local function definitions
		FunctionTable m_localFunctions;

		// Library import list
		std::list<String, Allocator<String>> m_importList;

		// Keep track of variables currently in scope
		VariableStackFrame m_variableStackFrame;

		// We're parsing a function that requires a return value
		bool m_requireReturnValue;

		// Validate that we've returned a value
		bool m_returnedValue;
	};

};

#endif // JX_PARSER_H__
