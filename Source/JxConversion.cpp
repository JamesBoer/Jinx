/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

namespace Jinx::Impl
{

	struct ConvTable
	{
		static inline uint8_t valueTypeToByte[] =
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

		static_assert(countof(valueTypeToByte) == (static_cast<size_t>(ValueType::NumValueTypes) + 1), "ValueType names don't match enum count");

		static inline ValueType byteToValueType[] =
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

		static_assert(countof(byteToValueType) == (static_cast<size_t>(ValueType::NumValueTypes) + 1), "ValueType names don't match enum count");
	};

	inline_t ValueType ByteToValueType(uint8_t byte)
	{
		assert(byte <= static_cast<size_t>(ValueType::NumValueTypes));
		return ConvTable::byteToValueType[byte];
	}

	inline_t uint8_t ValueTypeToByte(ValueType type)
	{
		assert(static_cast<size_t>(type) <= static_cast<size_t>(ValueType::NumValueTypes));
		return ConvTable::valueTypeToByte[static_cast<size_t>(type)];
	}

	inline_t bool StringToBoolean(const String & inValue, bool * outValue)
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

	inline_t bool StringToNumber(const String & value, double * outValue, NumericFormat format)
	{
		assert(outValue);
#ifdef JINX_USE_FROM_CHARS
		// In case contintental format is used, replace commas with decimal point
		if (format == NumericFormat::Continental)
		{
			String s = value;
			std::replace(s.begin(), s.end(), ',', '.');
			auto result = std::from_chars(s.data(), s.data() + s.size(), *outValue);
			if (result.ptr == s.data() + s.size())
				return true;
		}
		else
		{
			auto result = std::from_chars(value.data(), value.data() + value.size(), *outValue);
			if (result.ptr == value.data() + value.size())
				return true;
		}
		return false;
#else
		// In case contintental format is used, replace commas with decimal point
		if (format == NumericFormat::Continental)
		{
			String s = value;
			std::replace(s.begin(), s.end(), ',', '.');
			std::istringstream istr(s.c_str());
			istr.imbue(std::locale::classic());
			istr >> *outValue;
			if (istr.fail())
				return false;
		}
		else
		{
			std::istringstream istr(value.c_str());
			istr.imbue(std::locale::classic());
			istr >> *outValue;
			if (istr.fail())
				return false;
		}
		return true;
#endif
	}

	inline_t bool StringToInteger(const String & value, int64_t * outValue)
	{
		assert(outValue);
#ifdef JINX_USE_FROM_CHARS
		auto result = std::from_chars(value.data(), value.data() + value.size(), *outValue);
		if (result.ptr == value.data() + value.size())
			return true;
		return false;
#else
		char * endPtr;
		*outValue = strtoll(value.data(), &endPtr, 10);
		if (endPtr != (value.data() + value.size()))
			return false;
		return true;
#endif
	}

	inline_t bool StringToValueType(const String & value, ValueType * outValue)
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

	inline_t bool StringToGuid(const String & value, Guid * outValue)
	{
		assert(outValue);
		Guid guid;
		unsigned long p0;
		unsigned int p1, p2, p3, p4, p5, p6, p7, p8, p9, p10;
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

	inline_t bool GetDelimiterAndFormat(const String & value, char & delimiter, NumericFormat & format)
	{
		size_t tabCount = 0;
		size_t commaCount = 0;
		size_t semicolonCount = 0;
		for (size_t i = 0; i < value.size(); ++i)
		{
			char c = value[i];
			if (c == '\t')
				++tabCount;
			else if (c == ',')
				++commaCount;
			else if (c == ';')
				++semicolonCount;
			else if (c == '\n' && i != 0)
				break;
		}
		if (tabCount == 0 && commaCount == 0 && semicolonCount == 0)
			return false;
		delimiter = (commaCount >= tabCount) ? ((commaCount > semicolonCount) ? ',' : ';') : '\t';
		format = delimiter == ';' ? NumericFormat::Continental : NumericFormat::International;
		return true;
	}

	inline bool IsLineEnd(const char current)
	{
		return current == '\n' || current == '\r';
	}

	inline_t void ParseWhitespace(const char ** current, const char * end)
	{
		while (**current != *end)
		{
			char c = **current;
			if (c != '\t' && c != ' ' && c != '\r' && c != '\n')
				break;
			++(*current);
		}
	}

	inline_t Variant ParseValue(const String & value, NumericFormat format)
	{
		Guid guid;
		if (StringToGuid(value, &guid))
			return guid;
		int64_t integer;
		if (StringToInteger(value, &integer))
			return integer;
		double number;
		if (StringToNumber(value, &number, format))
			return number;
		bool boolean;
		if (StringToBoolean(value, &boolean))
			return boolean;
		return value;
	}

	inline_t String ParseCell(char delimiter, const char ** current, const char * end)
	{
		if (*current == end)
			return String();
		bool isQuoted = false;
		if (**current == '"')
		{
			isQuoted = true;
			++(*current);
		}
		String value;
		while (*current != end)
		{
			const char c = **current;
			if (isQuoted)
			{
				// Since this cell is double-quote delimited, proceed without checking delimiters until
				// we see another double quote character.
				if (c == '"')
				{
					// Advance the iterator and check to see if it's followed by the end of file or
					// delimiters.  If so, we're done parsing.  If not, a second double-quote should
					// follow.
					++(*current);
					if (*current == end || **current == delimiter || IsLineEnd(**current))
						break;
					// If this assert hits, your data is malformed, since an interior double-quote was not
					// followed by a second quote
					assert(**current == '"');
				}
			}
			else
			{
				// This isn't a quote-escaped cell, so check for normal delimiters
				if (c == delimiter || IsLineEnd(c))
					break;
			}
			value += c;
			++(*current);
		}
		return value;
	}

	inline_t std::vector<Variant, Jinx::Allocator<Variant>> ParseRow(char delimiter, NumericFormat format, const char ** current, const char * end)
	{
		std::vector<Variant, Jinx::Allocator<Variant>> variants;

		while (*current != end)
		{
			auto str = ParseCell(delimiter, current, end);
			auto val = ParseValue(str, format);
			variants.push_back(val);
			if (*current != end)
			{
				if (**current == delimiter)
				{
					++(*current);
				}
				else if (IsLineEnd(**current))
				{
					if (*current != end)
					{
						const char nc = *((*current) + 1);
						if (IsLineEnd(nc))
							++(*current);
					}
					++(*current);
					break;
				}
			}
		}

		return variants;
	}

	inline_t bool StringToCollection(const String & value, CollectionPtr * outValue)
	{

		// First check what type of delimiter is used, tabs or semicolons
		char delimiter;
		NumericFormat format;
		if (!GetDelimiterAndFormat(value, delimiter, format))
			return false;

		// Parse first row, which we'll uses as index values into each subsequent row
		const char * current = value.data();
		const char * end = current + value.size();
		ParseWhitespace(&current, end);
		auto header = ParseRow(delimiter, format, &current, end);
		if (header.empty())
			return false;

		*outValue = CreateCollection();
		CollectionPtr coll = *outValue;

		// Parse and create a collection for each row, and assign column ids from header
		while (true)
		{
			const auto & row = ParseRow(delimiter, format, &current, end);
			if (row.empty())
				break;
			if (row.size() != header.size())
			{
				*outValue = nullptr;
				return false;
			}
			const auto & rowName = row[0];
			auto rowColl = CreateCollection();
			coll->insert({rowName, rowColl });
			for (size_t i = 0; i < row.size(); ++i)
			{
				const auto & headerName = header[i];
				const auto & rowVal = row[i];
				rowColl->insert({ headerName, rowVal });
			}
		}

		return true;
	}


	inline_t String GuidToString(const Guid & value)
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

} // namespace Jinx::Impl



