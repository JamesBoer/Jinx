/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

namespace Jinx::Impl
{

	inline_t FunctionSignature::FunctionSignature()
	{
	}

	inline_t FunctionSignature::FunctionSignature(VisibilityType visibility, const String & libraryName, const FunctionSignatureParts & parts) :
		m_visibility(visibility),
		m_libraryName(libraryName),
		m_parts(parts)
	{
		if (m_visibility == VisibilityType::Local)
		{
			// Local functions use a randomly generated ID to avoid collisions with any other name.
			// We don't use a predictable hash algorithm because when calling a library function, it
			// could accidentally call the wrong local function if it has the same signature as a local
			// function in the original calling script.  Because it's a local function, there's also 
			// no real need to access it outside of the script itself or through the API either.
			m_id = GetRandomId();
		}
		else
		{
			// Library functions require a predictable ID.
			// Create a unique id based on a hash of the library name, signature text, and parameters
			String hashString = m_libraryName;
			hashString.reserve(64);
			for (auto itr = m_parts.begin(); itr != m_parts.end();)
			{
				if (itr->partType == FunctionSignaturePartType::Name)
				{
					for (const auto & name : itr->names)
						hashString += name;
				}
				else if (itr->partType == FunctionSignaturePartType::Parameter)
				{
					hashString += "{}";
				}
				else
				{
					hashString += "{";
					hashString += itr->names.front();
					hashString += "}";
				}
				++itr;
				if (itr != m_parts.end())
					hashString += " ";
			}
			m_id = GetHash(hashString.c_str(), hashString.length());
		}
	}

	inline_t size_t FunctionSignature::GetParameterCount() const
	{
		size_t count = 0;
		for (auto & part : m_parts)
		{
			if (part.partType == FunctionSignaturePartType::Parameter)
				++count;
		}
		return count;
	}

	inline_t FunctionSignatureParts FunctionSignature::GetParameters() const
	{
		FunctionSignatureParts parameters;
		for (auto & part : m_parts)
		{
			if (part.partType == FunctionSignaturePartType::Parameter)
				parameters.push_back(part);
		}
		return parameters;
	}

	inline_t String FunctionSignature::GetName() const
	{
		String fnName;
		fnName.reserve(32);
		if (!m_libraryName.empty())
		{
			fnName += m_libraryName;
			fnName += " ";
		}
		for (auto partItr = m_parts.begin(); partItr != m_parts.end();)
		{
			if (partItr->partType == FunctionSignaturePartType::Name)
			{
				if (partItr->optional)
					fnName += "(";
				for (auto nameItr = partItr->names.begin(); nameItr != partItr->names.end();)
				{
					fnName += *nameItr;
					++nameItr;
					if (nameItr != partItr->names.end() && partItr->names.size() > 1)
						fnName += "/";
				}
				if (partItr->optional)
					fnName += ")";
			}
			else
			{
				fnName += "{";
				if (partItr->valueType != ValueType::Any)
					fnName += GetValueTypeName(partItr->valueType);
				fnName += "}";
			}

			++partItr;
			if (partItr != m_parts.end())
				fnName += " ";
		}
		return fnName;
	}

	inline_t void FunctionSignature::Read(BinaryReader & reader)
	{
		// Read this object from a memory buffer
		reader.Read(&m_id);
		reader.Read<VisibilityType, uint8_t>(&m_visibility);
		reader.Read(&m_libraryName);
		uint8_t partSize;
		reader.Read(&partSize);
		for (uint8_t i = 0; i < partSize; ++i)
		{
			FunctionSignaturePart part;
			reader.Read<FunctionSignaturePartType, uint8_t>(&part.partType);
			reader.Read(&part.optional);
			reader.Read<ValueType, uint8_t>(&part.valueType);
			uint8_t nameSize;
			reader.Read(&nameSize);
			for (uint8_t j = 0; j < nameSize; ++j)
			{
				String name;
				reader.Read(&name);
				part.names.push_back(name);
			}
			m_parts.push_back(part);
		}
	}

	inline_t void FunctionSignature::Write(BinaryWriter & writer) const
	{
		// Write this object to a memory buffer
		writer.Write(m_id);
		writer.Write<VisibilityType, uint8_t>(m_visibility);
		writer.Write(m_libraryName);
		writer.Write(static_cast<uint8_t>(m_parts.size()));
		for (const auto & part : m_parts)
		{
			writer.Write<FunctionSignaturePartType, uint8_t>(part.partType);
			writer.Write(part.optional);
			writer.Write<ValueType, uint8_t>(part.valueType);
			writer.Write(static_cast<uint8_t>(part.names.size()));
			for (const auto & name : part.names)
			{
				writer.Write(name);
			}
		}
	}

	// FunctionSignature part comparison operator overloads
	inline_t bool operator == (const FunctionSignaturePart & left, const FunctionSignaturePart & right)
	{
		// If the types are different, the parts aren't equal
		if (left.partType != right.partType)
			return false;

		// Check for any matches between the left and right sets of names.  We consider
		// the signature parts equal if there are any matches.
		if (left.partType == FunctionSignaturePartType::Name)
		{
			for (auto & leftName : left.names)
			{
				for (auto & rightName : right.names)
				{
					if (leftName == rightName)
						return true;
				}
			}
			return false;
		}
		return true;
	}

	// FunctionSignature class comparison operator overload
	inline_t bool operator == (const FunctionSignature & left, const FunctionSignature & right)
	{
		// If there's any discrepency in size, then the signatures aren't equal
		if (left.m_parts.size() != right.m_parts.size())
			return false;

		// Compare all parts between left and right operands.  If any are different,
		// then the signature is not equivalant.
		auto leftPart = left.m_parts.begin();
		auto rightPart = right.m_parts.begin();
		while (leftPart != left.m_parts.end())
		{
			if (!(*leftPart == *rightPart))
				return false;
			++leftPart;
			++rightPart;
		}
		return true;
	}

} // namespace Jinx::Impl

