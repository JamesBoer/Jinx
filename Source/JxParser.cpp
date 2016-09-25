/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

using namespace Jinx;


Parser::Parser(RuntimeIPtr runtime, const SymbolList & symbolList, const String & uniqueName, std::initializer_list<String> libraries) :
	m_runtime(runtime),
	m_uniqueName(uniqueName),
	m_symbolList(symbolList),
	m_error(false),
	m_breakAddress(false),
	m_bytecode(CreateBuffer()),
	m_writer(m_bytecode),
	m_requireReturnValue(false),
	m_returnedValue(false)
{
	m_currentSymbol = symbolList.begin();
	m_importList = libraries;
}

bool Parser::Execute()
{
	m_variableFrame.clear();
	m_variableFrame.push_back(VariableStack());
	m_variableFrame.back().push_back(VariableSet());

	// Reserve 1K space
	m_bytecode->Reserve(1024);

	// Write bytecode header
	BytecodeHeader header;
	m_writer.Write(&header, sizeof(header));

	// Parse script symbols into bytecode
	ParseScript();

	// Return error status
	return !m_error;
}

void Parser::VariableAssign(const String & name)
{
	// Sanity check
	if (m_variableFrame.empty())
	{
		Error("Unexpected empty variable frame");
		return;
	}

	// Retrieve the current variable stack
	VariableStack & variableStack = m_variableFrame.back();
	if (variableStack.empty())
	{
		Error("Attempting to assign a variable to an empty stack");
		return;
	}

	// Work from the top of the stack down, and attempt to find an existing name
	for (auto ritr = variableStack.rbegin(); ritr != variableStack.rend(); ++ritr)
	{
		const auto & itr = ritr->find(name);
		if (itr != ritr->end())
			return;
	}

	// If we don't find the name, create a new variable on the top of the stack
	auto & map = variableStack.back();
	map.insert(name);
}

bool Parser::VariableExists(const String & name) const
{
	// Sanity check
	if (m_variableFrame.empty())
		return false;

	const VariableStack & variableStack = m_variableFrame.back();
	for (auto ritr = variableStack.rbegin(); ritr != variableStack.rend(); ++ritr)
	{
		auto itr = ritr->find(name);
		if (itr != ritr->end())
			return true;
	}
	return false;
}

void Parser::FrameBegin()
{
	m_variableFrame.push_back(VariableStack());
	m_variableFrame.back().push_back(VariableSet());
}

void Parser::FrameEnd()
{
	if (m_variableFrame.empty())
	{
		Error("Attempted to pop empty variable frame");
		return;
	}
	m_variableFrame.pop_back();
}

void Parser::ScopeBegin()
{
	if (m_variableFrame.empty())
	{
		Error("Unexpected empty variable frame");
		return;
	}
	VariableStack & variableStack = m_variableFrame.back();
	variableStack.push_back(VariableSet());
	EmitOpcode(Opcode::ScopeBegin);
}

void Parser::ScopeEnd()
{
	if (m_variableFrame.empty())
	{
		Error("Unexpected empty variable frame");
		return;
	}
	VariableStack & variableStack = m_variableFrame.back();
	if (variableStack.empty())
	{
		LogWriteLine("Attempted to pop empty variable stack");
		return;
	}
	variableStack.pop_back();
	EmitOpcode(Opcode::ScopeEnd);
}

bool Parser::IsRootScope() const
{
	return (m_variableFrame.back().size() == 1) ? true : false;
}

bool Parser::IsRootFrame() const
{
	return (m_variableFrame.size() == 1) ? true : false;
}

bool Parser::IsSymbolValid(SymbolListCItr symbol) const
{
	if (m_error)
		return false;
	if (symbol == m_symbolList.end())
		return false;
	if (symbol->type == SymbolType::NewLine)
		return false;
	return true;
}

bool Parser::IsLibraryName(const String & name) const
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

bool Parser::IsPropertyName(const String & libraryName, const String & propertyName) const
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

void Parser::EmitAddress(size_t address)
{
	m_writer.Write(uint32_t(address));
}

size_t Parser::EmitAddressPlaceholder()
{
	size_t offset = m_writer.Tell();
	m_writer.Write(uint32_t(0));
	return offset;
}

void Parser::EmitAddressBackfill(size_t address)
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

void Parser::EmitCount(uint32_t count)
{
	m_writer.Write(count);
}

void Parser::EmitName(const String & name)
{
	m_writer.Write(name);
}

void Parser::EmitOpcode(Opcode opcode)
{
	m_writer.Write<Opcode, uint8_t>(opcode);
}

void Parser::EmitValue(const Variant & value)
{
	value.Write(m_writer);
}

void Parser::EmitId(RuntimeID id)
{
	m_writer.Write(id);
}

void Parser::EmitIndex(int32_t index)
{
	m_writer.Write(index);
}

void Parser::EmitValueType(ValueType valueType)
{
	m_writer.Write(ValueTypeToByte(valueType));
}

void Parser::NextSymbol()
{
	++m_currentSymbol;
}

bool Parser::Accept(SymbolType symbol)
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

bool Parser::Expect(SymbolType symbol)
{
	if (Accept(symbol))
		return true;
	Error("Expected symbol %s", GetSymbolTypeText(symbol));
	return false;
}

bool Parser::Check(SymbolType symbol) const
{
	if (m_error || m_currentSymbol == m_symbolList.end())
		return false;
	return (symbol == m_currentSymbol->type);
}

bool Parser::CheckLogicalOperator() const
{
	if (m_error || m_currentSymbol == m_symbolList.end())
		return false;
	auto type = m_currentSymbol->type;
	return 
		type == SymbolType::And ||
		type == SymbolType::Or ||
		type == SymbolType::Not;
}

bool Parser::CheckBinaryOperator() const
{
	if (m_error || m_currentSymbol == m_symbolList.end())
		return false;
	auto type = m_currentSymbol->type;
	return 
		type == SymbolType::Asterisk ||
		type == SymbolType::Equals ||
		type == SymbolType::NotEquals ||
		type == SymbolType::ForwardSlash ||
		type == SymbolType::GreaterThan ||
		type == SymbolType::GreaterThanEquals ||
		type == SymbolType::LessThan ||
		type == SymbolType::LessThanEquals ||
		type == SymbolType::Minus ||
		type == SymbolType::Percent ||
		type == SymbolType::Plus;
}

bool Parser::CheckName() const
{
	if (m_error || m_currentSymbol == m_symbolList.end())
		return false;
	return m_currentSymbol->type == SymbolType::NameValue;
}

bool Parser::CheckValue() const
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

bool Parser::CheckValueType() const
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

bool Parser::CheckFunctionNamePart() const
{
	if (m_error || m_currentSymbol == m_symbolList.end())
		return false;
	return (m_currentSymbol->type == SymbolType::NameValue) || IsKeyword(m_currentSymbol->type);
}

String Parser::CheckLibraryName() const
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

const FunctionSignature * Parser::CheckFunctionCall() const
{
	// Store current symbol
	auto currentSymbol = m_currentSymbol;

	// Check for error or invalid symbols
	if (m_error || currentSymbol == m_symbolList.end())
		return nullptr;

	// Any operators other than open parentheses mean this can't be a function call
	if (IsOperator(currentSymbol->type) && (currentSymbol->type != SymbolType::ParenOpen))
		return nullptr;

	// Check for explicit library name in the first symbol.  libraryName is empty if not found.
	String libraryName = CheckLibraryName();

	// Advance the current symbol if we found a valid library name
	if (!libraryName.empty())
	{
		++currentSymbol;
		if (currentSymbol == m_symbolList.end())
			return nullptr;
	}

	// Create a signature parts list to match existing signatures against
	FunctionSignatureParts parts;

	// Add symbols to list until we hit a terminating symbol
	while (IsSymbolValid(currentSymbol))
	{
		int parenCount = 0;
		if (currentSymbol->type == SymbolType::NameValue || IsKeyword(currentSymbol->type))
		{
			String name = currentSymbol->text;
			FunctionSignaturePart part;
			if (IsLibraryName(name))
			{
				String libName = name;
				++currentSymbol;
				if (!IsSymbolValid(currentSymbol))
					return nullptr;
				name = currentSymbol->text;
				if (!IsPropertyName(libName, name))
					return nullptr;
				part.partType = FunctionSignaturePartType::Name;
			}
			else if (IsPropertyName("", name))
				part.partType = FunctionSignaturePartType::Parameter;
			else if (VariableExists(name))
				part.partType = FunctionSignaturePartType::Parameter;
			else
				part.partType = FunctionSignaturePartType::Name;
			part.names.push_back(name);
			parts.push_back(part);
		}
		else if (IsValue(currentSymbol->type))
		{
			FunctionSignaturePart part;
			part.partType = FunctionSignaturePartType::Parameter;
			parts.push_back(part);
		}
		else if (currentSymbol->type == SymbolType::ParenOpen)
		{
			++parenCount;
			while (parenCount)
			{
				++currentSymbol;
				if ((currentSymbol->type == SymbolType::NewLine) || (currentSymbol == m_symbolList.end()))
					return nullptr;
				if (currentSymbol->type == SymbolType::ParenClose)
					--parenCount;
				else if (currentSymbol->type == SymbolType::ParenOpen)
					++parenCount;
			}
			FunctionSignaturePart part;
			part.partType = FunctionSignaturePartType::Parameter;
			parts.push_back(part);
		}
		else if (currentSymbol->type == SymbolType::SquareOpen)
		{
			++parenCount;
			while (parenCount)
			{
				++currentSymbol;
				if ((currentSymbol->type == SymbolType::NewLine) || (currentSymbol == m_symbolList.end()))
					return nullptr;
				if (currentSymbol->type == SymbolType::SquareClose)
					--parenCount;
				else if (currentSymbol->type == SymbolType::SquareOpen)
					++parenCount;
			}
			FunctionSignaturePart part;
			part.partType = FunctionSignaturePartType::Parameter;
			parts.push_back(part);
		}
		else if (IsOperator(currentSymbol->type))
		{
			break;
		}
		++currentSymbol;
	}

	// Check possibility of a match with signature parts
	const FunctionSignature * functionSignature = nullptr;
	if (!parts.empty())
	{
		// If we explicitly specify a library name, then only look in that library
		if (!libraryName.empty())
		{
			auto library = m_runtime->GetLibraryInternal(libraryName);
			functionSignature = library->Functions().Find(parts);
		}
		else
		{
			// Check locally function table for signature match
			functionSignature = m_localFunctions.Find(parts);

			// If not found in local function table, search in libraries for a function match
			if (!functionSignature)
			{
				// Check the current library for a signature match
				functionSignature = m_library->Functions().Find(parts);

				// If a library name isn't specified or a signature wasn't found, search first in current library, then in order of imports
				if (!functionSignature)
				{
					// Search default library first
					auto library = m_runtime->GetLibraryInternal(libraryName);
					functionSignature = library->Functions().Find(parts);

					// If function wasn't found in default library, search through all import libraries
					if (!functionSignature)
					{
						// Loop through all imported library names
						for (const auto & libName : m_importList)
						{
							// Make sure the library exists
							if (!m_runtime->LibraryExists(libName))
							{
								LogWriteLine("Warning: Unable to find library '%s'", libName.c_str());
								continue;
							}

							// Search for function in this library
							library = m_runtime->GetLibraryInternal(libName);
							auto fnSig = library->Functions().Find(parts);
							if (fnSig)
							{
								if (functionSignature)
								{
									LogWriteLine("Warning: Ambiguous function name detected.  Use library name to disambiguate.");
									return nullptr;
								}
								else
								{
									functionSignature = fnSig;
									if (functionSignature->GetVisibility() == VisibilityType::Private && library != m_library)
									{
										LogWriteLine("Warning: Scope does not allow calling of library function");
										return nullptr;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return functionSignature;
}

bool Parser::CheckVariable() const
{
	if (m_error || m_currentSymbol == m_symbolList.end())
		return false;
	if (m_currentSymbol->type != SymbolType::NameValue)
		return false;
	return VariableExists(m_currentSymbol->text);
}

bool Parser::CheckProperty() const
{
	// Check symbol validity
	if (m_error || m_currentSymbol == m_symbolList.end())
		return false;
	if (m_currentSymbol->type != SymbolType::NameValue)
		return false;

	// Check to see if this begins with a library name
	String libraryName = CheckLibraryName();
	if (!libraryName.empty())
	{
		// Get next symbol and check validity
		auto currentSymbol = m_currentSymbol;
		++currentSymbol;
		if (currentSymbol == m_symbolList.end())
			return false;
		if (currentSymbol->type != SymbolType::NameValue)
			return false;

		// Check for property name in this specific library
		auto library = m_runtime->GetLibraryInternal(libraryName);
		assert(library);
		return library->PropertyNameExists(currentSymbol->text);
	}

	// Check for property name in the current library
	assert(m_library);
	if (m_library->PropertyNameExists(m_currentSymbol->text))
		return true;

	// Check against all imported libraries
	for (auto & importName : m_importList)
	{
		auto library = m_runtime->GetLibraryInternal(importName);
		if (library->PropertyNameExists(m_currentSymbol->text))
			return true;
	}
	return false;
}

VisibilityType Parser::ParseScope()
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

Opcode Parser::ParseLogicalOperator()
{
	if (m_error || m_currentSymbol == m_symbolList.end())
		return Opcode::NumOpcodes;

	Opcode opcode = Opcode::NumOpcodes;
	switch (m_currentSymbol->type)
	{
	case SymbolType::And:
		opcode = Opcode::And;
		break;
	case SymbolType::Or:
		opcode = Opcode::Or;
		break;
	case SymbolType::Not:
		opcode = Opcode::Not;
		break;
	default:
		Error("Unknown type when parsing condition keyword");
		break;
	}
	NextSymbol();
	return opcode;
}

Opcode Parser::ParseBinaryOperator()
{
	if (m_error || m_currentSymbol == m_symbolList.end())
		return Opcode::NumOpcodes;

	Opcode opcode = Opcode::NumOpcodes;
	switch (m_currentSymbol->type)
	{
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

Variant Parser::ParseValue()
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

ValueType Parser::ParseValueType()
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

String Parser::ParseName()
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

String Parser::ParseVariable()
{
	if (m_error || m_currentSymbol == m_symbolList.end())
		return String();
	if (m_currentSymbol->type != SymbolType::NameValue)
	{
		Error("Unexpected symbol type when parsing variable");
		return String();
	}
	String s = m_currentSymbol->text;
	NextSymbol();
	return s;
}

bool Parser::ParseSubscript()
{
	if (m_error || m_currentSymbol == m_symbolList.end())
		return false;
	if (!Accept(SymbolType::SquareOpen))
		return false;
	ParseExpression();
	return Expect(SymbolType::SquareClose);
}

void Parser::ParsePropertyDeclaration(bool readOnly, VisibilityType scope)
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

	// Make sure the property doesn't already exist
	if (CheckProperty())
	{
		Error("Property is already defined");
		return;
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
	auto name = ParseName();

	// Create a PropertyName object for registration
	PropertyName propertyName(readOnly, scope, propertyLibrary->GetName(), name);

	// Register the property name, and check for duplicates
	if (!propertyLibrary->RegisterPropertyName(propertyName, true))
	{
		Error("Error registering property name.  Possible duplicate.");
		return;
	}

	// Set property value
	EmitOpcode(Opcode::Property);
	propertyName.Write(m_writer);

	if (Accept(SymbolType::Is))
	{
		ParseExpression();
		EmitOpcode(Opcode::SetProp);
		EmitId(propertyName.GetId());
	}
	else if (readOnly)
	{
		// A declaration with no assignment is allowed, but not for a readonly property			
		Error("Must assign property an initial value");
		return;
	}
	Expect(SymbolType::NewLine);

}

PropertyName Parser::ParsePropertyName()
{
	PropertyName propertyName;
	// Check to see if this begins with a library name
	auto library = m_library;
	String libraryName = CheckLibraryName();
	if (!libraryName.empty())
	{
		libraryName = ParseName();
		library = m_runtime->GetLibraryInternal(libraryName);
		auto name = ParseName();
		propertyName = library->GetPropertyName(name);
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
		auto name = ParseName();
		propertyName = m_library->GetPropertyName(name);

		// Check import names if we can't find the property name locally.
		if (!propertyName.IsValid())
		{
			bool foundProperty = false;
			for (const auto & import : m_importList)
			{
				library = m_runtime->GetLibraryInternal(import);
				propertyName = library->GetPropertyName(name);
				if (propertyName.IsValid())
				{
					// If we haven't specified the library name explicitly, we can assume we're looking
					// for a different library.
					if (m_library->GetName() != libraryName && propertyName.GetVisibility() != VisibilityType::Public)
						continue;
					if (foundProperty)
					{
						Error("Ambiguous property name found");
						return PropertyName();
					}
					foundProperty = true;
				}
			}
			if (propertyName.IsValid() && library != m_library && propertyName.GetVisibility() != VisibilityType::Public)
			{
				Error("Unable to access private property");
				return PropertyName();
			}
		}
	}
	return propertyName;
}

String Parser::ParseFunctionNamePart()
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

FunctionSignature Parser::ParseFunctionSignature(VisibilityType scope)
{
	bool returnParameter = Accept(SymbolType::Return);
	if (Check(SymbolType::NewLine))
	{
		Error("Empty function signature");
		return FunctionSignature();
	}
	bool parsedParameter = false;
	bool parsedNonKeywordName = false;
	int parsedNameCount = 0;
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
			part.partType = FunctionSignaturePartType::Parameter;
			if (CheckValueType())
			{
				part.valueType = ParseValueType();
			}
			if (CheckName())
			{
				auto paramName = ParseName();
				part.names.push_back(paramName);
			}
			else
			{
				Error("No variable name or class identifier found in function signature");
				return FunctionSignature();
			}
			Expect(SymbolType::CurlyClose);
			parsedParameter = true;
		}
		else
		{
			if (!CheckFunctionNamePart())
			{
				Error("Invalid name in function signature");
				return FunctionSignature();
			}
			parsedNameCount++;
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
			parsedParameter = false;
		}
		signatureParts.push_back(part);
	}
	if (!Expect(SymbolType::NewLine))
	{
		Error("Expected new line at end of function signature");
		return FunctionSignature();
	}

	// Check for function signature validity
	if (!parsedNonKeywordName)
	{
		if (parsedNameCount == 1 && signatureParts.size() == 1)
		{
			Error("Function signature cannot match keyword");
			return FunctionSignature();
		}
	}

	// Emit function definition opcode
	EmitOpcode(Opcode::Function);

	// Create the function signature
	FunctionSignature signature(scope, returnParameter, m_library->GetName(), signatureParts);
	signature.Write(m_writer);
	return signature;
}

void Parser::ParseFunctionDefinition(VisibilityType scope)
{
	// Check to make sure we're at the root frame
	if (!IsRootFrame())
	{
		Error("Can't define a function inside another class or function");
		return;
	}

	// Check to make sure we're at the root scope
	if (!IsRootScope())
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
		if (!m_localFunctions.Register(signature, true))
		{
			Error("Function already defined in script %s", m_library->GetName().c_str());
			return;
		}
	}
	else
	{
		// Register function signature in library
		if (!m_library->Functions().Register(signature, true))
		{
			Error("Function already defined in library %s", m_library->GetName().c_str());
			return;
		}
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
		EmitName(itr->names.front());
		EmitIndex(stackIndex);
		EmitValueType(itr->valueType);
		--stackIndex;
	}

	// Mark whether we require a return value
	m_requireReturnValue = signature.HasReturnParameter();

	// Parse the function body
	while (!Check(SymbolType::End) && !m_error)
		ParseStatement();
	Expect(SymbolType::End);
	Expect(SymbolType::NewLine);

	// Check to make sure we've returned a value as expected
	if (m_requireReturnValue && !m_returnedValue)
		Error("Required return value not found");

	// Return from the function.  
	EmitOpcode(Opcode::Return);

	// Backfill jump destination 
	EmitAddressBackfill(jumpBackfillAddress);

	// Mark end of execution frame
	FrameEnd();
}

void Parser::ParseFunctionCall(const FunctionSignature * signature)
{
	assert(signature);

	auto libName = CheckLibraryName();
	if (!libName.empty())
		NextSymbol();
	
	// We only need to suppress potential recursive function calls if the first 
	// token is a parameter.
	int count = 0;

	// Match each token or token set to a part of the function signature
	for (const auto & part : signature->GetParts())
	{
		if (part.partType == FunctionSignaturePartType::Name)
		{
			// Validate each part of the function name
			if (CheckFunctionNamePart())
			{
				auto name = ParseFunctionNamePart();
				bool match = false;
				for (const auto & n : part.names)
				{
					if (name == n)
					{
						match = true;
						break;
					}
				}
				if (!match)
				{
					Error("Mismatch in function name");
					return;
				}
			}
			else
			{
				Error("Expecting function name");
				return;
			}
		}
		else
		{
			// Push parameter onto the stack
			if (Accept(SymbolType::ParenOpen))
			{
				ParseExpression();
				Expect(SymbolType::ParenClose);
			}
			else
			{
				ParseExpression(count == 0);
			}
		}
		count++;
	}

	// When finished validating the function and pushing parameters, call the function
	EmitOpcode(Opcode::CallFunc);
	EmitId(signature->GetId());
}

void Parser::ParseSubexpression(bool suppressFunctionCall)
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

	// Check for a logical not at the beginning of the expression
	if (Accept(SymbolType::Not))
	{
		ParseExpression();
		EmitOpcode(Opcode::Not);	
	}
	else
	{
		// Check for two operands in a row without an intervening operator
		// Note: there must be a cleaner way of parsing this while ensuring
		// ordering consistency of operands and operators.  This method seeems
		// sort of clunky.
		bool operand = false;
		while (!Check(SymbolType::NewLine))
		{
			if (CheckBinaryOperator())
			{
				auto opcode = ParseBinaryOperator();
				opcodeStack.push_back(opcode);
				operand = false;
			}
			else
			{
				const FunctionSignature * signature = nullptr;
				if (!suppressFunctionCall)
					signature = CheckFunctionCall();
				suppressFunctionCall = false;
				if (signature)
				{
					if (operand == true)
					{
						Error("Expecting operator in expression");
						return;
					}
					if (signature->HasReturnParameter() == false)
					{
						Error("Function in expression requires a return parameter");
						return;
					}
					ParseFunctionCall(signature);
					operand = true;
				}
				else if (CheckProperty())
				{
					if (operand == true)
					{
						Error("Expecting operator in expression");
						return;
					}
					auto propertyName = ParsePropertyName();
					if (!propertyName.IsValid())
					{
						Error("Unable to find property name in library");
						return;
					}
					bool subscript = ParseSubscript();
					EmitOpcode(subscript ? Opcode::PushPropKeyVal : Opcode::PushProp);
					EmitId(propertyName.GetId());
					operand = true;
					if (Accept(SymbolType::Type))
						EmitOpcode(Opcode::Type);
				}
				else if (CheckVariable())
				{
					if (operand == true)
					{
						Error("Expecting operator in expression");
						return;
					}
					String name = ParseVariable();
					bool subscript = ParseSubscript();
					EmitOpcode(subscript ? Opcode::PushVarKey : Opcode::PushVar);
					EmitName(name);
					operand = true;
					if (Accept(SymbolType::Type))
						EmitOpcode(Opcode::Type);
				}
				else if (Check(SymbolType::Comma) || Check(SymbolType::ParenClose) || Check(SymbolType::SquareClose) || Check(SymbolType::To) || Check(SymbolType::By))
				{
					break;
				}
				else if (Accept(SymbolType::ParenOpen))
				{
					if (operand == true)
					{
						Error("Expecting operator in expression");
						return;
					}
					ParseExpression();
					Expect(SymbolType::ParenClose);
					operand = true;
				}
				else if (Check(SymbolType::And) || Check(SymbolType::Or))
				{
					auto t = m_currentSymbol->type;
					NextSymbol();
					ParseExpression();
					EmitOpcode(t == SymbolType::And ? Opcode::And : Opcode::Or);
					break;
				}
				else if (Accept(SymbolType::As))
				{
					EmitOpcode(Opcode::Cast);
					auto valueType = ParseValueType();
					if (m_error)
						return;
					EmitValueType(valueType);
				}
				else if (CheckValue())
				{
					if (operand == true)
					{
						Error("Expecting operator in expression");
						return;
					}
					auto val = ParseValue();
					EmitOpcode(Opcode::PushVal);
					EmitValue(val);
					operand = true;
				}
				else if (CheckValueType())
				{
					if (operand == true)
					{
						Error("Expecting operator in expression");
						return;
					}
					auto val = ParseValueType();
					EmitOpcode(Opcode::PushVal);
					EmitValue(val);
					operand = true;
				}
				else
				{
					// We're finished with the expression
					return;
				}
				

				if (!opcodeStack.empty())
				{
					EmitOpcode(opcodeStack.back());
					opcodeStack.pop_back();
				}
			}
		}
	}

	// Check for leftover operators
	if (!opcodeStack.empty())
	{
		Error("Syntax error when parsing expression");
	}
}

void Parser::ParseExpression(bool suppressFunctionCall)
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
			ParseSubexpression(suppressFunctionCall);

			// If we see a comma after a square open bracket, we're parsing a key-value pair
			if (Accept(SymbolType::Comma))
			{
				ParseExpression(suppressFunctionCall);
				Expect(SymbolType::SquareClose);

				// Parse all subsequent key-value pairs
				uint32_t count = 1;
				while (Accept(SymbolType::Comma))
				{
					Expect(SymbolType::SquareOpen);
					ParseSubexpression();
					Expect(SymbolType::Comma);
					ParseSubexpression();
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
		ParseSubexpression(suppressFunctionCall);

		// If we finish the first subexpression with a common, then we're parsing an indexed list
		if (Accept(SymbolType::Comma))
		{
			if (Check(SymbolType::NewLine))
			{
				Error("Unexpected end of line in list");
				return;
			}

			// Parse all subexpressions in comma-delimited list
			uint32_t count = 1;
			do
			{
				ParseSubexpression();
				++count;
			} 
			while (Accept(SymbolType::Comma));

			// Pop all key-value pairs and push the results on the stack
			EmitOpcode(Opcode::PushList);
			EmitCount(count);
		}
	}
}

void Parser::ParseIncDec()
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
	}
	else if (CheckName())
	{
		varName = ParseName();
		EmitOpcode(Opcode::PushVar);
		EmitName(varName);
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
	}
	else
	{
		EmitOpcode(Opcode::SetVar);
		EmitName(varName);
	}
	Expect(SymbolType::NewLine);
}

void Parser::ParseIfElse()
{
	// Parse expression after the if keyword
	ParseExpression();
	Expect(SymbolType::NewLine);

	// Add jump instruction, making sure to store the jump address for later backfilling
	EmitOpcode(Opcode::JumpFalse);
	auto ifJumpAddress = EmitAddressPlaceholder();

	// Parse new block of code after if line
	ParseBlock();

	// If we've returned a value in the if block, store it here
	bool returnedValueInIfBlock = m_returnedValue;

	// Clear any return flag after this block, since it's after a conditional branch
	m_returnedValue = false;

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

			// Check to see if we're required to return a value
			if (m_requireReturnValue)
			{
				// If we've haven't returned a value in the 'if' block, then we can't
				// leave the returned value flag marked true, since not all paths
				// have returned a value.
				if (!returnedValueInIfBlock)
					m_returnedValue = false;
			}
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

	if (!returnedValueInIfBlock)
		m_returnedValue = false;
}

void Parser::ParseLoop()
{
	// Check to see if we're using an explicitly named variable for the loop counter
	String name;
	if (CheckName())
		name = ParseName();
	
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
			EmitName(name);
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

		// Retrieve collection from top of stack and push iterator and value from beginning position
		EmitOpcode(Opcode::PushItr);
		EmitOpcode(Opcode::PushItrVal);

		// Assign the iterator to a variable name if it exists
		if (!name.empty())
		{
			VariableAssign(name);
			EmitOpcode(Opcode::SetVar);
			EmitName(name);
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
	// Loops while a condition is true
	else if (Accept(SymbolType::While))
	{
		// Store where the loop logic begins
		auto loopBeginAddress = m_writer.Tell();

		// Parse the expression to control the loop's jump branch
		ParseExpression();
		if (!Expect(SymbolType::NewLine))
			return;

		// Add jump instruction, making sure to store the jump address for later backfilling
		EmitOpcode(Opcode::JumpFalse);
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
	// Executes once and then loops again while a condition is true
	else if (Accept(SymbolType::NewLine))
	{
		// Store where the loop logic begins
		auto loopBeginAddress = m_writer.Tell();

		// Parse the while loop block
		ParseBlock();
		Expect(SymbolType::While);

		// Parse expression used to determine if loop should execute again
		ParseExpression();
		Expect(SymbolType::NewLine);

		// Conditionally jump to top of loop
		EmitOpcode(Opcode::JumpTrue);
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

void Parser::ParseStatement()
{
	// No need to continue if an error has been flagged
	if (m_error)
		return;

	// Functions signatures have precedence over everything, so check for a 
	// potential signature match before anything else.
	auto signature = CheckFunctionCall();
	if (signature)
	{
		// We found a valid function signature that matches the current token(s)
		ParseFunctionCall(signature);

		// If we're calling a function that has a return value as a statement,
		// we must discard the return value on the stack.
		if (signature->HasReturnParameter())
			EmitOpcode(Opcode::Pop);
		Expect(SymbolType::NewLine);
	}
	else
	{
		
		// Parse optional readonly
		bool readOnly = Accept(SymbolType::Readonly);

		// Parse scope level
		VisibilityType scope = ParseScope();

		// Read only can only apply to properties
		if (readOnly)
		{
			if (scope == VisibilityType::Local)
			{
				Error("The 'readonly' keyword must precede a private or public property");
				return;
			}
		}
		
		if (Accept(SymbolType::Function))
		{
			// We're parsing a function definition
			ParseFunctionDefinition(scope);
		}
		else if (CheckName())
		{
			// Can't use the current library name or preface the variable with it
			if (m_currentSymbol->text == m_library->GetName())
			{
				Error("Illegal use of library name in identifier");
				return;
			}

			// We're declaring a new property if we see a non-local scope declaration
			if (scope != VisibilityType::Local)
			{
				ParsePropertyDeclaration(readOnly, scope);
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
						return;
					}

					// Check for subscript operator
					bool subscript = ParseSubscript();

					// Check for an 'is' statement
					Expect(SymbolType::Is);

					// Parse assignment expression
					ParseExpression();
					Expect(SymbolType::NewLine);

					// Assign property
					EmitOpcode(subscript ? Opcode::SetPropKeyVal : Opcode::SetProp);
					EmitId(propertyName.GetId());
				}
				// Otherwise we're just dealing with an ordinary variable
				else
				{
					// Get the variable name
					String name = ParseName();

					// Check for subscript operator
					bool subscript = ParseSubscript();

					// Check for an 'is' statement
					Expect(SymbolType::Is);

					// Parse assignment expression
					ParseExpression();
					Expect(SymbolType::NewLine);

					// Assign a variable.  
					EmitOpcode(subscript ? Opcode::SetVarKey : Opcode::SetVar);
					EmitName(name);

					// Add to variable table
					VariableAssign(name);
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
			else if (Check(SymbolType::Increment) || Check(SymbolType::Decrement))
			{
				// We're parsing an increment or decrement statement
				ParseIncDec();
			}
			else if (Accept(SymbolType::Return))
			{
				// We've hit a return value
				if (!Check(SymbolType::NewLine))
				{
					// Check to make sure we're allowed to return a value
					if (!m_requireReturnValue)
						Error("Unexpected return value");
					else
						m_returnedValue = true;
					ParseExpression();
					EmitOpcode(Opcode::ReturnValue);
				}
				else
				{
					if (m_requireReturnValue)
						Error("Required return value not found");
					EmitOpcode(Opcode::Return);
				}
				Accept(SymbolType::NewLine);
			}
			else if (Accept(SymbolType::Break))
			{
				// We've hit a break statement
				Expect(SymbolType::NewLine);
				EmitOpcode(Opcode::Jump);
				m_breakAddress = EmitAddressPlaceholder();
			}
			else if (Accept(SymbolType::Yield))
			{
				// Check for basic yield statement
				if (Accept(SymbolType::NewLine))
					EmitOpcode(Opcode::Yield);
				else if (Accept(SymbolType::While))
				{
					// Store expression address
					auto expressionAddress = m_writer.Tell();

					// Parse the expression to check for yield
					ParseExpression();
					if (!Expect(SymbolType::NewLine))
						return;

					// Add jump if false over yield statement
					EmitOpcode(Opcode::JumpFalse);
					auto addressPlaceholder = EmitAddressPlaceholder();

					// Yield statement is executed if expression is true
					EmitOpcode(Opcode::Yield);
					EmitOpcode(Opcode::Jump);
					EmitAddress(expressionAddress);

					// Backfill placeholder at end of conditional yield statement
					EmitAddressBackfill(addressPlaceholder);
				}
				else
				{
					Error("Unexpected symbol after yield");
				}
			}
			else
			{
				Error("Unknown symbol in statement");
			}
		}
		else
		{
			Error("Invalid symbol after scope specifier '%'", scope == VisibilityType::Public ? "public" : "private");
		}
	}
}

void Parser::ParseBlock()
{
	if (m_error)
		return;
	
	// Push local variable stack frame
	ScopeBegin();

	// Parse each statement until we reach a terminating symbol
	while (!(Check(SymbolType::End) || Check(SymbolType::Else) || Check(SymbolType::While)) && !m_error)
		ParseStatement();

	// Pop local variable stack frame
	ScopeEnd();
}

void Parser::ParseLibraryImports()
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
		if (!Expect(SymbolType::NewLine))
		{
			Error("Expected new line after library import name");
			return;
		}

		// Check to make sure we're not adding duplicates
		for (auto & importName : m_importList)
		{
			if (importName == name)
				continue;
		}

		// Add library to the list of imported libraries for this script
		m_importList.push_back(name);
	}
}

void Parser::ParseLibraryDeclaration()
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
		if (!Expect(SymbolType::NewLine))
		{
			Error("Expected new line after library name");
			return;
		}
		m_library = m_runtime->GetLibraryInternal(libraryName);
	}

	// Emit library declaration bytecode
	EmitOpcode(Opcode::Library);
	EmitName(libraryName);

	// Retrieve library interface by name
	m_library = m_runtime->GetLibraryInternal(libraryName);
}

void Parser::ParseScript()
{
	ParseLibraryImports();
	ParseLibraryDeclaration();
	while (m_currentSymbol != m_symbolList.end() && !m_error)
		ParseStatement();
	if (m_breakAddress)
		Error("Illegal break");
	EmitOpcode(Opcode::Exit);
}