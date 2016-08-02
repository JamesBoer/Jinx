/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_FUNCTION_SIGNATURE_H__
#define JX_FUNCTION_SIGNATURE_H__


namespace Jinx
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
			valueType(ValueType::Any)
		{}
		FunctionSignaturePartType partType;
		ValueType valueType;
		std::vector<String, Allocator<String>> names;
	};

	typedef std::vector<FunctionSignaturePart, Allocator<FunctionSignaturePart>> FunctionSignatureParts;

	// Function and member function signature object.
	class FunctionSignature
	{
	public:
		FunctionSignature();
		FunctionSignature(VisibilityType visibility, bool returnParameter, const String & libraryName, const FunctionSignatureParts & parts);

		// Get unique function id
		RuntimeID GetId() const { return m_id; }

		// Get signature length
		size_t GetLength() const { return m_parts.size(); }

		// Get visibility level
		VisibilityType GetVisibility() const { return m_visibility; }

		// Get signature parts
		const FunctionSignatureParts & GetParts() const { return m_parts; }

		// Does this function return a parameter?
		bool HasReturnParameter() const { return m_returnParameter; }

		// Does this have a class parameter
		bool HasClassParameter() const { return m_classParameter; }

		// Is this a valid signature?
		inline bool IsValid() const { return !m_parts.empty(); }

		// Get a list of parameter parts
		FunctionSignatureParts GetParameters() const;

		// Get number of parameters
		size_t GetParameterCount() const;

		// Check for a signature match against parts.  Note that the source parts list is allowed to exceed the
		// signature being matched against, but not vice-versa.
		bool IsMatch(const FunctionSignatureParts & parts) const;

		// Serialization
		void Read(BinaryReader & reader);
		void Write(BinaryWriter & writer) const;

	private:

		friend bool operator == (const FunctionSignature & left, const FunctionSignature & right);

	private:

		// Unique id
		RuntimeID m_id;

		// Visibility level
		VisibilityType m_visibility;

		// Indicates whether the function returns an anonymous parameter
		bool m_returnParameter;

		// Indicates this has a class parameter
		bool m_classParameter;

		// Each signature is made up of any number of parts representing either part
		// of the function name or a variable placeholder.
		FunctionSignatureParts m_parts;

	};

	bool operator == (const FunctionSignaturePart & left, const FunctionSignaturePart & right);
	bool operator == (const FunctionSignature & left, const FunctionSignature & right);

};

#endif // JX_FUNCTION_SIGNATURE_H__

