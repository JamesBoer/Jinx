/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

namespace Jinx::Impl
{

	inline_t Parser::Parser(RuntimeIPtr runtime, const SymbolList & symbolList, const String & name, std::initializer_list<String> libraries) :
		m_runtime(runtime),
		m_name(name),
		m_symbolList(symbolList),
		m_lastLine(1),
		m_error(false),
		m_breakAddress(false),
		m_bytecode(CreateBuffer()),
		m_writer(m_bytecode)
	{
		m_currentSymbol = symbolList.begin();
		m_importList = libraries;
		if (EnableDebugInfo())
			m_debugLines.reserve(1024);
	}

	inline_t Parser::Parser(RuntimeIPtr runtime, const SymbolList & symbolList, const String & name) :
		m_runtime(runtime),
		m_name(name),
		m_symbolList(symbolList),
		m_lastLine(1),
		m_error(false),
		m_breakAddress(false),
		m_bytecode(CreateBuffer()),
		m_writer(m_bytecode)
	{
		m_currentSymbol = symbolList.begin();
	}

	inline_t bool Parser::Execute()
	{
		// Reserve 1K space
		m_bytecode->Reserve(1024);

		// Write bytecode header
		BytecodeHeader header;
		m_writer.Write(&header, sizeof(header));

		// Write script name
		m_writer.Write(m_name);

		// Parse script symbols into bytecode
		ParseScript();

		// Return error status
		return !m_error;
	}

	inline_t FunctionSignature Parser::ParseFunctionSignature(VisibilityType access, const String & libraryName)
	{
		m_library = m_runtime->GetLibraryInternal(libraryName);
		return ParseFunctionSignature(access, false);
	}

	inline_t String Parser::GetNameFromID(RuntimeID id) const
	{
		auto itr = m_idNameMap.find(id);
		if (itr == m_idNameMap.end())
			return String();
		return itr->second;
	}

	inline_t RuntimeID Parser::VariableNameToRuntimeID(const String & name)
	{
		auto id = GetVariableId(name.c_str(), name.size(), m_variableStackFrame.GetStackDepthFromName(name));
		m_idNameMap[id] = name;
		return id;
	}

	inline_t void Parser::VariableAssign(const String & name)
	{
		if (!m_variableStackFrame.VariableAssign(name))
			Error("%s", m_variableStackFrame.GetErrorMessage());
	}

	inline_t bool Parser::VariableExists(const String & name) const
	{
		return m_variableStackFrame.VariableExists(name);
	}

	inline_t void Parser::FrameBegin()
	{
		m_variableStackFrame.FrameBegin();
	}

	inline_t void Parser::FrameEnd()
	{
		if (!m_variableStackFrame.FrameEnd())
			Error("%", m_variableStackFrame.GetErrorMessage());
	}

	inline_t void Parser::ScopeBegin()
	{
		if (!m_variableStackFrame.ScopeBegin())
			Error("%s", m_variableStackFrame.GetErrorMessage());
		EmitOpcode(Opcode::ScopeBegin);
	}

	inline_t void Parser::ScopeEnd()
	{
		if (!m_variableStackFrame.ScopeEnd())
			Error("%s", m_variableStackFrame.GetErrorMessage());
		EmitOpcode(Opcode::ScopeEnd);
	}

	inline_t uint32_t Parser::GetOperatorPrecedence(Opcode opcode) const
	{
		switch (opcode)
		{
		case Opcode::Multiply: return 1;
		case Opcode::Divide: return 1;
		case Opcode::Mod: return 1;
		case Opcode::Add: return 2;
		case Opcode::Subtract: return 2;
		case Opcode::Less: return 3;
		case Opcode::LessEq: return 3;
		case Opcode::Greater: return 3;
		case Opcode::GreaterEq: return 3;
		case Opcode::Equals: return 3;
		case Opcode::NotEquals: return 3;
		case Opcode::And: return 4;
		case Opcode::Or: return 4;
		default:
			assert(!"Unknown opcode used in binary expression");
			return 0;
		};
	}

	inline_t bool Parser::IsSymbolValid(SymbolListCItr symbol) const
	{
		if (m_error)
			return false;
		if (symbol == m_symbolList.end())
			return false;
		if (symbol->type == SymbolType::NewLine)
			return false;
		return true;
	}

	inline_t bool Parser::IsLibraryName(const String & name) const
	{
		if (name == m_library->GetName())
			return true;
		for (const auto & n : m_importList)
		{
			if (name == n)
				return true;
		}
		return false;
	}

	inline_t bool Parser::IsPropertyName(const String & libraryName, const String & propertyName) const
	{
		if (!libraryName.empty())
		{
			auto library = m_runtime->GetLibraryInternal(libraryName);
			return library->PropertyNameExists(propertyName);
		}
		else
		{
			if (m_library->PropertyNameExists(propertyName))
				return true;
			for (const auto & n : m_importList)
			{
				auto library = m_runtime->GetLibraryInternal(n);
				if (library->PropertyNameExists(propertyName))
					return true;
			}
		}
		return false;
	}

	inline_t void Parser::EmitAddress(size_t address)
	{
		m_writer.Write(uint32_t(address));
	}

	inline_t size_t Parser::EmitAddressPlaceholder()
	{
		size_t offset = m_writer.Tell();
		m_writer.Write(uint32_t(0));
		return offset;
	}

	inline_t void Parser::EmitAddressBackfill(size_t address)
	{
		// This function is used to back-fill jump locations once we've parsed far enough to know
		// where a jump should land.

		// Retrieve current writer location
		size_t current = m_writer.Tell();
		// Seek to previous offset location
		m_writer.Seek(address);
		// Write the current location as the new jump offset location.
		m_writer.Write(static_cast<uint32_t>(current));
		// Restore the current writer location
		m_writer.Seek(current);
	}

	inline_t void Parser::EmitCount(uint32_t count)
	{
		m_writer.Write(count);
	}

	inline_t void Parser::EmitName(const String & name)
	{
		m_writer.Write(name);
	}

	inline_t void Parser::EmitOpcode(Opcode opcode)
	{
		// Only bother with writing this information if we're generating debug info
		if (EnableDebugInfo())
		{
			// Only write a new entry when we're at a new line, since that's all we're tracking
			if (m_debugLines.empty() || m_debugLines.back().lineNumber != m_lastLine)
			{
				auto pos = static_cast<uint32_t>(m_writer.Tell());
				m_debugLines.push_back({ pos, m_lastLine });
			}
		}
		m_writer.Write<Opcode, uint8_t>(opcode);
	}

	inline_t void Parser::EmitValue(const Variant & value)
	{
		value.Write(m_writer);
	}

	inline_t void Parser::EmitId(RuntimeID id)
	{
		m_writer.Write(id);
	}

	inline_t void Parser::EmitIndex(int32_t index)
	{
		m_writer.Write(index);
	}

	inline_t void Parser::EmitValueType(ValueType valueType)
	{
		m_writer.Write(ValueTypeToByte(valueType));
	}

	inline_t void Parser::WriteBytecodeHeader()
	{
		// Get bytecode data size
		size_t currentPos = m_writer.Tell();
		size_t bytecodeSize = currentPos - sizeof(BytecodeHeader);
		if (bytecodeSize > UINT_MAX)
		{
			Error("Bytecode data has exceeded maximum allowable size");
			return;
		}
		BytecodeHeader header;
		header.dataSize = static_cast<uint32_t>(bytecodeSize);
		m_writer.Seek(0);
		m_writer.Write(&header, sizeof(header));
		m_writer.Seek(currentPos);
	}

	inline_t void Parser::WriteDebugInfo()
	{
		DebugHeader opcodeHeader;
		opcodeHeader.lineEntryCount = static_cast<uint32_t>(m_debugLines.size());
		opcodeHeader.dataSize = static_cast<uint32_t>(m_debugLines.size() + sizeof(DebugLineEntry));
		m_writer.Write(&opcodeHeader, sizeof(opcodeHeader));
		for (const auto & lineEntry : m_debugLines)
			m_writer.Write(&lineEntry, sizeof(lineEntry));
	}

	inline_t void Parser::NextSymbol()
	{
		m_lastLine = m_currentSymbol->lineNumber;
		++m_currentSymbol;
	}

	inline_t bool Parser::Accept(SymbolType symbol)
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return false;
		if (symbol == m_currentSymbol->type)
		{
			NextSymbol();
			return true;
		}
		return false;
	}

	inline_t bool Parser::Expect(SymbolType symbol)
	{
		if (Accept(symbol))
			return true;
		Error("Expected symbol %s", GetSymbolTypeText(symbol));
		return false;
	}

	inline_t bool Parser::Check(SymbolType symbol) const
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return false;
		return (symbol == m_currentSymbol->type);
	}

	inline_t bool Parser::CheckBinaryOperator() const
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return false;
		auto type = m_currentSymbol->type;
		return
			type == SymbolType::And ||
			type == SymbolType::Asterisk ||
			type == SymbolType::Equals ||
			type == SymbolType::NotEquals ||
			type == SymbolType::ForwardSlash ||
			type == SymbolType::GreaterThan ||
			type == SymbolType::GreaterThanEquals ||
			type == SymbolType::LessThan ||
			type == SymbolType::LessThanEquals ||
			type == SymbolType::Minus ||
			type == SymbolType::Or ||
			type == SymbolType::Percent ||
			type == SymbolType::Plus;
	}

	inline_t bool Parser::CheckName() const
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return false;
		return m_currentSymbol->type == SymbolType::NameValue;
	}

	inline_t bool Parser::CheckValue() const
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return false;
		auto type = m_currentSymbol->type;
		return
			type == SymbolType::NumberValue ||
			type == SymbolType::IntegerValue ||
			type == SymbolType::BooleanValue ||
			type == SymbolType::StringValue ||
			type == SymbolType::Null;
	}

	inline_t bool Parser::CheckValueType() const
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return false;
		auto type = m_currentSymbol->type;
		return
			type == SymbolType::Number ||
			type == SymbolType::Integer ||
			type == SymbolType::Boolean ||
			type == SymbolType::String ||
			type == SymbolType::Collection ||
			type == SymbolType::Guid ||
			type == SymbolType::Null;
	}

	inline_t bool Parser::CheckFunctionNamePart() const
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return false;
		return (m_currentSymbol->type == SymbolType::NameValue) || IsKeyword(m_currentSymbol->type);
	}

	inline_t String Parser::CheckLibraryName() const
	{
		String libraryName;
		if (m_currentSymbol->type == SymbolType::NameValue || IsKeyword(m_currentSymbol->type))
		{
			String tokenName = m_currentSymbol->text;
			if (tokenName == m_library->GetName())
			{
				libraryName = m_library->GetName();
			}
			else
			{
				for (auto & importName : m_importList)
				{
					if (tokenName == importName)
					{
						libraryName = importName;
						break;
					}
				}
			}
		}
		return libraryName;
	}

	inline_t bool Parser::CheckFunctionCallPart(const FunctionSignatureParts & parts, size_t partsIndex, SymbolListCItr currSym, SymbolListCItr endSym, FunctionMatch & match) const
	{
		// If we reach the end of the parts list, return success
		if (partsIndex >= parts.size())
			return false;

		// If we've exceeded our range, return failure
		if (currSym == endSym)
			return false;

		const auto & part = parts[partsIndex];

		// Check for invalid symbol
		if (!IsSymbolValid(currSym))
			return part.optional && partsIndex == (parts.size() - 1);

		// Recursively iterate through all parts and match them against the signature.
		if (part.partType == FunctionSignaturePartType::Name)
		{
			if (!part.optional && currSym->type != SymbolType::NameValue && !IsKeyword(currSym->type))
				return false;

			for (const auto & name : part.names)
			{
				if (name == currSym->text)
				{
					match.partData.push_back(std::make_tuple(FunctionSignaturePartType::Name, 1, part.optional));
					auto newCurrSym = currSym;
					++newCurrSym;
					auto newMatch = match;
					if (CheckFunctionCallPart(parts, partsIndex + 1, newCurrSym, endSym, newMatch))
					{
						match = newMatch;
						return true;
					}
				}
			}

			if (part.optional)
			{
				auto newMatch = match;
				newMatch.partData.push_back(std::make_tuple(FunctionSignaturePartType::Name, 0, true));
				if (CheckFunctionCallPart(parts, partsIndex + 1, currSym, endSym, newMatch))
				{
					match = newMatch;
					return true;
				}
			}
		}
		else
		{
			// Set a flag if this is the initial token in the expression.
			bool isInitialToken = partsIndex >= match.partData.size();

			// Check for valid expressions
			size_t symCount = 0;
			if (CheckVariable(currSym, &symCount) || CheckProperty(currSym, &symCount))
			{
				for (size_t i = 0; i < symCount; ++i)
					++currSym;
			}
			else if (currSym->type == SymbolType::ParenOpen)
			{
				size_t parenCount = 1;
				symCount = 1;
				while (parenCount)
				{
					++currSym;
					++symCount;
					if (!IsSymbolValid(currSym))
						return false;
					if (currSym->type == SymbolType::ParenOpen)
						++parenCount;
					else if (currSym->type == SymbolType::ParenClose)
						--parenCount;
				}
				++currSym;
			}
			else if (currSym->type == SymbolType::ParenClose)
			{
				return false;
			}
			else if (IsConstant(currSym->type) || currSym->type == SymbolType::Not)
			{
				++currSym;
				symCount = 1;
			}
			else if (!isInitialToken && (IsOperator(currSym->type) || currSym->type == SymbolType::And || currSym->type == SymbolType::Or))
			{
				++currSym;
				symCount = 1;
			}
			else
			{
				if (partsIndex != 0)
				{
					FunctionMatch exprMatch = CheckFunctionCall(false, currSym, endSym);
					if (exprMatch.signature)
					{
						for (size_t i = 0; i < exprMatch.partData.size(); ++i)
						{
							++currSym;
							++symCount;
						}
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}

			// If our match structure isn't up to date, push new match items.  Otherwise,
			// advance our expression token count.  This will be important for determining
			// how many symbols we need to parse for an expression.
			if (isInitialToken)
				match.partData.push_back(std::make_tuple(FunctionSignaturePartType::Parameter, symCount, false));
			else
				std::get<1>(match.partData[partsIndex]) = std::get<1>(match.partData[partsIndex]) + symCount;

			// Store off current match structure
			auto newMatch = match;

			// Check to see if advancing part index leads to success
			if (CheckFunctionCallPart(parts, partsIndex + 1, currSym, endSym, newMatch))
			{
				match = newMatch;
				return true;
			}

			// Check symbols against the current part.
			if (CheckFunctionCallPart(parts, partsIndex, currSym, endSym, newMatch))
			{
				match = newMatch;
				return true;
			}
		}

		return match.partData.size() == parts.size();
	}

	inline_t Parser::FunctionMatch Parser::CheckFunctionCall(const FunctionSignature & signature, SymbolListCItr currSym, SymbolListCItr endSym, bool skipInitialParam) const
	{
		FunctionMatch match;
		const auto & parts = signature.GetParts();
		size_t partsIndex = 0;
		if (skipInitialParam && signature.GetParts()[0].partType == FunctionSignaturePartType::Parameter)
		{
			partsIndex++;
			match.partData.push_back(std::make_tuple(FunctionSignaturePartType::Parameter, 0, false));
		}
		if (CheckFunctionCallPart(parts, partsIndex, currSym, endSym, match))
		{
			match.signature = &signature;
			return match;
		}
		return FunctionMatch();
	}

	inline_t Parser::FunctionMatch Parser::CheckFunctionCall(const FunctionList & functionList, SymbolListCItr currSym, SymbolListCItr endSym, bool skipInitialParam) const
	{
		FunctionMatch match;
		for (const auto & functionSig : functionList)
		{
			auto currentSymbol = currSym;
			auto newMatch = CheckFunctionCall(functionSig, currentSymbol, endSym, skipInitialParam);
			if (newMatch.signature)
			{
				if (!match.signature || match.partData.size() < newMatch.partData.size())
					match = newMatch;
			}
		}
		return match;
	}

	inline_t Parser::FunctionMatch Parser::CheckFunctionCall(LibraryIPtr library, SymbolListCItr currSym, SymbolListCItr endSym, bool skipInitialParam) const
	{
		const auto functionPtrList = library->Functions();

		FunctionMatch match;
		for (const auto functionSig : functionPtrList)
		{
			auto currentSymbol = currSym;
			auto newMatch = CheckFunctionCall(*functionSig, currentSymbol, endSym, skipInitialParam);
			if (newMatch.signature)
			{
				if (!match.signature || match.partData.size() < newMatch.partData.size())
					match = newMatch;
			}
		}
		return match;
	}

	inline_t Parser::FunctionMatch Parser::CheckFunctionCall(bool skipInitialParam, SymbolListCItr currSym, SymbolListCItr endSym) const
	{
		FunctionMatch match;

		// Store current symbol
		auto currentSymbol = currSym;

		// Check for error or invalid symbols
		if (m_error || currentSymbol == m_symbolList.end())
			return match;

		// Any operators other than open parentheses mean this can't be a function call
		if (IsOperator(currentSymbol->type) && (currentSymbol->type != SymbolType::ParenOpen))
			return match;

		// Check for explicit library name in the first symbol.  libraryName is empty if not found.
		String libraryName = CheckLibraryName();

		// Advance the current symbol if we found a valid library name
		if (!libraryName.empty())
		{
			++currentSymbol;
			if (currentSymbol == m_symbolList.end())
				return match;
		}

		// If we explicitly specify a library name, then only look in that library
		if (!libraryName.empty())
		{
			auto library = m_runtime->GetLibraryInternal(libraryName);
			match = CheckFunctionCall(library, currentSymbol, endSym, skipInitialParam);
		}
		else
		{
			// Check local function table for signature match
			match = CheckFunctionCall(m_localFunctions, currentSymbol, endSym, skipInitialParam);

			// If not found in local function table, search in libraries for a function match
			if (!match.signature)
			{
				// Check the current library for a signature match
				match = CheckFunctionCall(m_library, currentSymbol, endSym, skipInitialParam);

				// If a library name isn't specified or a signature wasn't found, search first in current library, then in order of imports
				if (!match.signature)
				{
					// Search default library first
					auto library = m_runtime->GetLibraryInternal(libraryName);
					match = CheckFunctionCall(library, currentSymbol, endSym, skipInitialParam);

					// If function wasn't found in default library, search through all import libraries
					if (!match.signature)
					{
						// Loop through all imported library names
						for (const auto & libName : m_importList)
						{
							// Make sure the library exists
							if (!m_runtime->LibraryExists(libName))
							{
								LogWriteLine(LogLevel::Warning, "Unable to find library '%s'", libName.c_str());
								return FunctionMatch();
							}

							// Search for function in this library
							library = m_runtime->GetLibraryInternal(libName);
							auto newMatch = CheckFunctionCall(library, currentSymbol, endSym, skipInitialParam);
							if (newMatch.signature)
							{
								if (match.signature)
								{
									LogWriteLine(LogLevel::Warning, "Ambiguous function name detected.  Use library name to disambiguate.");
									return FunctionMatch();
								}
								else
								{
									match = newMatch;
									if (match.signature->GetVisibility() == VisibilityType::Private && library != m_library)
									{
										LogWriteLine(LogLevel::Error, "Unable to call library function with private scope.");
										return FunctionMatch();
									}
								}
							}
						}
					}
				}
			}
		}

		return match;
	}

	inline_t Parser::FunctionMatch Parser::CheckFunctionCall(bool skipInitialParam, SymbolListCItr endSym) const
	{
		return CheckFunctionCall(skipInitialParam, m_currentSymbol, endSym);
	}

	inline_t Parser::FunctionMatch Parser::CheckFunctionCall() const
	{
		return CheckFunctionCall(false, m_symbolList.end());
	}

	inline_t bool Parser::CheckVariable(SymbolListCItr currSym, size_t * symCount) const
	{
		if (m_error || currSym == m_symbolList.end())
			return false;
		if (currSym->type != SymbolType::NameValue)
			return false;

		// Check up to the max number of parts
		auto maxParts = m_variableStackFrame.GetMaxVariableParts();
		for (size_t s = maxParts; s > 0; --s)
		{
			auto curr = currSym;
			String name = curr->text;
			size_t sc = 1;
			bool error = false;
			for (size_t i = 1; i < s; ++i)
			{
				++curr;
				if (!IsSymbolValid(curr) || curr->text.empty())
				{
					error = true;
					break;
				}
				name += " ";
				name += curr->text;
				++sc;
			}
			if (error)
				continue;
			bool exists = VariableExists(name);
			if (exists)
			{
				if (symCount)
					*symCount = sc;
				return true;
			}
		}
		return false;

	}

	inline_t bool Parser::CheckVariable() const
	{
		return CheckVariable(m_currentSymbol);
	}

	inline_t bool Parser::CheckProperty(SymbolListCItr currSym, size_t * symCount) const
	{
		// Check symbol validity
		if (m_error || currSym == m_symbolList.end())
			return false;
		if (currSym->type != SymbolType::NameValue)
			return false;

		// Check to see if this begins with a library name
		String libraryName = CheckLibraryName();
		if (!libraryName.empty())
		{
			// Get next symbol and check validity
			auto currentSymbol = currSym;
			++currentSymbol;
			if (currentSymbol == m_symbolList.end())
				return false;
			if (currentSymbol->type != SymbolType::NameValue)
				return false;

			// Check for property name in this specific library
			auto library = m_runtime->GetLibraryInternal(libraryName);
			assert(library);
			return CheckPropertyName(library, currentSymbol, symCount);
		}

		// Check for property name in the current library
		assert(m_library);
		if (CheckPropertyName(m_library, currSym, symCount))
			return true;

		// Check against all imported libraries
		for (auto & importName : m_importList)
		{
			auto library = m_runtime->GetLibraryInternal(importName);
			if (library != m_library && CheckPropertyName(library, currSym, symCount))
				return true;
		}
		return false;
	}

	inline_t bool Parser::CheckProperty(size_t * symCount) const
	{
		return CheckProperty(m_currentSymbol, symCount);
	}

	inline_t bool Parser::CheckPropertyName(LibraryIPtr library, SymbolListCItr currSym, size_t * symCount) const
	{
		// Internal function called once we've established a library to check
		// Check up to the max number of parts

		// Initial error checks
		if (m_error || currSym == m_symbolList.end())
			return false;
		if (currSym->type != SymbolType::NameValue)
			return false;

		// Check for names starting with max property count
		auto maxParts = library->GetMaxPropertyParts();
		for (size_t s = maxParts; s > 0; --s)
		{
			auto curr = currSym;
			String name = curr->text;
			size_t sc = 1;
			for (size_t i = 1; i < s; ++i)
			{
				++curr;
				if (!IsSymbolValid(curr) || curr->text.empty())
					continue;
				name += " ";
				name += curr->text;
				++sc;
			}
			bool exists = library->PropertyNameExists(name);
			if (exists)
			{
				if (symCount)
					*symCount = sc;
				return true;
			}
		}
		return false;
	}

	inline_t VisibilityType Parser::ParseScope()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return VisibilityType::Local;
		if (m_currentSymbol->type == SymbolType::Private)
		{
			NextSymbol();
			return VisibilityType::Private;
		}
		if (m_currentSymbol->type == SymbolType::Public)
		{
			NextSymbol();
			return VisibilityType::Public;
		}
		return VisibilityType::Local;
	}

	inline_t Opcode Parser::ParseBinaryOperator()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return Opcode::NumOpcodes;

		Opcode opcode = Opcode::NumOpcodes;
		switch (m_currentSymbol->type)
		{
		case SymbolType::And:
			opcode = Opcode::And;
			break;
		case SymbolType::Asterisk:
			opcode = Opcode::Multiply;
			break;
		case SymbolType::Equals:
			opcode = Opcode::Equals;
			break;
		case SymbolType::NotEquals:
			opcode = Opcode::NotEquals;
			break;
		case SymbolType::ForwardSlash:
			opcode = Opcode::Divide;
			break;
		case SymbolType::GreaterThan:
			opcode = Opcode::Greater;
			break;
		case SymbolType::GreaterThanEquals:
			opcode = Opcode::GreaterEq;
			break;
		case SymbolType::LessThan:
			opcode = Opcode::Less;
			break;
		case SymbolType::LessThanEquals:
			opcode = Opcode::LessEq;
			break;
		case SymbolType::Minus:
			opcode = Opcode::Subtract;
			break;
		case SymbolType::Or:
			opcode = Opcode::Or;
			break;
		case SymbolType::Percent:
			opcode = Opcode::Mod;
			break;
		case SymbolType::Plus:
			opcode = Opcode::Add;
			break;
		default:
			Error("Unknown type when parsing binary operator");
			break;
		}
		NextSymbol();
		return opcode;
	}

	inline_t Variant Parser::ParseValue()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return Variant();
		Variant val;
		switch (m_currentSymbol->type)
		{
		case SymbolType::NumberValue:
			val.SetNumber(m_currentSymbol->numVal);
			break;
		case SymbolType::IntegerValue:
			val.SetInteger(m_currentSymbol->intVal);
			break;
		case SymbolType::BooleanValue:
			val.SetBoolean(m_currentSymbol->boolVal);
			break;
		case SymbolType::StringValue:
			val.SetString(m_currentSymbol->text);
			break;
		case SymbolType::Null:
			break;
		default:
			Error("Unknown value");
		}
		NextSymbol();
		return val;
	}

	inline_t ValueType Parser::ParseValueType()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return ValueType::Null;

		auto type = m_currentSymbol->type;
		NextSymbol();
		switch (type)
		{
		case SymbolType::Number:
			return ValueType::Number;
		case SymbolType::Integer:
			return ValueType::Integer;
		case SymbolType::Boolean:
			return ValueType::Boolean;
		case SymbolType::String:
			return ValueType::String;
		case SymbolType::Null:
			return ValueType::Null;
		case SymbolType::Collection:
			return ValueType::Collection;
		case SymbolType::Guid:
			return ValueType::Guid;
		default:
			Error("Unknown type");
			break;
		}
		return ValueType::Null;
	}

	inline_t String Parser::ParseName()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return String();
		if (m_currentSymbol->type != SymbolType::NameValue)
		{
			Error("Unexpected symbol type when parsing name");
			return String();
		}
		String s = m_currentSymbol->text;
		NextSymbol();
		return s;
	}

	inline_t String Parser::ParseMultiName(std::initializer_list<SymbolType> symbols)
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return String();
		if (m_currentSymbol->type != SymbolType::NameValue)
		{
			Error("Unexpected symbol type when parsing name");
			return String();
		}
		String s = m_currentSymbol->text;
		NextSymbol();

		while (IsSymbolValid(m_currentSymbol) && !m_currentSymbol->text.empty())
		{
			if (m_currentSymbol->type != SymbolType::NameValue)
			{
				for (auto symbol : symbols)
				{
					if (m_currentSymbol->type == symbol)
						return s;
				}
			}
			s += " ";
			s += m_currentSymbol->text;
			NextSymbol();
		}

		return s;
	}

	inline_t String Parser::ParseVariable()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return String();
		if (m_currentSymbol->type != SymbolType::NameValue)
		{
			Error("Unexpected symbol type when parsing variable");
			return String();
		}

		// Check up to the max number of parts until we find a variable match
		auto maxParts = m_variableStackFrame.GetMaxVariableParts();
		for (size_t s = maxParts; s > 0; --s)
		{
			auto curr = m_currentSymbol;
			String name = curr->text;
			size_t symbolCount = 1;
			for (size_t i = 1; i < s; ++i)
			{
				++curr;
				if (!IsSymbolValid(curr) || curr->text.empty())
					continue;
				name += " ";
				name += curr->text;
				++symbolCount;
			}
			bool exists = VariableExists(name);
			if (exists)
			{
				// Now that we know the longest variable count that matches, advance this number of symbols
				for (size_t i = 0; i < symbolCount; ++i)
					NextSymbol();

				// Return the variable name
				return name;
			}
		}
		Error("Could not parse variable name");
		return String();
	}

	inline_t void Parser::ParseSubscriptGet()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return;
		while (Accept(SymbolType::SquareOpen))
		{
			ParseExpression();
			Expect(SymbolType::SquareClose);
			EmitOpcode(Opcode::PushKeyVal);
		}
	}

	inline_t uint32_t Parser::ParseSubscriptSet()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return 0;
		uint32_t count = 0;
		while (Accept(SymbolType::SquareOpen))
		{
			ParseExpression();
			Expect(SymbolType::SquareClose);
			++count;
		}
		return count;
	}

	inline_t void Parser::ParsePropertyDeclaration(VisibilityType scope, bool readOnly)
	{
		if (m_error)
			return;

		if (m_currentSymbol == m_symbolList.end())
		{
			Error("Unexpected end of script when parsing property declaration");
			return;
		}
		if (m_currentSymbol->type != SymbolType::NameValue)
		{
			Error("Unexpected symbol type when parsing property declaration");
			return;
		}

		// Check if first keyword matches a library name
		for (auto libName : m_importList)
		{
			if (libName == m_currentSymbol->text)
			{
				Error("Property name cannot start with an import library name");
				return;
			}
		}

		// Find out which library this property belongs to
		auto propertyLibrary = m_library;
		auto libraryName = CheckLibraryName();
		if (!libraryName.empty())
		{
			propertyLibrary = m_runtime->GetLibraryInternal(libraryName);
			NextSymbol();
		}

		// Parse the property name
		if (!CheckName())
		{
			Error("Property name expected");
			return;
		}

		// Search for multi-part property names
		String name = ParseMultiName({ SymbolType::To });

		if (propertyLibrary->PropertyNameExists(name))
		{
			Error("Property is already defined");
			return;
		}

		// Create a PropertyName object for registration
		PropertyName propertyName(scope, readOnly, propertyLibrary->GetName(), name);

		// Register the property name, and check for duplicates
		if (!propertyLibrary->RegisterPropertyName(propertyName, true))
		{
			Error("Error registering property name.  Possible duplicate.");
			return;
		}

		// Set property value
		EmitOpcode(Opcode::Property);
		propertyName.Write(m_writer);

		if (Accept(SymbolType::To))
		{
			ParseExpression();
			EmitOpcode(Opcode::SetProp);
			EmitId(propertyName.GetId());
			m_idNameMap[propertyName.GetId()] = propertyName.GetName();
		}
		else if (readOnly)
		{
			// A declaration with no assignment is allowed, but not for a readonly property			
			Error("Must assign property an initial value");
			return;
		}
		Expect(SymbolType::NewLine);
	}

	inline_t PropertyName Parser::ParsePropertyName()
	{
		PropertyName propertyName;
		// Check to see if this begins with a library name
		auto library = m_library;
		String libraryName = CheckLibraryName();
		if (!libraryName.empty())
		{
			libraryName = ParseName();
			library = m_runtime->GetLibraryInternal(libraryName);
			propertyName = ParsePropertyNameParts(library);
			if (!propertyName.IsValid())
			{
				Error("Could not find property name");
				return PropertyName();
			}
			// Check for invalid scope
			if (m_library->GetName() != libraryName && propertyName.GetVisibility() != VisibilityType::Public)
			{
				Error("Unable to access private property");
				return PropertyName();
			}
		}
		// No library name, so we have to search for the best match
		else
		{
			// Check default library for property first
			propertyName = ParsePropertyNameParts(m_library);

			// Check import names if we can't find the property name locally.
			if (!propertyName.IsValid())
			{
				bool foundProperty = false;
				for (const auto & import : m_importList)
				{
					// Get import library by name
					library = m_runtime->GetLibraryInternal(import);

					// Don't bother checking the default library again
					if (library == m_library)
						continue;

					// Attempt to find valid import library name
					auto newPropertyName = ParsePropertyNameParts(library);
					if (newPropertyName.IsValid())
					{
						// If we haven't specified the library name explicitly, we can assume we're looking
						// for a different library.
						if (m_library->GetName() != libraryName && newPropertyName.GetVisibility() != VisibilityType::Public)
							continue;

						// Check for multiple found property names, which indicates this name is ambiguous
						if (foundProperty)
						{
							Error("Ambiguous property name found");
							return PropertyName();
						}
						foundProperty = true;
						propertyName = newPropertyName;
					}
				}

				// Check for invalid scope
				if (propertyName.IsValid() && library != m_library && propertyName.GetVisibility() != VisibilityType::Public)
				{
					Error("Unable to access private property");
					return PropertyName();
				}
			}
		}
		m_idNameMap[propertyName.GetId()] = propertyName.GetName();
		return propertyName;
	}

	inline_t PropertyName Parser::ParsePropertyNameParts(LibraryIPtr library)
	{
		// Check for initial errors
		if (m_error || m_currentSymbol == m_symbolList.end() || m_currentSymbol->type != SymbolType::NameValue)
			return PropertyName();

		// Check up to the max number of parts until we find a variable match
		auto maxParts = library->GetMaxPropertyParts();
		for (size_t s = maxParts; s > 0; --s)
		{
			auto curr = m_currentSymbol;
			String name = curr->text;
			size_t symbolCount = 1;
			for (size_t i = 1; i < s; ++i)
			{
				++curr;
				if (!IsSymbolValid(curr) || curr->text.empty())
					continue;
				name += " ";
				name += curr->text;
				++symbolCount;
			}
			bool exists = library->PropertyNameExists(name);
			if (exists)
			{
				// Now that we know the longest variable count that matches, advance this number of symbols
				for (size_t i = 0; i < symbolCount; ++i)
					NextSymbol();

				// Return the property name
				return library->GetPropertyName(name);
			}
		}
		return PropertyName();

	}

	inline_t String Parser::ParseFunctionNamePart()
	{
		if (m_error || m_currentSymbol == m_symbolList.end())
			return String();
		if (m_currentSymbol->text.empty())
		{
			Error("Unexpected symbol type when parsing function name");
			return String();
		}
		String s = m_currentSymbol->text;
		NextSymbol();
		return s;
	}

	inline_t FunctionSignature Parser::ParseFunctionSignature(VisibilityType scope, bool signatureOnly)
	{
		if (Check(SymbolType::NewLine))
		{
			Error("Empty function signature");
			return FunctionSignature();
		}
		bool parsedParameter = false;
		bool parsedNonKeywordName = false;
		int parsedNameCount = 0;
		int optionalNameCount = 0;
		int parsedNameCountSection = 0;
		int optionalNameCountSection = 0;
		FunctionSignatureParts signatureParts;
		while (!Check(SymbolType::NewLine))
		{
			FunctionSignaturePart part;
			if (Accept(SymbolType::CurlyOpen))
			{
				if (parsedParameter)
				{
					Error("Functions cannot have multiple variables without a name separating them");
					return FunctionSignature();
				}
				if (parsedNameCount && parsedNameCountSection <= optionalNameCountSection)
				{
					Error("Arguments can't be separated soley by optional parameters");
					return FunctionSignature();
				}
				part.partType = FunctionSignaturePartType::Parameter;
				if (CheckValueType())
				{
					part.valueType = ParseValueType();
				}
				if (CheckName())
				{
					String paramName = ParseMultiName({ SymbolType::CurlyClose });
					if (m_library->PropertyNameExists(paramName))
					{
						Error("Function parameter name '%s' already exists as a property name", paramName.c_str());
						return FunctionSignature();
					}
					part.names.push_back(paramName);
				}
				else if (signatureOnly)
				{
					Error("No variable name or class identifier found in function signature");
					return FunctionSignature();
				}
				Expect(SymbolType::CurlyClose);
				parsedParameter = true;
				parsedNameCountSection = 0;
				optionalNameCountSection = 0;
			}
			else
			{
				part.optional = Accept(SymbolType::ParenOpen);
				if (!CheckFunctionNamePart())
				{
					Error("Invalid name in function signature");
					return FunctionSignature();
				}
				parsedNameCount++;
				parsedNameCountSection++;
				if (IsKeyword(m_currentSymbol->type) == false)
					parsedNonKeywordName = true;
				part.partType = FunctionSignaturePartType::Name;
				part.names.push_back(ParseFunctionNamePart());
				while (Accept(SymbolType::ForwardSlash))
				{
					if (!CheckFunctionNamePart())
					{
						Error("Invalid name in function signature");
						return FunctionSignature();
					}
					auto name = ParseFunctionNamePart();
					for (auto n : part.names)
					{
						if (n == name)
						{
							Error("Duplicate alternative name in function signature");
							return FunctionSignature();
						}
					}
					part.names.push_back(name);
				}
				if (part.optional)
				{
					optionalNameCount++;
					optionalNameCountSection++;
					if (!Expect(SymbolType::ParenClose))
					{
						Error("Expected closing parentheses for optional function name part");
						return FunctionSignature();
					}
				}
				parsedParameter = false;
			}
			signatureParts.push_back(part);
		}
		if (!Expect(SymbolType::NewLine))
		{
			Error("Expected new line at end of function signature");
			return FunctionSignature();
		}

		// Check for function signature validity with matching keywords
		if (!parsedNonKeywordName)
		{
			if (parsedNameCount == 1 && signatureParts.size() == 1)
			{
				Error("Function signature cannot match keyword");
				return FunctionSignature();
			}
		}

		// Check to make sure the function has at least one non-optional keyword part
		if (parsedNameCount == optionalNameCount)
		{
			Error("Function signature must have at least one non-optional name part");
			return FunctionSignature();
		}

		// Create the function signature
		FunctionSignature signature(scope, m_library->GetName(), signatureParts);

		// This flag indicates that we're not generating bytecode, so no need to output that data.
		if (signatureOnly)
		{
			// Emit function definition opcode
			EmitOpcode(Opcode::Function);
			signature.Write(m_writer);
		}

		return signature;
	}

	inline_t void Parser::ParseFunctionDefinition(VisibilityType scope)
	{
		// Check to make sure we're at the root frame
		if (!m_variableStackFrame.IsRootFrame())
		{
			Error("Can't define a function inside another class or function");
			return;
		}

		// Check to make sure we're at the root scope
		if (!m_variableStackFrame.IsRootScope())
		{
			Error("Can't define a function inside a scoped execution block");
			return;
		}

		// Parse function signature
		FunctionSignature signature = ParseFunctionSignature(scope);
		if (!signature.IsValid())
		{
			Error("Invalid function definition");
			return;
		}

		// Check function scope type
		if (signature.GetVisibility() == VisibilityType::Local)
		{
			// Register function signature for local scope only
			auto itr = std::find(m_localFunctions.begin(), m_localFunctions.end(), signature);
			if (itr != m_localFunctions.end())
			{
				Error("Function already defined in script %s", m_library->GetName().c_str());
				return;
			}
			m_localFunctions.push_back(signature);
		}
		else
		{
			// Register function signature in library
			if (m_library->FunctionSignatureExists(signature))
			{
				Error("Function already defined in library %s", m_library->GetName().c_str());
				return;
			}
			m_library->RegisterFunctionSignature(signature);
		}

		// During initial execution, jump over code body
		EmitOpcode(Opcode::Jump);
		auto jumpBackfillAddress = EmitAddressPlaceholder();

		// Mark beginning of new execution frame
		FrameBegin();

		// Get function parameters
		FunctionSignatureParts params = signature.GetParameters();

		// We're indexing from the top of the stack.
		int32_t stackIndex = -1;

		// Assign parameter names expected on the call stack.  We assign them in reverse order since
		// they were pushed on the stack in order.
		for (FunctionSignatureParts::reverse_iterator itr = params.rbegin(); itr != params.rend(); ++itr)
		{

			VariableAssign(itr->names.front());
			EmitOpcode(Opcode::SetIndex);
			EmitId(VariableNameToRuntimeID(itr->names.front()));
			EmitIndex(stackIndex);
			EmitValueType(itr->valueType);
			--stackIndex;
		}

		// Parse the function body
		bool returnedValue = false;
		while (!Check(SymbolType::End) && !m_error)
		{
			if (ParseStatement())
				returnedValue = true;
		}
		Expect(SymbolType::End);
		Expect(SymbolType::NewLine);

		// Check to make sure we've returned a value as expected.
		if (!returnedValue)
		{
			EmitOpcode(Opcode::PushVal);
			EmitValue(nullptr);
			EmitOpcode(Opcode::Return);
		}

		// Backfill jump destination 
		EmitAddressBackfill(jumpBackfillAddress);

		// Mark end of execution frame
		FrameEnd();
	}

	inline_t void Parser::ParseFunctionCall(const FunctionMatch & match)
	{
		assert(match.signature);

		auto libName = CheckLibraryName();
		if (!libName.empty())
			NextSymbol();

		// Parse function components according to match data
		for (size_t i = 0; i < match.partData.size(); ++i)
		{
			if (std::get<0>(match.partData[i]) == FunctionSignaturePartType::Name)
			{
				// Check to make sure this isn't a skipped optional name part
				if (std::get<1>(match.partData[i]) != 0)
					NextSymbol();
			}
			else
			{
				if (Accept(SymbolType::ParenOpen))
				{
					ParseExpression();
					Expect(SymbolType::ParenClose);
				}
				else
				{
					auto expressionSize = std::get<1>(match.partData[i]);
					auto endSymbol = m_currentSymbol;
					for (size_t j = 0; j < expressionSize; ++j)
						++endSymbol;
					ParseExpression(endSymbol);
				}
			}
		}

		// When finished validating the function and pushing parameters, call the function
		EmitOpcode(Opcode::CallFunc);
		EmitId(match.signature->GetId());
		m_idNameMap[match.signature->GetId()] = match.signature->GetName();

		// Check for post-function index operator
		ParseSubscriptGet();
	}

	inline_t void Parser::ParseCast()
	{
		EmitOpcode(Opcode::Cast);
		auto valueType = ParseValueType();
		EmitValueType(valueType);
	}

	inline_t void Parser::ParseSubexpressionOperand(bool required, SymbolListCItr endSymbol)
	{
		if (m_error)
			return;

		if (Check(SymbolType::Comma) || Check(SymbolType::ParenClose) || Check(SymbolType::SquareClose) || Check(SymbolType::To) || Check(SymbolType::By))
		{
			if (required)
				Error("Expected operand");
			return;
		}
		else
		{
			bool negation = Accept(SymbolType::Minus);
			FunctionMatch functionMatch = CheckFunctionCall(false, endSymbol);
			if (functionMatch.signature)
			{
				ParseFunctionCall(functionMatch);
			}
			else if (Accept(SymbolType::ParenOpen))
			{
				ParseExpression();
				Expect(SymbolType::ParenClose);
			}
			else if (CheckProperty())
			{
				auto propertyName = ParsePropertyName();
				if (!propertyName.IsValid())
				{
					Error("Unable to find property name in library");
					return;
				}
				EmitOpcode(Opcode::PushProp);
				EmitId(propertyName.GetId());
				m_idNameMap[propertyName.GetId()] = propertyName.GetName();
				ParseSubscriptGet();
				if (Accept(SymbolType::Type))
					EmitOpcode(Opcode::Type);
			}
			else if (CheckVariable())
			{
				String name = ParseVariable();
				EmitOpcode(Opcode::PushVar);
				EmitId(VariableNameToRuntimeID(name));
				ParseSubscriptGet();
				if (Accept(SymbolType::Type))
					EmitOpcode(Opcode::Type);
			}
			else if (CheckValue())
			{
				auto val = ParseValue();
				EmitOpcode(Opcode::PushVal);
				EmitValue(val);
			}
			else if (CheckValueType())
			{
				auto val = ParseValueType();
				EmitOpcode(Opcode::PushVal);
				EmitValue(val);
			}
			else
			{
				// If we can't find a valid function call within this end symbol, try
				// a match without those limits.  This gives priority to initial expressions,
				// but still allows functions to parse correctly if no valid expressions
				// otherwise exist.
				endSymbol = m_symbolList.end();
				functionMatch = CheckFunctionCall(false, endSymbol);
				if (functionMatch.signature)
				{
					ParseFunctionCall(functionMatch);
				}
				else
				{
					Error("Expected operand");
				}
			}
			if (negation)
				EmitOpcode(Opcode::Negate);
		}
	}

	inline_t void Parser::ParseSubexpression(SymbolListCItr endSymbol)
	{
		if (m_error)
			return;

		// Make sure we have a valid expression
		if (Check(SymbolType::NewLine))
		{
			Error("Expected valid expression");
			return;
		}

		// Opcode stack for operators 
		std::vector<Opcode, Allocator<Opcode>> opcodeStack;

		bool requiredOperand = false;
		std::vector<size_t, Allocator<size_t>> jumpAddrStack;

		bool notOp = false;
		while (IsSymbolValid(m_currentSymbol) && m_currentSymbol->type != SymbolType::NewLine && m_currentSymbol != endSymbol)
		{
			// Check for a unary negation operator
			if (Accept(SymbolType::Not))
			{
				notOp = !notOp;
				if (Check(SymbolType::Not))
				{
					Error("More than one consecutive not operator is not permitted");
					return;
				}
			}

			// Parse operand
			ParseSubexpressionOperand(requiredOperand, endSymbol);
			requiredOperand = false;

			// Check for casts
			if (Accept(SymbolType::As))
				ParseCast();

			// Parse binary operator
			if (CheckBinaryOperator())
			{
				requiredOperand = true;
				auto opcode = ParseBinaryOperator();

				// Check precedence if we've already parsed a binary math expression
				while (!opcodeStack.empty() && GetOperatorPrecedence(opcode) >= GetOperatorPrecedence(opcodeStack.back()))
				{
					EmitOpcode(opcodeStack.back());
					opcodeStack.pop_back();
				}

				// Emit short-circuit evaluation jump check with placeholder address
				if (opcode == Opcode::And || opcode == Opcode::Or)
				{
					// Emit not opcode if required
					if (notOp)
					{
						EmitOpcode(Opcode::Not);
						notOp = false;
					}

					EmitOpcode(opcode == Opcode::And ? Opcode::JumpFalseCheck : Opcode::JumpTrueCheck);
					jumpAddrStack.push_back(EmitAddressPlaceholder());
				}

				opcodeStack.push_back(opcode);
			}
			else if (!opcodeStack.empty())
			{
				while (!opcodeStack.empty())
				{
					EmitOpcode(opcodeStack.back());
					opcodeStack.pop_back();
				}
			}
			else
			{
				break;
			}
		}

		// Emit not opcode if required
		if (notOp)
			EmitOpcode(Opcode::Not);

		// Check for chained function calls
		while (true)
		{
			const auto match = CheckFunctionCall(true, endSymbol);
			if (match.signature)
			{
				if (match.signature->GetParts()[0].partType != FunctionSignaturePartType::Parameter)
				{
					Error("Missing operator before function '%s'", match.signature->GetName().c_str());
					return;
				}
				ParseFunctionCall(match);
			}
			else
				break;
		}

		// Backfill any short-circuit test jump address now that we're finished with local expression
		while (!jumpAddrStack.empty())
		{
			EmitAddressBackfill(jumpAddrStack.back());
			jumpAddrStack.pop_back();
		}

		// Check for leftover operators
		if (!opcodeStack.empty())
		{
			Error("Syntax error when parsing expression");
		}

	}

	inline_t void Parser::ParseSubexpression()
	{
		ParseSubexpression(m_symbolList.end());
	}

	inline_t void Parser::ParseExpression(SymbolListCItr endSymbol)
	{
		// Check first for an opening bracket, which indicates either an index operator or a key-value pair.
		if (Accept(SymbolType::SquareOpen))
		{
			if (Accept(SymbolType::SquareClose))
			{
				// If we immediately see a close bracket, create an empty collection
				EmitOpcode(Opcode::PushColl);
				EmitCount(0);
			}
			else
			{
				ParseSubexpression(endSymbol);

				// If we see a comma after a square open bracket, we're parsing a key-value pair
				if (Accept(SymbolType::Comma))
				{
					ParseExpression(endSymbol);
					Expect(SymbolType::SquareClose);

					// Parse all subsequent key-value pairs
					uint32_t count = 1;
					while (Accept(SymbolType::Comma))
					{
						Accept(SymbolType::NewLine);
						Expect(SymbolType::SquareOpen);
						ParseSubexpression(endSymbol);
						Expect(SymbolType::Comma);
						ParseSubexpression(endSymbol);
						Expect(SymbolType::SquareClose);
						++count;
					}

					// Pop all key-value pairs and push a new collection onto the stack
					EmitOpcode(Opcode::PushColl);
					EmitCount(count);
				}
				else
				{
					Error("Expected comma separating key-value pair");
				}
			}
		}
		else
		{
			// Parse the first subexpression, defined as any normal expression excluding index operators or lists, 
			// which are handled in this function
			ParseSubexpression(endSymbol);

			// If we finish the first subexpression with a common, then we're parsing an indexed list
			if (Accept(SymbolType::Comma))
			{
				// Parse all subexpressions in comma-delimited list
				uint32_t count = 1;
				do
				{
					Accept(SymbolType::NewLine);
					ParseSubexpression(endSymbol);
					++count;
				} 
				while (Accept(SymbolType::Comma));

				// Pop all key-value pairs and push the results on the stack
				EmitOpcode(Opcode::PushList);
				EmitCount(count);
			}
		}
	}

	inline_t void Parser::ParseExpression()
	{
		ParseExpression(m_symbolList.end());
	}

	inline_t void Parser::ParseErase()
	{
		if (CheckProperty())
		{
			auto propName = ParsePropertyName();
			if (propName.IsReadOnly())
			{
				Error("Can't erase a readonly property");
				return;
			}
			uint32_t subscripts = ParseSubscriptSet();
			Expect(SymbolType::NewLine);
			if (subscripts)
			{
				EmitOpcode(Opcode::ErasePropKeyVal);
				EmitCount(subscripts);
				EmitId(propName.GetId());
			}
			else
			{
				Error("Expected index operator after erase keyword");
				return;
			}
		}
		else if (CheckVariable())
		{
			auto varName = ParseVariable();
			uint32_t subscripts = ParseSubscriptSet();
			Expect(SymbolType::NewLine);
			if (subscripts)
			{
				EmitOpcode(Opcode::EraseVarKeyVal);
				EmitCount(subscripts);
				EmitId(VariableNameToRuntimeID(varName));
			}
			else
			{
				EmitOpcode(Opcode::EraseItr);
				EmitId(VariableNameToRuntimeID(varName));
			}
		}
		else
		{
			Error("Valid property or variable name expected after erase keyword");
			return;
		}
	}

	inline_t void Parser::ParseIncDec()
	{
		bool increment = Accept(SymbolType::Increment);
		if (!increment)
			Expect(SymbolType::Decrement);
		PropertyName propName;
		String varName;
		if (CheckProperty())
		{
			propName = ParsePropertyName();
			if (propName.IsReadOnly())
			{
				Error("Can't %s a readonly property", increment ? "increment" : "decrement");
				return;
			}
			EmitOpcode(Opcode::PushProp);
			EmitId(propName.GetId());
			m_idNameMap[propName.GetId()] = propName.GetName();
		}
		else if (CheckVariable())
		{
			varName = ParseVariable();
			EmitOpcode(Opcode::PushVar);
			EmitId(VariableNameToRuntimeID(varName));
		}
		else
		{
			Error("Valid property or variable name expected after %s keyword", increment ? "increment" : "decrement");
			return;
		}
		if (Accept(SymbolType::By))
		{
			ParseExpression();
		}
		else
		{
			EmitOpcode(Opcode::PushVal);
			EmitValue(1);
		}
		EmitOpcode(increment ? Opcode::Increment : Opcode::Decrement);
		if (!propName.GetName().empty())
		{
			EmitOpcode(Opcode::SetProp);
			EmitId(propName.GetId());
			m_idNameMap[propName.GetId()] = propName.GetName();
		}
		else
		{
			EmitOpcode(Opcode::SetVar);
			EmitId(VariableNameToRuntimeID(varName));
		}
		Expect(SymbolType::NewLine);
	}

	inline_t void Parser::ParseIfElse()
	{
		// Parse expression after the if keyword
		ParseExpression();
		Expect(SymbolType::NewLine);

		// Add jump instruction, making sure to store the jump address for later backfilling
		EmitOpcode(Opcode::JumpFalse);
		auto ifJumpAddress = EmitAddressPlaceholder();

		// Parse new block of code after if line
		ParseBlock();

		// Check to see if we continue with 'else' or 'end' the if block
		if (Accept(SymbolType::Else))
		{
			// Set a jump statement before the else for the end of the if-true execution block
			EmitOpcode(Opcode::Jump);
			auto elseJumpAddress = EmitAddressPlaceholder();

			// Backfill jump target address after we've reserved space for a new jump
			EmitAddressBackfill(ifJumpAddress);

			// We should now see either an endline or another if symbol.
			// If we see an endline, then we just have an else block
			if (Accept(SymbolType::NewLine))
			{
				// Parse the 'else' block
				ParseBlock();

				// Check that the block ends properly
				Expect(SymbolType::End);
				Expect(SymbolType::NewLine);
			}
			else if (Accept(SymbolType::If))
			{
				ParseIfElse();
			}
			else
			{
				Error("Unexpected symbol after else");
			}

			// Backfill else jump
			EmitAddressBackfill(elseJumpAddress);
		}
		else if (Accept(SymbolType::End))
		{
			// We're expecting a newline after the 'end' keyword
			Expect(SymbolType::NewLine);

			// Backfill if-true jump target address
			EmitAddressBackfill(ifJumpAddress);
		}
		else
		{
			Error("Missing block termination after if");
		}
	}

	inline_t void Parser::ParseLoop()
	{
		// Check to see if we're using an explicitly named variable for the loop counter
		String name;
		if (CheckName())
		{
			// Parse initial name part
			name = ParseMultiName({ SymbolType::From, SymbolType::Over, SymbolType::Until, SymbolType::While });

			// Make sure the variable name doesn't already exist
			if (VariableExists(name))
			{
				Error("Variable name '%s' already exists in this scope", name.c_str());
				return;
			}
		}

		// We're looping using a counter
		if (Accept(SymbolType::From))
		{
			// Begin scope for loop control variables
			ScopeBegin();

			// Parse from value
			ParseExpression();

			// Assign the counter to a variable name if it exists
			if (!name.empty())
			{
				VariableAssign(name);
				EmitOpcode(Opcode::SetVar);
				EmitId(VariableNameToRuntimeID(name));
			}

			// Parse to value
			Expect(SymbolType::To);
			ParseExpression();

			// Parse increment amount
			if (Accept(SymbolType::By))
			{
				ParseExpression();
			}
			else
			{
				EmitOpcode(Opcode::PushVal);
				EmitValue(nullptr);
			}
			Expect(SymbolType::NewLine);

			// Mark where the loop count evaluation has to jump
			auto loopBeginAddress = m_writer.Tell();

			// Evaluate code block inside loop
			ParseBlock();
			Expect(SymbolType::End);
			Expect(SymbolType::NewLine);

			// Advance counter and evaluate
			EmitOpcode(Opcode::LoopCount);

			// Evaluate result of loop count instruction
			EmitOpcode(Opcode::JumpTrue);
			EmitAddress(loopBeginAddress);

			// End loop scope
			ScopeEnd();
		}
		// We're looping over a collection
		else if (Accept(SymbolType::Over))
		{
			// Begin scope for loop control variables
			ScopeBegin();

			// Parse the collection
			ParseExpression();
			if (!Expect(SymbolType::NewLine))
				return;

			// Check to see if the collection is empty, and if so, skip the loop
			EmitOpcode(Opcode::PushTop);
			EmitOpcode(Opcode::JumpFalse);
			auto emptyLoopJumpAddress = EmitAddressPlaceholder();

			// Retrieve collection from top of stack and push iterator from beginning position
			EmitOpcode(Opcode::PushItr);

			// Assign the iterator to a variable name if it exists
			if (!name.empty())
			{
				VariableAssign(name);
				EmitOpcode(Opcode::SetVar);
				EmitId(VariableNameToRuntimeID(name));
			}

			// Store where the loop logic begins
			auto loopBeginAddress = m_writer.Tell();

			// Parse the while loop block
			ParseBlock();
			Expect(SymbolType::End);
			Expect(SymbolType::NewLine);

			// Increment iterator and test against collection end
			EmitOpcode(Opcode::LoopOver);
			EmitOpcode(Opcode::JumpFalse);
			EmitAddress(loopBeginAddress);

			// Backfill empty loop jump address
			EmitAddressBackfill(emptyLoopJumpAddress);

			// End loop scope
			ScopeEnd();

		}
		// Loops while a condition is true or false
		else if (Check(SymbolType::Until) || Check(SymbolType::While))
		{
			// Store where the loop logic begins
			auto loopBeginAddress = m_writer.Tell();

			// Check for keyword
			bool jumpTrue = Accept(SymbolType::Until);
			if (!jumpTrue)
				Expect(SymbolType::While);

			// Parse the expression to control the loop's jump branch
			ParseExpression();
			if (!Expect(SymbolType::NewLine))
				return;

			// Add jump instruction, making sure to store the jump address for later backfilling
			EmitOpcode(jumpTrue ? Opcode::JumpTrue : Opcode::JumpFalse);
			auto loopJumpAddress = EmitAddressPlaceholder();

			// Parse the while loop block
			ParseBlock();
			Expect(SymbolType::End);
			Expect(SymbolType::NewLine);

			// Jump to top of loop
			EmitOpcode(Opcode::Jump);
			EmitAddress(loopBeginAddress);

			// Backfill loop jump target address
			EmitAddressBackfill(loopJumpAddress);
		}
		// Executes once and then loops again while a condition is true/false, depending on keyword used
		else if (Accept(SymbolType::NewLine))
		{
			// Store where the loop logic begins
			auto loopBeginAddress = m_writer.Tell();

			// Parse the until/while loop block
			ParseBlock();

			// Check the keyword is used
			bool jumpTrue = Accept(SymbolType::While);
			if (!jumpTrue)
				Expect(SymbolType::Until);

			// Parse expression used to determine if loop should execute again
			ParseExpression();
			Expect(SymbolType::NewLine);

			// Conditionally jump to top of loop
			EmitOpcode(jumpTrue ? Opcode::JumpTrue : Opcode::JumpFalse);
			EmitAddress(loopBeginAddress);
		}
		else
		{
			Error("Unknown syntax after loop keyword");
			return;
		}

		// If we used a break somewhere inside the loop, backfill the address now
		if (m_breakAddress)
		{
			EmitAddressBackfill(m_breakAddress);
			m_breakAddress = 0;
		}
	}

	inline_t bool Parser::ParseStatement()
	{
		// No need to continue if an error has been flagged
		if (m_error)
			return false;

		bool returnedValue = false;

		// Functions signatures have precedence over everything, so check for a 
		// potential signature match before anything else.
		const auto functionMatch = CheckFunctionCall();
		if (functionMatch.signature)
		{
			// We found a valid function signature that matches the current token(s)
			ParseFunctionCall(functionMatch);

			// Since all functions return a value, we need to discard the return
			// value not on the stack, since we're not assigning it to a variable.
			EmitOpcode(Opcode::Pop);
			Expect(SymbolType::NewLine);
		}
		else
		{
			bool set = Accept(SymbolType::Set);

			// Parse scope level
			VisibilityType scope = ParseScope();

			// Parse optional readonly, which can only apply to properties
			bool readOnly = Accept(SymbolType::Readonly);
			if (readOnly)
			{
				if (scope == VisibilityType::Local)
				{
					Error("The 'readonly' keyword must follow a private or public keyword");
					return false;
				}
			}

			if (Accept(SymbolType::Function))
			{
				// We're parsing a function definition
				ParseFunctionDefinition(scope);
			}
			else if (set && CheckName())
			{
				// Can't use the current library name or preface the variable with it
				if (m_currentSymbol->text == m_library->GetName())
				{
					Error("Illegal use of library name in identifier");
					return false;
				}

				// We're declaring a new property if we see a non-local scope declaration
				if (scope != VisibilityType::Local)
				{
					ParsePropertyDeclaration(scope, readOnly);
				}
				// Either this is an existing property or a variable
				else
				{
					// Check to see if this is an existing property
					if (CheckProperty())
					{
						// Get the property name
						auto propertyName = ParsePropertyName();

						// Make sure we're not trying to assign a value to a readonly property
						if (propertyName.IsReadOnly())
						{
							Error("Can't change readonly property");
							return false;
						}

						// Check for subscript operators
						uint32_t subscripts = ParseSubscriptSet();

						// Check for a 'to' statement
						Expect(SymbolType::To);

						// Parse assignment expression
						ParseExpression();
						Expect(SymbolType::NewLine);

						// Assign property
						if (subscripts)
						{
							EmitOpcode(Opcode::SetPropKeyVal);
							EmitCount(subscripts);
						}
						else
							EmitOpcode(Opcode::SetProp);
						EmitId(propertyName.GetId());
						m_idNameMap[propertyName.GetId()] = propertyName.GetName();
					}
					// Otherwise we're just dealing with an ordinary variable
					else
					{
						// Get the variable name
						String name = ParseMultiName({ SymbolType::To, SymbolType::SquareOpen });

						// Check for subscript operator
						uint32_t subscripts = ParseSubscriptSet();

						// Check for a 'to' statement
						Expect(SymbolType::To);

						// Parse assignment expression
						ParseExpression();
						Expect(SymbolType::NewLine);

						// Add to variable table
						VariableAssign(name);

						// Assign a variable. 
						if (subscripts)
						{
							EmitOpcode(Opcode::SetVarKeyVal);
							EmitCount(subscripts);
						}
						else
							EmitOpcode(Opcode::SetVar);
						EmitId(VariableNameToRuntimeID(name));
					}
				}
			}
			else if (scope == VisibilityType::Local)
			{
				if (Accept(SymbolType::Begin))
				{
					// We're parsing a begin/end block
					Expect(SymbolType::NewLine);
					ParseBlock();
					Expect(SymbolType::End);
					Expect(SymbolType::NewLine);
				}
				else if (Accept(SymbolType::If))
				{
					// We're parsing an if or if/else block
					ParseIfElse();
				}
				else if (Accept(SymbolType::Loop))
				{
					// We're parsing a loop block
					ParseLoop();
				}
				else if (Accept(SymbolType::Erase))
				{
					// We're parsing an erase operation
					ParseErase();
				}
				else if (Check(SymbolType::Increment) || Check(SymbolType::Decrement))
				{
					// We're parsing an increment or decrement statement
					ParseIncDec();
				}
				else if (Accept(SymbolType::Return))
				{
					// We've hit a return value.  There are different behaviors depending whether or
					// not we're at the base scope or not.
					if (m_variableStackFrame.IsRootFrame())
					{
						if (!Check(SymbolType::NewLine))
						{
							LogWriteLine(LogLevel::Warning, "Return values at root scope do nothing");
							ParseExpression();
						}
						EmitOpcode(Opcode::Exit);
						Accept(SymbolType::NewLine);
						returnedValue = true;
					}
					else
					{
						if (!Check(SymbolType::NewLine))
						{
							ParseExpression();
						}
						else
						{
							EmitOpcode(Opcode::PushVal);
							EmitValue(nullptr);
						}
						EmitOpcode(Opcode::Return);
						Accept(SymbolType::NewLine);
						returnedValue = true;
					}
				}
				else if (Accept(SymbolType::Break))
				{
					// We've hit a break statement
					Expect(SymbolType::NewLine);
					EmitOpcode(Opcode::Jump);
					m_breakAddress = EmitAddressPlaceholder();
				}
				else if (Accept(SymbolType::Wait))
				{
					// Check for basic wait statement
					if (Accept(SymbolType::NewLine))
						EmitOpcode(Opcode::Wait);
					else if (Check(SymbolType::Until) || Check(SymbolType::While))
					{
						// Store expression address
						auto expressionAddress = m_writer.Tell();

						// Check which keyword was used
						bool jumpTrue = Accept(SymbolType::Until);
						if (!jumpTrue)
							Expect(SymbolType::While);

						// Parse the expression to check for wait
						ParseExpression();
						if (!Expect(SymbolType::NewLine))
							return false;

						// Add jump if false/true over wait statement depending on keyword
						EmitOpcode(jumpTrue ? Opcode::JumpTrue : Opcode::JumpFalse);

						// Mark placeholder for later jump address insertion
						auto addressPlaceholder = EmitAddressPlaceholder();

						// Wait statement is executed if expression is true
						EmitOpcode(Opcode::Wait);
						EmitOpcode(Opcode::Jump);
						EmitAddress(expressionAddress);

						// Backfill placeholder at end of conditional wait statement
						EmitAddressBackfill(addressPlaceholder);
					}
					else
					{
						Error("Unexpected symbol after wait");
					}
				}
				else if (Accept(SymbolType::External))
				{
					// First check to see if this collides with an existing property name
					bool propExists = CheckProperty();

					// Get the variable name
					String name = ParseMultiName({ });

					// Validate the name is legal and register it as a variable name
					if (!m_variableStackFrame.IsRootFrame())
						Error("External variable '%s' can't be declared in a function", name.c_str());
					else if (!m_variableStackFrame.IsRootScope())
						Error("External variable '%s' must be declared at the root scope", name.c_str());
					else if (propExists)
						Error("External variable '%s' is already a property name", name.c_str());
					else if (m_variableStackFrame.VariableExists(name))
						Error("Variable '%s' already exists", name.c_str());
					else if (!m_variableStackFrame.VariableAssign(name))
						Error("Error creating external variable '%s'", name.c_str());

					Expect(SymbolType::NewLine);
				}
				else
				{
					Error("Unknown symbol in statement");
				}
			}
			else
			{
				Error("Invalid symbol after scope specifier '%s'", scope == VisibilityType::Public ? "public" : "private");
			}
		}

		// This is not an error value, but signals whether we've returned a value from this statement
		return returnedValue;
	}

	inline_t void Parser::ParseBlock()
	{
		if (m_error)
			return;

		// Push local variable stack frame
		ScopeBegin();

		// Parse each statement until we reach a terminating symbol
		while (!(Check(SymbolType::End) || Check(SymbolType::Else) || Check(SymbolType::Until) || Check(SymbolType::While)) && !m_error)
			ParseStatement();

		// Pop local variable stack frame
		ScopeEnd();
	}

	inline_t void Parser::ParseLibraryImports()
	{
		while (true)
		{
			if (!Accept(SymbolType::Import))
				break;
			auto name = ParseName();
			if (name.empty())
			{
				Error("Expected valid name after 'import' keyword");
				return;
			}
			Expect(SymbolType::NewLine);

			// Check to make sure we're not adding duplicates
			bool foundDup = false;
			for (auto & importName : m_importList)
			{
				if (importName == name)
				{
					foundDup = true;
					continue;
				}
			}

			// Add library to the list of imported libraries for this script
			if (!foundDup)
			{
				m_importList.push_back(name);
				if (!m_runtime->LibraryExists(name))
				{
					Error("Could not find library named '%s'", name.c_str());
					break;
				}
			}
		}
	}

	inline_t void Parser::ParseLibraryDeclaration()
	{
		if (m_library != nullptr)
		{
			Error("Library has already been declared for this script");
			return;
		}
		String libraryName;
		if (Accept(SymbolType::Library))
		{
			libraryName = ParseName();
			if (libraryName.empty())
			{
				Error("Expected valid name after 'library' keyword");
				return;
			}
			Expect(SymbolType::NewLine);
			m_library = m_runtime->GetLibraryInternal(libraryName);
		}

		// Emit library declaration bytecode
		EmitOpcode(Opcode::Library);
		EmitName(libraryName);

		// Retrieve library interface by name
		m_library = m_runtime->GetLibraryInternal(libraryName);
	}

	inline_t void Parser::ParseScript()
	{
		ParseLibraryImports();
		ParseLibraryDeclaration();
		while (m_currentSymbol != m_symbolList.end() && !m_error)
			ParseStatement();
		if (m_breakAddress)
			Error("Illegal break");
		EmitOpcode(Opcode::Exit);
		WriteBytecodeHeader();
		if (EnableDebugInfo())
			WriteDebugInfo();
	}

} // namespace Jinx::Impl

