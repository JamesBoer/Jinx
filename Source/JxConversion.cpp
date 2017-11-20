/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

using namespace Jinx;

static uint8_t s_valueTypeToByte[] =
{
	0,  // Null,
	1,  // Number,
	2,  // Integer,
	3,  // Boolean,
	4,  // String,
	5,  // Collection,
	6,  // CollectionItr,
	7,  // UserData,
	8,  // Buffer,
	9,  // Guid,
	10, // ValType,
	11, // Any
};

static_assert(countof(s_valueTypeToByte) == (static_cast<size_t>(ValueType::NumValueTypes) + 1), "ValueType names don't match enum count");

static ValueType s_byteToValueType[] =
{
	ValueType::Null,
	ValueType::Number,
	ValueType::Integer,
	ValueType::Boolean,
	ValueType::String,
	ValueType::Collection,
	ValueType::CollectionItr,
	ValueType::UserObject,
	ValueType::Buffer,
	ValueType::Guid,
	ValueType::ValType,
	ValueType::Any,
};

static_assert(countof(s_byteToValueType) == (static_cast<size_t>(ValueType::NumValueTypes) + 1), "ValueType names don't match enum count");

ValueType Jinx::ByteToValueType(uint8_t byte)
{
	assert(byte <= static_cast<size_t>(ValueType::NumValueTypes));
	return s_byteToValueType[byte];
}

uint8_t Jinx::ValueTypeToByte(ValueType type)
{
	assert(static_cast<size_t>(type) <= static_cast<size_t>(ValueType::NumValueTypes));
	return s_valueTypeToByte[static_cast<size_t>(type)];
}

bool Jinx::StringToBoolean(const String & inValue, bool * outValue)
{
	assert(outValue);
	if (strcmp(inValue.c_str(), "true") == 0)
	{
		*outValue = true;
		return true;
	}
	else if (strcmp(inValue.c_str(), "false") == 0)
	{
		*outValue = false;
		return true;
	}
	return false;
}

bool Jinx::StringToNumber(const String & value, double * outValue)
{
	assert(outValue);
	const char * s = value.c_str();
	const char * p = s;
	bool decimal = false;
	int count = 0;
	if (*p == '-' || *p == '+')
		++p;
	while (*p != '\0')
	{
		if (*p < 0)
			return false;
		if (std::isdigit(*p))
		{
			++count;
			if (count > 16)
				return false;
		}
		else if (*p == '.')
		{
			if (decimal)
				return false;
			decimal = true;
		}
		else
			return false;
		++p;
	}
	*outValue = std::atof(s);
	return count > 0;
}

bool Jinx::StringToInteger(const String & value, int64_t * outValue)
{
	assert(outValue);
	const char * s = value.c_str();
	const char * p = s;
	int count = 0;
	if (*p == '-' || *p == '+')
		++p;
	while (*p != '\0')
	{
		if (*p < 0)
			return false;
		if (std::isdigit(*p))
		{
			++count;
			if (count > 18)
				return false;
		}
		else
			return false;
		++p;
	}
	*outValue = std::strtoull(s, nullptr, 10);
	return count > 0;
}

bool Jinx::StringToValueType(const String & value, ValueType * outValue)
{
	if (value == "null")
	{
		*outValue = ValueType::Null;
		return true;
	}
	else if (value == "number")
	{
		*outValue = ValueType::Number;
		return true;
	}
	else if (value == "integer")
	{
		*outValue = ValueType::Integer;
		return true;
	}
	else if (value == "boolean")
	{
		*outValue = ValueType::Boolean;
		return true;
	}
	else if (value == "string")
	{
		*outValue = ValueType::String;
		return true;
	}
	else if (value == "collection")
	{
		*outValue = ValueType::Collection;
		return true;
	}
	else if (value == "collectionitr")
	{
		*outValue = ValueType::CollectionItr;
		return true;
	}
	else if (value == "buffer")
	{
		*outValue = ValueType::Buffer;
		return true;
	}
	else if (value == "guid")
	{
		*outValue = ValueType::Guid;
		return true;
	}
	else if (value == "valtype")
	{
		*outValue = ValueType::ValType;
		return true;
	}
	return false;
}

bool Jinx::StringToGuid(const String & value, Guid * outValue)
{
	assert(outValue);
	Guid guid;
	unsigned long p0;
	int p1, p2, p3, p4, p5, p6, p7, p8, p9, p10;
#ifdef JINX_WINDOWS
	int err = sscanf_s(value.c_str(), "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		&p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10);
#else
	int err = sscanf(value.c_str(), "%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		&p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7, &p8, &p9, &p10);
#endif
	if (err != 11)
		return false;
	guid.data1 = static_cast<uint32_t>(p0);
	guid.data2 = static_cast<uint16_t>(p1);
	guid.data3 = static_cast<uint16_t>(p2);
	guid.data4[0] = static_cast<uint8_t>(p3);
	guid.data4[1] = static_cast<uint8_t>(p4);
	guid.data4[2] = static_cast<uint8_t>(p5);
	guid.data4[3] = static_cast<uint8_t>(p6);
	guid.data4[4] = static_cast<uint8_t>(p7);
	guid.data4[5] = static_cast<uint8_t>(p8);
	guid.data4[6] = static_cast<uint8_t>(p9);
	guid.data4[7] = static_cast<uint8_t>(p10);
	*outValue = guid;
	return true;
}

String Jinx::GuidToString(const Guid & value)
{
	char buffer[64];
	snprintf(
		buffer,
		countof(buffer),
		"%.*X-%.*X-%.*X-%.*X%.*X-%.*X%.*X%.*X%.*X%.*X%.*X",
		8, value.data1,
		4, value.data2,
		4, value.data3,
		2, value.data4[0],
		2, value.data4[1],
		2, value.data4[2],
		2, value.data4[3],
		2, value.data4[4],
		2, value.data4[5],
		2, value.data4[6],
		2, value.data4[7]
	);
	return String(buffer);
}


