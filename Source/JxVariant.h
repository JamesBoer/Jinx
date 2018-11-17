/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_VARIANT_H__
#define JX_VARIANT_H__

/*! \file */

/*! \namespace */
namespace Jinx
{

	class BinaryReader;
	class BinaryWriter;

	/// Interface for user objects in scripts
	class IUserObject
	{
	public:
		virtual ~IUserObject() {};
	};

	using UserObjectPtr = std::shared_ptr<IUserObject>;

	/// ValueType represents the type of value contained in a Variant object
	enum class ValueType
	{
		Null,
		Number,
		Integer,
		Boolean,
		String,
		Collection,
		CollectionItr,
		UserObject,
		Buffer,
		Guid,
		ValType,
		NumValueTypes,
		Any = NumValueTypes, // Internal use only
	};

	/// Variant stores value/type pairs in a convenient class.
	/**
	The Variant class is both used to store and manipulate values internally.  It can convert
	between different value types dynamically.  It also provides a convenient mechanism to pass 
	and retrieve values from the native Jinx API in a type-agnostic fashion.
	*/
	class Variant
	{
	public:

		// Constructor overloads
		Variant() :
			m_type(ValueType::Null)
		{}
		Variant(const Variant & copy);
		Variant(std::nullptr_t) : m_type(ValueType::Null) { SetNull(); }
		Variant(bool value) : m_type(ValueType::Null) { SetBoolean(value); }
		Variant(int32_t value) : m_type(ValueType::Null) { SetInteger(value); }
		Variant(int64_t value) : m_type(ValueType::Null) { SetInteger(value); }
		Variant(double value) : m_type(ValueType::Null) { SetNumber(value); }
		Variant(const char * value) : m_type(ValueType::Null) { SetString(value); }
		Variant(const char16_t * value) : m_type(ValueType::Null) { SetString(value); }
		Variant(const wchar_t * value) : m_type(ValueType::Null) { SetString(value); }
		Variant(const String & value) : m_type(ValueType::Null) { SetString(value); }
		Variant(const StringU16 & value) : m_type(ValueType::Null) { SetString(value); }
		Variant(const WString & value) : m_type(ValueType::Null) { SetString(value); }
		Variant(const CollectionPtr & value) : m_type(ValueType::Null) { SetCollection(value); }
		Variant(const CollectionItrPair & value) : m_type(ValueType::Null) { SetCollectionItr(value); }
		Variant(const UserObjectPtr & value) : m_type(ValueType::Null) { SetUserObject(value); }
		Variant(const BufferPtr & value) : m_type(ValueType::Null) { SetBuffer(value); }
		Variant(const Guid & value) : m_type(ValueType::Null) { SetGuid(value); }
		Variant(ValueType value) : m_type(ValueType::Null) { SetValType(value); }

		// Destructor
		~Variant();

		// Assignment operator overloads
		Variant & operator= (const Variant & copy);

		// Increment operators
		Variant & operator++();
		Variant operator++(int) {
			Variant tmp(*this);
			operator++();
			return tmp;
		}
		Variant & operator += (const Variant & right);

		// Decrement operators
		Variant & operator--();
		Variant operator--(int) {
			Variant tmp(*this);
			operator--();
			return tmp;
		}
		Variant & operator -= (const Variant & right);

		// Value getters
		bool GetBoolean() const;
		int64_t GetInteger() const;
		double GetNumber() const;
		String GetString() const;
		StringU16 GetStringU16() const;
		WString GetWString() const;
		CollectionPtr GetCollection() const;
		CollectionItrPair GetCollectionItr() const;
		UserObjectPtr GetUserObject() const;
		BufferPtr GetBuffer() const;
		Guid GetGuid() const;
		ValueType GetValType() const;

		// Type getter
		ValueType GetType() const { return m_type; }

		// Is this a valid collection key?
		bool IsKeyType() const;

		// Is this an integer or number type?
		bool IsNumericType() const;

		// Type checks
		bool IsType(ValueType type) { return m_type == type ? true : false; }
		bool IsNull() const { return m_type == ValueType::Null ? true : false; }
		bool IsBoolean() const { return m_type == ValueType::Boolean ? true : false; }
		bool IsInteger() const { return m_type == ValueType::Integer ? true : false; }
		bool IsNumber() const { return m_type == ValueType::Number ? true : false; }
		bool IsString() const { return m_type == ValueType::String ? true : false; }
		bool IsCollection() const { return m_type == ValueType::Collection ? true : false; }
		bool IsCollectionItr() const { return m_type == ValueType::CollectionItr ? true : false; }
		bool IsUserObject() const { return m_type == ValueType::UserObject ? true : false; }
		bool IsBuffer() const { return m_type == ValueType::Buffer ? true : false; }
		bool IsGuid() const { return m_type == ValueType::Guid ? true : false; }
		bool IsValType() const { return m_type == ValueType::ValType ? true : false; }

		// Value setters
		void SetNull();
		void SetBoolean(bool value);
		void SetInteger(int64_t value);
		void SetNumber(double value);
		void SetString(const String & value);
		void SetString(const StringU16 & value);
		void SetString(const WString & value);
		void SetCollection(const CollectionPtr & value);
		void SetCollectionItr(const CollectionItrPair & value);
		void SetUserObject(const UserObjectPtr & value);
		void SetBuffer(const BufferPtr & value);
		void SetGuid(const Guid & value);
		void SetValType(ValueType type);

		// Check to see if a successful type conversion can be made
		bool CanConvertTo(ValueType type) const;

		// Conversion casts.  True if successful, false if invalid cast
		bool ConvertTo(ValueType type);

		// Serialization
		void Read(BinaryReader & reader);
		void Write(BinaryWriter & writer) const;

	private:

		void Destroy();

		ValueType m_type;
		union
		{
			bool m_boolean;
			double m_number;
			int64_t m_integer;
			ValueType m_valType;
			String m_string;
			CollectionPtr m_collection;
			CollectionItrPair m_collectionItrPair;
			UserObjectPtr m_userObject;
			BufferPtr m_buffer;
			Guid m_guid;
		};
	};

	// Arithmetic operators
	Variant operator + (const Variant & left, const Variant & right);
	Variant operator - (const Variant & left, const Variant & right);
	Variant operator * (const Variant & left, const Variant & right);
	Variant operator / (const Variant & left, const Variant & right);
	Variant operator % (const Variant & left, const Variant & right);

	// Comparison operators
	bool operator == (const Variant & left, const Variant & right);
	bool operator < (const Variant & left, const Variant & right);
	bool operator <= (const Variant & left, const Variant & right);
	inline bool operator != (const Variant & left, const Variant & right) { return (left == right) ? false : true; }
	inline bool operator > (const Variant & left, const Variant & right) { return (left <= right) ? false : true; }
	inline bool operator >= (const Variant & left, const Variant & right) { return (left < right) ? false : true; }

	// Variant operator <, <=, >, >= type validation
	bool ValidateValueComparison(const Variant & left, const Variant & right);

	/// Helper function convertion to utf-8 string from other types using a Variant object
	inline String Str(const char16_t * str) { return Variant(str).GetString(); }
	inline String Str(const wchar_t * str) { return Variant(str).GetString(); }
	inline String Str(const StringU16 & str) { return Variant(str).GetString(); }
	inline String Str(const WString & str) { return Variant(str).GetString(); }

}

#endif // JX_VARIANT_H__