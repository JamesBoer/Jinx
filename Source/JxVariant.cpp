/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

namespace Jinx
{

	namespace Impl
	{
		struct VarCmpTable
		{
			static inline bool allowValueCompare[] =
			{
				false,	// Null
				true,	// Number
				true,	// Integer
				false,	// Boolean
				true,	// String
				false,	// Collection
				false,	// CollectionItr
				false,	// UserObject
				false,	// Buffer
				true,	// Guid
				false,	// ValType
			};

			static_assert(countof(allowValueCompare) == static_cast<size_t>(ValueType::NumValueTypes), "Value compare flags don't match enum count");

		};

		inline_t bool CheckValueTypeForCompare(ValueType type)
		{
			return VarCmpTable::allowValueCompare[static_cast<size_t>(type)];
		}

	} // namespace Impl

	inline_t Variant::Variant(const Variant & copy)
	{
		m_type = copy.m_type;
		switch (m_type)
		{
			case ValueType::Null:
				break;
			case ValueType::Number:
				m_number = copy.m_number;
				break;
			case ValueType::Integer:
				m_integer = copy.m_integer;
				break;
			case ValueType::Boolean:
				m_boolean = copy.m_boolean;
				break;
			case ValueType::String:
				new(&m_string) String();
				m_string = copy.m_string;
				break;
			case ValueType::Collection:
				new(&m_collection) CollectionPtr();
				m_collection = copy.m_collection;
				break;
			case ValueType::CollectionItr:
				new(&m_collectionItrPair) CollectionItrPair();
				m_collectionItrPair = copy.m_collectionItrPair;
				break;
			case ValueType::UserObject:
				new(&m_userObject) UserObjectPtr();
				m_userObject = copy.m_userObject;
				break;
			case ValueType::Buffer:
				new(&m_buffer) BufferPtr();
				m_buffer = copy.m_buffer;
				break;
			case ValueType::Guid:
				m_guid = copy.m_guid;
				break;
			case ValueType::ValType:
				m_valType = copy.m_valType;
				break;
			default:
				assert(!"Unknown variant type!");
		};
	}

	inline_t Variant::~Variant()
	{
		Destroy();
	}

	inline_t Variant & Variant::operator= (const Variant & copy)
	{
		Destroy();
		m_type = copy.m_type;
		switch (m_type)
		{
			case ValueType::Null:
				break;
			case ValueType::Number:
				m_number = copy.m_number;
				break;
			case ValueType::Integer:
				m_integer = copy.m_integer;
				break;
			case ValueType::Boolean:
				m_boolean = copy.m_boolean;
				break;
			case ValueType::String:
				new(&m_string) String();
				m_string = copy.m_string;
				break;
			case ValueType::Collection:
				new(&m_collection) CollectionPtr();
				m_collection = copy.m_collection;
				break;
			case ValueType::CollectionItr:
				new(&m_collectionItrPair) CollectionItrPair();
				m_collectionItrPair = copy.m_collectionItrPair;
				break;
			case ValueType::UserObject:
				new(&m_userObject) UserObjectPtr();
				m_userObject = copy.m_userObject;
				break;
			case ValueType::Buffer:
				new(&m_buffer) BufferPtr();
				m_buffer = copy.m_buffer;
				break;
			case ValueType::Guid:
				m_guid = copy.m_guid;
				break;
			case ValueType::ValType:
				m_valType = copy.m_valType;
				break;
			default:
				assert(!"Unknown variant type!");
		};
		return *this;
	}

	inline_t Variant & Variant::operator ++()
	{
		switch (m_type)
		{
			case ValueType::Number:
				++m_number;
				break;
			case ValueType::Integer:
				++m_integer;
				break;
			case ValueType::CollectionItr:
				++m_collectionItrPair.first;
				break;
			default:
				break;
		};
		return *this;
	}

	inline_t Variant & Variant::operator += (const Variant & right)
	{
		switch (m_type)
		{
			case ValueType::Number:
				m_number += right.GetNumber();
				break;
			case ValueType::Integer:
				m_integer += right.GetInteger();
				break;
			case ValueType::String:
				m_string += right.GetString();
				break;
			default:
				break;
		}
		return *this;
	}

	inline_t Variant & Variant::operator --()
	{
		switch (m_type)
		{
			case ValueType::Number:
				--m_number;
				break;
			case ValueType::Integer:
				--m_integer;
				break;
			default:
				break;
		};
		return *this;
	}

	inline_t Variant & Variant::operator -= (const Variant & right)
	{
		switch (m_type)
		{
			case ValueType::Number:
				m_number -= right.GetNumber();
				break;
			case ValueType::Integer:
				m_integer -= right.GetInteger();
				break;
			default:
				break;
		}
		return *this;
	}

	inline_t bool Variant::CanConvertTo(ValueType type) const
	{
		if (m_type == type || type == ValueType::Null)
			return true;
		Variant v = *this;
		return v.ConvertTo(type);
	}

	inline_t bool Variant::ConvertTo(ValueType type)
	{
		if (m_type == type)
			return true;
		if (type == ValueType::Null)
		{
			SetNull();
			return true;
		}

		switch (m_type)
		{
			case ValueType::Null:
			{
				switch (type)
				{
					case ValueType::Number:
						SetNumber(Impl::NullToNumber());
						return true;
					case ValueType::Integer:
						SetInteger(Impl::NullToInteger());
						return true;
					case ValueType::Boolean:
						SetBoolean(Impl::NullToBoolean());
						return true;
					case ValueType::String:
						SetString(Impl::NullToString());
						return true;
					default:
						break;
				};
			}
			break;
			case ValueType::Number:
			{
				switch (type)
				{
					case ValueType::Integer:
						SetInteger(Impl::NumberToInteger(m_number));
						return true;
					case ValueType::Boolean:
						SetBoolean(Impl::NumberToBoolean(m_number));
						return true;
					case ValueType::String:
						SetString(Impl::NumberToString(m_number));
						return true;
					default:
						break;
				};
			}
			break;
			case ValueType::Integer:
			{
				switch (type)
				{
					case ValueType::Number:
						SetNumber(Impl::IntegerToNumber(m_integer));
						return true;
					case ValueType::Boolean:
						SetBoolean(Impl::IntegerToBoolean(m_integer));
						return true;
					case ValueType::String:
						SetString(Impl::IntegerToString(m_integer));
						return true;
					default:
						break;
				};
			}
			break;
			case ValueType::Boolean:
			{
				switch (type)
				{
					case ValueType::Number:
						SetNumber(Impl::BooleanToNumber(m_boolean));
						return true;
					case ValueType::Integer:
						SetInteger(Impl::BooleanToInteger(m_boolean));
						return true;
					case ValueType::String:
						SetString(Impl::BooleanToString(m_boolean));
						return true;
					default:
						break;
				};
			}
			break;
			case ValueType::String:
			{
				switch (type)
				{
					case ValueType::Number:
					{
						double number;
						if (!Impl::StringToNumber(m_string, &number))
						{
							Impl::LogWriteLine(LogLevel::Error, "Error converting string %s to number", m_string.c_str());
							SetNull();
							return false;
						}
						SetNumber(number);
						return true;
					}
					case ValueType::Integer:
					{
						int64_t integer;
						if (!Impl::StringToInteger(m_string, &integer))
						{
							Impl::LogWriteLine(LogLevel::Error, "Error converting string %s to integer", m_string.c_str());
							SetNull();
							return false;
						}
						SetInteger(integer);
						return true;
					}
					case ValueType::Boolean:
					{
						bool boolean;
						if (!Impl::StringToBoolean(m_string, &boolean))
						{
							Impl::LogWriteLine(LogLevel::Error, "Error converting string %s to boolean", m_string.c_str());
							SetNull();
							return false;
						}
						SetBoolean(boolean);
						return true;
					}
					case ValueType::Collection:
					{
						auto collection = CreateCollection();
						if (!Impl::StringToCollection(m_string, &collection))
						{
							Impl::LogWriteLine(LogLevel::Error, "Error converting string to collection type", m_string.c_str());
							SetNull();
							return false;
						}
						SetCollection(collection);
						return true;
					}
					case ValueType::Guid:
					{
						Guid guid;
						if (!Impl::StringToGuid(m_string, &guid))
						{
							Impl::LogWriteLine(LogLevel::Error, "Error converting string %s to Guid", m_string.c_str());
							SetNull();
							return false;
						}
						SetGuid(guid);
						return true;
					}
					case ValueType::ValType:
					{
						ValueType valType;
						if (!Impl::StringToValueType(m_string, &valType))
						{
							Impl::LogWriteLine(LogLevel::Error, "Error converting string %s to value type", m_string.c_str());
							SetNull();
							return false;
						}
						SetValType(valType);
						return true;
					}
					default:
						break;
				};
			}
			break;
			case ValueType::Collection:
				switch (type)
				{
					case ValueType::Boolean:
						SetBoolean(!m_collection->empty());
						return true;
					default:
						break;
				};
				break;
			case ValueType::Guid:
				switch (type)
				{
					case ValueType::String:
						SetString(Impl::GuidToString(m_guid));
						return true;
					default:
						break;
				};
				break;
			case ValueType::ValType:
				switch (type)
				{
					case ValueType::String:
						SetString(Impl::GetValueTypeName(m_valType));
						return true;
					default:
						break;
				};
				break;
			default:
				break;
		};
		Impl::LogWriteLine(LogLevel::Error, "Error converting %s to %s", Impl::GetValueTypeName(m_type), Impl::GetValueTypeName(type));
		SetNull();
		return false;
	}

	inline_t void Variant::Destroy()
	{
		// Optimize for common case
		if (m_type == ValueType::Null)
			return;

		// Explicitly call destructors for object types
		switch (m_type)
		{
			case ValueType::String:
				m_string.~String();
				break;
			case ValueType::Collection:
				m_collection.~CollectionPtr();
				break;
			case ValueType::CollectionItr:
				m_collectionItrPair.~CollectionItrPair();
				break;
			case ValueType::UserObject:
				m_userObject.~UserObjectPtr();
				break;
			case ValueType::Buffer:
				m_buffer.~BufferPtr();
				break;
			default:
				break;
		};

		m_type = ValueType::Null;
	}

	inline_t bool Variant::GetBoolean() const
	{
		if (IsBoolean())
			return m_boolean;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::Boolean))
			return false;
		return v.GetBoolean();
	}

	inline_t CollectionPtr Variant::GetCollection() const
	{
		if (IsCollection())
			return m_collection;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::Collection))
			return nullptr;
		return v.GetCollection();
	}

	inline_t CollectionItrPair Variant::GetCollectionItr() const
	{
		if (IsCollectionItr())
			return m_collectionItrPair;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::CollectionItr))
			return CollectionItrPair();
		return v.GetCollectionItr();
	}

	inline_t UserObjectPtr Variant::GetUserObject() const
	{
		if (IsUserObject())
			return m_userObject;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::UserObject))
			return nullptr;
		return v.GetUserObject();
	}

	inline_t BufferPtr Variant::GetBuffer() const
	{
		if (IsBuffer())
			return m_buffer;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::Buffer))
			return nullptr;
		return v.GetBuffer();
	}

	inline_t Guid Variant::GetGuid() const
	{
		if (IsGuid())
			return m_guid;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::Guid))
			return NullGuid;
		return v.GetGuid();
	}

	inline_t int64_t Variant::GetInteger() const
	{
		if (IsInteger())
			return m_integer;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::Integer))
			return 0;
		return v.GetInteger();
	}

	inline_t double Variant::GetNumber() const
	{
		if (IsNumber())
			return m_number;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::Number))
			return 0.0;
		return v.GetNumber();
	}

	inline_t String Variant::GetString() const
	{
		if (IsString())
			return m_string;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::String))
			return String();
		return v.GetString();
	}

	inline_t StringU16 Variant::GetStringU16() const
	{
		return Impl::ConvertUtf8ToUtf16(GetString());
	}

	inline_t WString Variant::GetWString() const
	{
		return Impl::ConvertUtf8ToWString(GetString());
	}

	inline_t ValueType Variant::GetValType() const
	{
		if (IsValType())
			return m_valType;
		Variant v = *this;
		if (!v.ConvertTo(ValueType::ValType))
			return ValueType::Null;
		return v.GetValType();
	}

	inline_t bool Variant::IsKeyType() const
	{
		switch (m_type)
		{
			case ValueType::Number:
			case ValueType::Integer:
			case ValueType::Boolean:
			case ValueType::String:
			case ValueType::Guid:
				return true;
			default:
				break;
		};
		return false;
	}

	inline_t bool Variant::IsNumericType() const
	{
		if (m_type == ValueType::Integer || m_type == ValueType::Number)
			return true;
		return false;
	}

	inline_t void Variant::SetBuffer(const BufferPtr & value)
	{
		Destroy();
		if (!value)
		{
			m_type = ValueType::Null;
			m_integer = 0;
		}
		m_type = ValueType::Buffer;
		new(&m_buffer) BufferPtr();
		m_buffer = value;
	}

	inline_t void Variant::SetBoolean(bool value)
	{
		Destroy();
		m_type = ValueType::Boolean;
		m_boolean = value;
	}

	inline_t void Variant::SetCollection(const CollectionPtr & value)
	{
		Destroy();
		if (!value)
		{
			m_type = ValueType::Null;
			m_integer = 0;
		}
		m_type = ValueType::Collection;
		new(&m_collection) CollectionPtr();
		m_collection = value;
	}

	inline_t void Variant::SetCollectionItr(const CollectionItrPair & value)
	{
		assert(value.second);
		Destroy();
		m_type = ValueType::CollectionItr;
		new(&m_collectionItrPair) CollectionItrPair();
		m_collectionItrPair = value;
	}

	inline_t void Variant::SetGuid(const Guid & value)
	{
		Destroy();
		m_type = ValueType::Guid;
		m_guid = value;
	}

	inline_t void Variant::SetInteger(int64_t value)
	{
		Destroy();
		m_type = ValueType::Integer;
		m_integer = value;
	}

	inline_t void Variant::SetNull()
	{
		Destroy();
		m_type = ValueType::Null;
	}

	inline_t void Variant::SetNumber(double value)
	{
		Destroy();
		m_type = ValueType::Number;
		m_number = value;
	}

	inline_t void Variant::SetUserObject(const UserObjectPtr & value)
	{
		Destroy();
		m_type = ValueType::UserObject;
		new(&m_userObject) UserObjectPtr();
		m_userObject = value;
	}

	inline_t void Variant::SetString(const String & value)
	{
		Destroy();
		m_type = ValueType::String;
		new(&m_string) String();
		m_string = value;
	}

	inline_t void Variant::SetString(const StringU16 & value)
	{
		SetString(Impl::ConvertUtf16ToUtf8(value));
	}

	inline_t void Variant::SetString(const WString & value)
	{
		SetString(Impl::ConvertWStringToUtf8(value));
	}

	inline_t void Variant::SetValType(ValueType value)
	{
		Destroy();
		m_type = ValueType::ValType;
		m_valType = value;
	}

	inline_t void Variant::Write(BinaryWriter & writer) const
	{

		// Write out the type
		uint8_t t = Impl::ValueTypeToByte(m_type);
		writer.Write(t);

		switch (m_type)
		{
			case ValueType::Null:
				break;
			case ValueType::Number:
				writer.Write(m_number);
				break;
			case ValueType::Integer:
				writer.Write(m_integer);
				break;
			case ValueType::Boolean:
				writer.Write(m_boolean);
				break;
			case ValueType::String:
				writer.Write(m_string);
				break;
			case ValueType::Collection:
				break;
			case ValueType::CollectionItr:
				break;
			case ValueType::UserObject:
				break;
			case ValueType::Buffer:
				writer.Write(m_buffer);
				break;
			case ValueType::Guid:
				writer.Write(&m_guid, sizeof(m_guid));
				break;
			case ValueType::ValType:
				writer.Write(Impl::ValueTypeToByte(m_valType));
				break;
			default:
				assert(!"Unknown variant type!");
		};

	}

	inline_t void Variant::Read(BinaryReader & reader)
	{
		Destroy();
		uint8_t t;
		reader.Read(&t);
		m_type = Impl::ByteToValueType(t);

		switch (m_type)
		{
			case ValueType::Null:
				break;
			case ValueType::Number:
				reader.Read(&m_number);
				break;
			case ValueType::Integer:
				reader.Read(&m_integer);
				break;
			case ValueType::Boolean:
				reader.Read(&m_boolean);
				break;
			case ValueType::String:
				new(&m_string) String();
				reader.Read(&m_string);
				break;
			case ValueType::Collection:
				break;
			case ValueType::CollectionItr:
				break;
			case ValueType::UserObject:
				break;
			case ValueType::Buffer:
				new(&m_buffer) BufferPtr();
				reader.Read(m_buffer);
				break;
			case ValueType::Guid:
				reader.Read(&m_guid, sizeof(m_guid));
				break;
			case ValueType::ValType:
			{
				uint8_t vt;
				reader.Read(&vt);
				m_valType = Impl::ByteToValueType(vt);
				break;
			}
			default:
				assert(!"Unknown variant type!");
		};

	}

	inline_t Variant operator + (const Variant & left, const Variant & right)
	{
		if (left.GetType() == ValueType::String)
		{
			Variant result;
			result.SetString(left.GetString() + right.GetString());
			return result;
		}
		if (left.GetType() != ValueType::Number && left.GetType() != ValueType::Integer)
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid left operand for addition");
			return Variant();
		}
		if (right.GetType() != ValueType::Number && right.GetType() != ValueType::Integer)
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid right operand for addition");
			return Variant();
		}
		if (left.GetType() == ValueType::Integer && right.GetType() == ValueType::Integer)
		{
			Variant result;
			result.SetInteger(left.GetInteger() + right.GetInteger());
			return result;
		}
		else
		{
			Variant result;
			result.SetNumber(left.GetNumber() + right.GetNumber());
			return result;
		}
	}

	inline_t Variant operator - (const Variant & left, const Variant & right)
	{
		if (!left.IsNumericType())
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid left operand for subtraction");
			return Variant();
		}
		if (!right.IsNumericType())
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid right operand for subtraction");
			return Variant();
		}
		if (left.GetType() == ValueType::Integer && right.GetType() == ValueType::Integer)
		{
			Variant result;
			result.SetInteger(left.GetInteger() - right.GetInteger());
			return result;
		}
		else
		{
			Variant result;
			result.SetNumber(left.GetNumber() - right.GetNumber());
			return result;
		}
	}

	inline_t Variant operator * (const Variant & left, const Variant & right)
	{
		if (!left.IsNumericType())
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid left operand for multiplication");
			return Variant();
		}
		if (!right.IsNumericType())
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid right operand for multiplication");
			return Variant();
		}
		if (left.GetType() == ValueType::Integer && right.GetType() == ValueType::Integer)
		{
			Variant result;
			result.SetInteger(left.GetInteger() * right.GetInteger());
			return result;
		}
		else
		{
			Variant result;
			result.SetNumber(left.GetNumber() * right.GetNumber());
			return result;
		}
	}

	inline_t Variant operator / (const Variant & left, const Variant & right)
	{
		if (!left.IsNumericType())
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid left operand for division");
			return Variant();
		}
		if (!right.IsNumericType())
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid right operand for division");
			return Variant();
		}
		if (left.GetType() == ValueType::Integer && right.GetType() == ValueType::Integer)
		{
			Variant result;
			int64_t l = left.GetInteger();
			int64_t r = right.GetInteger();
			if (l % r == 0)
				result.SetInteger(l / r);
			else
				result.SetNumber(double(l) / (double)r);
			return result;
		}
		else
		{
			Variant result;
			result.SetNumber(left.GetNumber() / right.GetNumber());
			return result;
		}
	}

	inline_t Variant operator % (const Variant & left, const Variant & right)
	{
		// Handle floating-point numbers with fmod function
		if (left.GetType() == ValueType::Number || right.GetType() == ValueType::Number)
		{
			auto l = left.GetNumber();
			auto r = right.GetNumber();
			return fmod(fmod(l, r) + r, r);
		}

		// Check for non-integer types, especially since co-erced right values will be zero
		if (left.GetType() != ValueType::Integer)
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid left operand for mod");
			return Variant();
		}
		if (right.GetType() != ValueType::Integer)
		{
			Impl::LogWriteLine(LogLevel::Error, "Invalid right operand for mod");
			return Variant();
		}

		// Return result from integer mod operation
		auto l = left.GetInteger();
		auto r = right.GetInteger();
		return ((l % r) + r) % r;
	}

	inline_t bool operator == (const Variant & left, const Variant & right)
	{
		switch (left.GetType())
		{
			case ValueType::Null:
			{
				return right.IsNull();
			}
			case ValueType::Number:
			{
				return left.GetNumber() == right.GetNumber();
			}
			case ValueType::Integer:
			{
				if (!right.IsNumericType())
					return false;
				if (right.IsNumber())
					return left.GetNumber() == right.GetNumber();
				else
					return left.GetInteger() == right.GetInteger();
			}
			case ValueType::Boolean:
			{
				if (!right.IsBoolean())
					return false;
				return left.GetBoolean() == right.GetBoolean();
			}
			case ValueType::String:
			{
				if (!right.IsString())
					return false;
				return left.GetString() == right.GetString();
			}
			case ValueType::Collection:
			{
				if (!right.IsCollection())
					return false;
				return left.GetCollection() == right.GetCollection();
			}
			case ValueType::CollectionItr:
			{
				if (!right.IsCollectionItr())
					return false;
				return left.GetCollectionItr() == right.GetCollectionItr();
			}
			case ValueType::UserObject:
			{
				if (!right.IsUserObject())
					return false;
				return left.GetUserObject() == right.GetUserObject();
			}
			case ValueType::Buffer:
			{
				if (!right.IsBuffer())
					return false;
				return left.GetBuffer() == right.GetBuffer();
			}
			case ValueType::Guid:
			{
				if (!right.IsGuid())
					return false;
				return left.GetGuid() == right.GetGuid();
			}
			case ValueType::ValType:
			{
				if (!right.IsValType())
					return false;
				return left.GetValType() == right.GetValType();
			}
			default:
			{
				assert(!"Unknown variant type!");
			}
		};

		return false;
	}

	inline_t bool operator < (const Variant & left, const Variant & right)
	{
		switch (left.GetType())
		{
			case ValueType::Null:
			{
				Impl::LogWriteLine(LogLevel::Error, "Error comparing null type with < operator");
				return false;
			}
			case ValueType::Number:
			{
				if (!right.IsNumericType())
					break;
				return left.GetNumber() < right.GetNumber();
			}
			case ValueType::Integer:
			{
				if (!right.IsNumericType())
					break;
				if (right.IsNumber())
					return left.GetNumber() < right.GetNumber();
				else
					return left.GetInteger() < right.GetInteger();
			}
			case ValueType::Boolean:
			{
				if (!right.IsBoolean())
					break;
				return left.GetBoolean() < right.GetBoolean();
			}
			case ValueType::String:
			{
				if (!right.IsString())
					break;
				return left.GetString() < right.GetString();
			}
			case ValueType::Collection:
			{
				Impl::LogWriteLine(LogLevel::Error, "Error comparing collection type with < operator");
				return false;
			}
			case ValueType::CollectionItr:
			{
				Impl::LogWriteLine(LogLevel::Error, "Error comparing collectionitr type with < operator");
				return false;
			}
			case ValueType::UserObject:
			{
				if (!right.IsUserObject())
					break;
				return left.GetUserObject() < right.GetUserObject();
			}
			case ValueType::Buffer:
			{
				Impl::LogWriteLine(LogLevel::Error, "Error comparing buffer type with < operator");
				return false;
			}
			case ValueType::Guid:
			{
				if (!right.IsGuid())
					break;
				return left.GetGuid() < right.GetGuid();
			}
			case ValueType::ValType:
			{
				if (!right.IsValType())
					break;
				return left.GetValType() < right.GetValType();
			}
			default:
			{
				assert(!"Unknown variant type!");
			}
		};

		Impl::LogWriteLine(LogLevel::Error, "Type error in right operand when using < operator");
		return false;
	}

	inline_t bool operator <= (const Variant & left, const Variant & right)
	{
		switch (left.GetType())
		{
			case ValueType::Null:
			{
				Impl::LogWriteLine(LogLevel::Error, "Error comparing null type with < operator");
				return false;
			}
			case ValueType::Number:
			{
				if (!right.IsNumericType())
					break;
				return left.GetNumber() <= right.GetNumber();
			}
			case ValueType::Integer:
			{
				if (!right.IsNumericType())
					break;
				if (right.IsNumber())
					return left.GetNumber() <= right.GetNumber();
				else
					return left.GetInteger() <= right.GetInteger();
			}
			case ValueType::Boolean:
			{
				if (!right.IsBoolean())
					break;
				return left.GetBoolean() <= right.GetBoolean();
			}
			case ValueType::String:
			{
				if (!right.IsNumericType())
					break;
				return left.GetString() <= right.GetString();
			}
			case ValueType::Collection:
			{
				Impl::LogWriteLine(LogLevel::Error, "Error comparing collection type with < operator");
				return false;
			}
			case ValueType::CollectionItr:
			{
				Impl::LogWriteLine(LogLevel::Error, "Error comparing collectionitr type with < operator");
				return false;
			}
			case ValueType::UserObject:
			{
				if (!right.IsUserObject())
					break;
				return left.GetUserObject() <= right.GetUserObject();
			}
			case ValueType::Buffer:
			{
				Impl::LogWriteLine(LogLevel::Error, "Error comparing buffer type with < operator");
				return false;
			}
			case ValueType::Guid:
			{
				if (!right.IsGuid())
					break;
				return left.GetGuid() <= right.GetGuid();
			}
			case ValueType::ValType:
			{
				if (!right.IsValType())
					break;
				return left.GetValType() <= right.GetValType();
			}
			default:
			{
				assert(!"Unknown variant type!");
			}
		};

		Impl::LogWriteLine(LogLevel::Error, "Type error in right operand when using < operator");
		return false;
	}

	inline_t bool ValidateValueComparison(const Variant & left, const Variant & right)
	{
		if (!Impl::CheckValueTypeForCompare(left.GetType()))
			return false;
		if (!Impl::CheckValueTypeForCompare(right.GetType()))
			return false;
		if (left.GetType() != right.GetType())
		{
			if (!left.IsNumericType() || !right.IsNumericType())
				return false;
		}
		return true;
	}

} // namespace Jinx

