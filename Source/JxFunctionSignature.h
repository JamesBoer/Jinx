/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_FUNCTION_SIGNATURE_H__
#define JX_FUNCTION_SIGNATURE_H__


namespace Jinx::Impl
{

	enum class FunctionSignaturePartType
	{
		Name,
		Parameter,
	};

	struct FunctionSignaturePart
	{
		FunctionSignaturePart() :
			partType(FunctionSignaturePartType::Name),
			optional(false),
			valueType(ValueType::Any)
		{}
		FunctionSignaturePartType partType;
		bool optional;
		ValueType valueType;
		std::vector<String, Allocator<String>> names;
	};

	using FunctionSignatureParts = std::vector<FunctionSignaturePart, Allocator<FunctionSignaturePart>>;

	// Function and member function signature object.
	class FunctionSignature
	{
	public:
		FunctionSignature();
		FunctionSignature(VisibilityType visibility, const String & libraryName, const FunctionSignatureParts & parts);

		// Get unique function id
		RuntimeID GetId() const { return m_id; }

		// Get human-readable name for debug purposes
		String GetName() const;

		// Get signature length
		size_t GetLength() const { return m_parts.size(); }

		// Get visibility level
		VisibilityType GetVisibility() const { return m_visibility; }

		// Get signature parts
		const FunctionSignatureParts & GetParts() const { return m_parts; }

		// Is this a valid signature?
		inline bool IsValid() const { return !m_parts.empty(); }

		// Get a list of parameter parts
		FunctionSignatureParts GetParameters() const;

		// Get number of parameters
		size_t GetParameterCount() const;

		// Serialization
		void Read(BinaryReader & reader);
		void Write(BinaryWriter & writer) const;

	private:

		friend bool operator == (const FunctionSignature & left, const FunctionSignature & right);

	private:

		// Unique id
		RuntimeID m_id = 0;

		// Visibility level
		VisibilityType m_visibility = VisibilityType::Local;

		// Library name
		String m_libraryName;

		// Each signature is made up of any number of parts representing either part
		// of the function name or a variable placeholder.
		FunctionSignatureParts m_parts;

	};

	bool operator == (const FunctionSignaturePart & left, const FunctionSignaturePart & right);
	bool operator == (const FunctionSignature & left, const FunctionSignature & right);

	using FunctionList = std::list<FunctionSignature, Allocator<FunctionSignature>>;
	using  FunctionPtrList = std::vector<const FunctionSignature*, Allocator<const FunctionSignature*>>;

} // namespace Jinx::Impl

#endif // JX_FUNCTION_SIGNATURE_H__

