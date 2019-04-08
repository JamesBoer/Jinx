/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_CONVERSION_H__
#define JX_CONVERSION_H__


namespace Jinx::Impl
{

	// Convert value type to byte and back
	ValueType ByteToValueType(uint8_t byte);
	uint8_t ValueTypeToByte(ValueType type);

	// Null conversions
	constexpr inline double NullToNumber() { return 0.0; }
	constexpr inline int64_t NullToInteger() { return 0ll; }
	constexpr inline bool NullToBoolean() { return false; }
	inline String NullToString() { return String("null"); }

	// Number conversions
	inline int64_t NumberToInteger(double value) { return static_cast<int64_t>(value); }
	inline bool NumberToBoolean(double value) { return value != 0.0f ? true : false; }
	inline String NumberToString(double value)
	{
		char buffer[32];
		snprintf(buffer, 32, "%.6f", value);
		return String(buffer);
	}

	// Integer conversions
	inline double IntegerToNumber(int64_t value) { return static_cast<double>(value); }
	inline bool IntegerToBoolean(int64_t value) { return value != 0 ? true : false; }
	inline String IntegerToString(int64_t value)
	{
		char buffer[32];
		snprintf(buffer, 32, "%" PRId64, value);
		return String(buffer);
	}

	// Boolean conversions
	inline double BooleanToNumber(bool value) { return value ? 1.0 : 0.0; }
	inline int64_t BooleanToInteger(bool value) { return value ? 1ll : 0ll; }
	inline String BooleanToString(bool value) { return value ? String("true") : String("false"); }

	enum class NumericFormat
	{
		International, // Uses dot as decimal indicator
		Continental    // Uses comma as decimal indicator
	};

	// String conversions
	bool StringToBoolean(const String & inValue, bool * outValue);
	bool StringToNumber(const String & value, double * outValue, NumericFormat format = NumericFormat::International);
	bool StringToInteger(const String & value, int64_t * outValue);
	bool StringToValueType(const String & value, ValueType * outValue);
	bool StringToGuid(const String & value, Guid * outValue);
	bool StringToCollection(const String & value, CollectionPtr * outValue);

	// GUID conversions
	String GuidToString(const Guid & value);

} // namespace Jinx::Impl

#endif // JX_CONVERSION_H__

