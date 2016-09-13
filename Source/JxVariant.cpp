/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

using namespace Jinx;


Variant::Variant(const Variant & copy)
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
		new(&m_collectionItr) CollectionItr();
		m_collectionItr = copy.m_collectionItr;
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

Variant::~Variant()
{
	Destroy();
}

Variant & Variant::operator= (const Variant & copy)
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
		new(&m_collectionItr) CollectionItr();
		m_collectionItr = copy.m_collectionItr;
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

Variant & Variant::operator ++()
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
		++m_collectionItr;
		break;
	default:
		break;
	};
	return *this;
}

Variant & Variant::operator += (const Variant & right)
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

Variant & Variant::operator --()
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

Variant & Variant::operator -= (const Variant & right)
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

bool Variant::ConvertTo(ValueType type)
{
	switch (m_type)
	{
	case ValueType::Null:
		{
			switch (type)
			{
			case ValueType::Null:
				return true;
			case ValueType::Number:
				SetNumber(NullToNumber());
				return true;
			case ValueType::Integer:
				SetInteger(NullToInteger());
				return true;
			case ValueType::Boolean:
				SetBoolean(NullToBoolean());
				return true;
			case ValueType::String:
				SetString(NullToString());
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
		case ValueType::Null:
			SetNull();
			return true;
		case ValueType::Number:
			return true;
		case ValueType::Integer:
			SetInteger(NumberToInteger(m_number));
			return true;
		case ValueType::Boolean:
			SetBoolean(NumberToBoolean(m_number));
			return true;
		case ValueType::String:
			SetString(NumberToString(m_number));
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
		case ValueType::Null:
			SetNull();
			return true;
		case ValueType::Number:
			SetNumber(IntegerToNumber(m_integer));
			return true;
		case ValueType::Integer:
			return true;
		case ValueType::Boolean:
			SetBoolean(IntegerToBoolean(m_integer));
			return true;
		case ValueType::String:
			SetString(IntegerToString(m_integer));
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
		case ValueType::Null:
			SetNull();
			return true;
		case ValueType::Number:
			SetNumber(BooleanToNumber(m_boolean));
			return true;
		case ValueType::Integer:
			SetInteger(BooleanToInteger(m_boolean));
			return true;
		case ValueType::Boolean:
			return true;
		case ValueType::String:
			SetString(BooleanToString(m_boolean));
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
		case ValueType::Null:
			SetNull();
			return true;
		case ValueType::Number:
		{
			double number;
			if (!StringToNumber(m_string, &number))
			{
				LogWriteLine("Error converting string %s to number", m_string.c_str());
				return false;
			}
			SetNumber(number);
			return true;
		}
		case ValueType::Integer:
		{
			int64_t integer;
			if (!StringToInteger(m_string, &integer))
			{
				LogWriteLine("Error converting string %s to integer", m_string.c_str());
				return false;
			}
			SetInteger(integer);
			return true;
		}
		case ValueType::Boolean:
		{
			bool boolean;
			if (!StringToBoolean(m_string, &boolean))
			{
				LogWriteLine("Error converting string %s to boolean", m_string.c_str());
				return false;
			}
			SetBoolean(boolean);
			return true;
		}
		case ValueType::String:
			return true;
		default:
			break;
		};
	}
	break;
	case ValueType::Collection:
		break;
	case ValueType::CollectionItr:
		break;
	case ValueType::UserObject:
		break;
	case ValueType::Buffer:
		break;
	case ValueType::Guid:
		break;
	case ValueType::ValType:
		break;
	default:
		break;
	};
	LogWriteLine("Error converting %s to %s", GetValueTypeName(m_type), GetValueTypeName(type));
	return false;
}

void Variant::Destroy()
{
	if (m_type == ValueType::String)
	{
		m_string.~String();
	}
	else if (m_type == ValueType::Buffer)
	{
		m_buffer.~BufferPtr();
	}
	else if (m_type == ValueType::Collection)
	{
		m_collection.~CollectionPtr();
	}
	else if (m_type == ValueType::CollectionItr)
	{
		m_collectionItr.~CollectionItr();
	}
	else if (m_type == ValueType::UserObject)
	{
		m_userObject.~UserObjectPtr();
	}
	m_type = ValueType::Null;
}

bool Variant::GetBoolean() const
{
	switch (m_type)
	{
	case ValueType::Null:
		return false;
	case ValueType::Number:
		return m_number != 0.0;
	case ValueType::Integer:
		return m_integer != 0;
	case ValueType::Boolean:
		return m_boolean;
	case ValueType::String:
		return m_string.length() != 0;
	case ValueType::Collection:
		return !m_collection->empty();
	case ValueType::CollectionItr:
		break;
	default:
		break;
	};
	return false;
}

CollectionPtr Variant::GetCollection() const
{
	switch (m_type)
	{
	case ValueType::Collection:
		return m_collection;
	default:
		break;
	};
	return nullptr;
}

CollectionItr Variant::GetCollectionItr() const
{
	switch (m_type)
	{
	case ValueType::CollectionItr:
		return m_collectionItr;
	default:
		break;
	};
	return CollectionItr();
}

UserObjectPtr Variant::GetUserObject() const
{
	if (m_type == ValueType::UserObject)
		return m_userObject;
	return nullptr;
}

BufferPtr Variant::GetBuffer() const
{
	if (m_type == ValueType::Buffer)
		return m_buffer;
	return nullptr;
}

Guid Variant::GetGuid() const
{
	switch (m_type)
	{
	case ValueType::Guid:
		return m_guid;
	default:
		break;
	};
	return NullGuid;
}

int64_t Variant::GetInteger() const
{
	switch (m_type)
	{
	case ValueType::Number:
		return static_cast<int64_t>(m_number);
	case ValueType::Integer:
		return m_integer;
	case ValueType::Boolean:
		return m_boolean ? 1 : 0;
	case ValueType::String:
	{
		int64_t integer;
		if (!StringToInteger(m_string, &integer))
			return 0;
		return integer;
	}
	default:
		break;
	};
	return 0;
}

double Variant::GetNumber() const
{
	switch (m_type)
	{
	case ValueType::Number:
		return m_number;
	case ValueType::Integer:
		return static_cast<double>(m_integer);
	case ValueType::Boolean:
		return m_boolean ? 1.0 : 0.0;
	case ValueType::String:
	{
		double number;
		if (!StringToNumber(m_string, &number))
			return 0.0;
		return number;
	}
	default:
		break;
	};
	return 0.0;
}

String Variant::GetString() const
{
	switch (m_type)
	{
	case ValueType::Null:
		return NullToString();
	case ValueType::Number:
		return NumberToString(m_number);
	case ValueType::Integer:
		return IntegerToString(m_integer);
	case ValueType::Boolean:
		return BooleanToString(m_boolean);
	case ValueType::String:
		return m_string;
	case ValueType::ValType:
		return GetValueTypeName(m_valType);
	default:
		break;
	};
	return String();
}

StringU16 Variant::GetStringU16() const
{
	return ConvertUtf8ToUtf16(GetString());
}

ValueType Variant::GetValType() const
{
	switch (m_type)
	{
	case ValueType::ValType:
		return m_valType;
	default:
		break;
	};
	return ValueType::Null;
}

bool Variant::IsKeyType() const
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

void Variant::SetBuffer(const BufferPtr & value)
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

void Variant::SetBoolean(bool value)
{
	Destroy();
	m_type = ValueType::Boolean;
	m_boolean = value;
}

void Variant::SetCollection(const CollectionPtr & value)
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

void Variant::SetCollectionItr(const CollectionItr & value)
{
	Destroy();
	m_type = ValueType::CollectionItr;
	new(&m_collectionItr) CollectionItr();
	m_collectionItr = value;
}

void Variant::SetGuid(const Guid & value)
{
	Destroy();
	m_type = ValueType::Guid;
	m_guid = value;
}

void Variant::SetInteger(int64_t value)
{
	Destroy();
	m_type = ValueType::Integer;
	m_integer = value;
}

void Variant::SetNull()
{
	Destroy();
	m_type = ValueType::Null;
}

void Variant::SetNumber(double value)
{
	Destroy();
	m_type = ValueType::Number;
	m_number = value;
}

void Variant::SetUserObject(const UserObjectPtr & value)
{
	Destroy();
	m_type = ValueType::UserObject;
	new(&m_userObject) UserObjectPtr();
	m_userObject = value;
}

void Variant::SetString(const String & value)
{
	Destroy();
	m_type = ValueType::String;
	new(&m_string) String();
	m_string = value;
}

void Variant::SetString(const StringU16 & value)
{
	SetString(ConvertUtf16ToUtf8(value));
}

void Variant::SetValType(ValueType value)
{
	Destroy();
	m_type = ValueType::ValType;
	m_valType = value;
}

void Variant::Write(BinaryWriter & writer) const
{

	// Write out the type
	uint8_t t = ValueTypeToByte(m_type);
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
		writer.Write(ValueTypeToByte(m_valType));
		break;
	default:
		assert(!"Unknown variant type!");
	};

}

void Variant::Read(BinaryReader & reader)
{
	Destroy();
	uint8_t t;
	reader.Read(&t);
	m_type = ByteToValueType(t);

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
		m_valType = ByteToValueType(vt);
		break;
	}
	default:
		assert(!"Unknown variant type!");
	};

}

Variant Jinx::operator + (const Variant & left, const Variant & right)
{
	if (left.GetType() == ValueType::String)
	{
		Variant result;
		result.SetString(left.GetString() + right.GetString());
		return result;
	}
	if (left.GetType() != ValueType::Number && left.GetType() != ValueType::Integer)
	{
		LogWriteLine("Invalid left operand for addition");
		return Variant();
	}
	if (right.GetType() != ValueType::Number && right.GetType() != ValueType::Integer)
	{
		LogWriteLine("Invalid right operand for addition");
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

Variant Jinx::operator - (const Variant & left, const Variant & right)
{
	if (left.GetType() != ValueType::Number && left.GetType() != ValueType::Integer)
	{
		LogWriteLine("Invalid left operand for subtraction");
		return Variant();
	}
	if (right.GetType() != ValueType::Number && right.GetType() != ValueType::Integer)
	{
		LogWriteLine("Invalid right operand for subtraction");
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

Variant Jinx::operator * (const Variant & left, const Variant & right)
{
	if (left.GetType() != ValueType::Number && left.GetType() != ValueType::Integer)
	{
		LogWriteLine("Invalid left operand for multiplication");
		return Variant();
	}
	if (right.GetType() != ValueType::Number && right.GetType() != ValueType::Integer)
	{
		LogWriteLine("Invalid right operand for multiplication");
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

Variant Jinx::operator / (const Variant & left, const Variant & right)
{
	if (left.GetType() != ValueType::Number && left.GetType() != ValueType::Integer)
	{
		LogWriteLine("Invalid left operand for division");
		return Variant();
	}
	if (right.GetType() != ValueType::Number && right.GetType() != ValueType::Integer)
	{
		LogWriteLine("Invalid right operand for division");
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

Variant Jinx::operator % (const Variant & left, const Variant & right)
{
	// Handle floating-point numbers with fmod function
	if (left.GetType() == ValueType::Number || right.GetType() == ValueType::Number)
		return fmod(left.GetNumber(), right.GetNumber());

	// Check for non-integer types, especially since co-erced right values will be zero
	if (left.GetType() != ValueType::Integer)
	{
		LogWriteLine("Invalid left operand for mod");
		return Variant();
	}
	if (right.GetType() != ValueType::Integer)
	{
		LogWriteLine("Invalid right operand for mod");
		return Variant();
	}

	// Return result from integer mod operation
	return Variant(left.GetInteger() % right.GetInteger());
}

bool Jinx::operator == (const Variant & left, const Variant & right)
{
	switch (left.GetType())
	{
	case ValueType::Null:
		return right.IsNull();
	case ValueType::Number:
		return left.GetNumber() == right.GetNumber();
	case ValueType::Integer:
		return left.GetInteger() == right.GetInteger();
	case ValueType::Boolean:
		return left.GetBoolean() == right.GetBoolean();
	case ValueType::String:
		return left.GetString() == right.GetString();
	case ValueType::Collection:
		return left.GetCollection() == right.GetCollection();
	case ValueType::CollectionItr:
		return left.GetCollectionItr() == right.GetCollectionItr();
	case ValueType::UserObject:
		return left.GetUserObject() == right.GetUserObject();
	case ValueType::Buffer:
		return left.GetBuffer() == right.GetBuffer();
	case ValueType::Guid:
		return left.GetGuid() == right.GetGuid();
	case ValueType::ValType:
		return left.GetValType() == right.GetValType();
	default:
		assert(!"Unknown variant type!");
	};

	return false;
}

bool Jinx::operator < (const Variant & left, const Variant & right)
{
	switch (left.GetType())
	{
	case ValueType::Null:
		return false;
	case ValueType::Number:
		return left.GetNumber() < right.GetNumber();
	case ValueType::Integer:
		return left.GetInteger() < right.GetInteger();
	case ValueType::Boolean:
		return left.GetBoolean() < right.GetBoolean();
	case ValueType::String:
		return left.GetString() < right.GetString();
	case ValueType::Collection:
		return left.GetCollection() < right.GetCollection();
	case ValueType::CollectionItr:
		return left.GetCollectionItr() < right.GetCollectionItr();
	case ValueType::UserObject:
		return left.GetUserObject() < right.GetUserObject();
	case ValueType::Buffer:
		return left.GetBuffer() < right.GetBuffer();
	case ValueType::Guid:
		return left.GetGuid() < right.GetGuid();
	case ValueType::ValType:
		return left.GetValType() < right.GetValType();
	default:
		assert(!"Unknown variant type!");
	};

	return false;
}

bool Jinx::operator <= (const Variant & left, const Variant & right)
{
	switch (left.GetType())
	{
	case ValueType::Null:
		return false;
	case ValueType::Number:
		return left.GetNumber() <= right.GetNumber();
	case ValueType::Integer:
		return left.GetInteger() <= right.GetInteger();
	case ValueType::Boolean:
		return left.GetBoolean() <= right.GetBoolean();
	case ValueType::String:
		return left.GetString() <= right.GetString();
	case ValueType::Collection:
		return left.GetCollection() <= right.GetCollection();
	case ValueType::CollectionItr:
		return left.GetCollectionItr() <= right.GetCollectionItr();
	case ValueType::UserObject:
		return left.GetUserObject() <= right.GetUserObject();
	case ValueType::Buffer:
		return left.GetBuffer() <= right.GetBuffer();
	case ValueType::Guid:
		return left.GetGuid() <= right.GetGuid();
	case ValueType::ValType:
		return left.GetValType() < right.GetValType();
	default:
		assert(!"Unknown variant type!");
	};

	return false;
}
