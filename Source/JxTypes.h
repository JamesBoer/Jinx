/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_TYPES_H__
#define JX_TYPES_H__

namespace Jinx
{

	struct Guid
	{
		uint32_t data1;
		uint16_t  data2;
		uint16_t  data3;
		uint8_t  data4[8];
	};

	const Guid NullGuid = { 0, 0, 0,{ 0, 0, 0, 0, 0, 0, 0, 0 } };

	inline bool operator < (const Guid & left, const Guid & right)
	{
		return (memcmp(&left, &right, sizeof(Guid)) < 0) ? true : false;
	}

	inline bool operator == (const Guid & left, const Guid & right)
	{
		return (memcmp(&left, &right, sizeof(Guid)) == 0) ? true : false;
	}

	inline bool operator != (const Guid & left, const Guid & right)
	{
		return !(left == right);
	}

	// Define a custom UTF-8 string using internal allocator
	using String = std::basic_string <char, std::char_traits<char>, Allocator<char>>;

	// Define a custom UTF-16 string using internal allocator
	using StringU16 = std::basic_string <char16_t, std::char_traits<char16_t>, Allocator<char16_t>>;

	// Define a custom wide character string using internal allocator
	using WString = std::basic_string <wchar_t, std::char_traits<wchar_t>, Allocator<wchar_t>>;

#ifdef JINX_USE_PMR
	template <typename T>
	using Vector = std::pmr::vector<T>;
#else
	template <typename T>
	using Vector = std::vector<T, Allocator<T>>;
#endif

	// Runtime ID used for unique identifiers
	using RuntimeID = uint64_t;
	const RuntimeID InvalidID = 0;

}

#endif // JX_MEMORY_H__
