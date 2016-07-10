/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

using namespace Jinx;

static size_t s_sizeUTF8[256] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, 4,4,4,4,4,4,4,4,5,5,5,5,6,6,6,6,
};

size_t Jinx::GetUtf8CharSize(const char * utf8Str)
{
	// Validate parameter
	if (!(utf8Str && utf8Str[0] != 0))
	{
		LogWriteLine("Invalid character value passed to GetUtf8CharSize()");
		return 1;
	}

	// Retrieve UTF-8 character size from lookup table
	unsigned char c = utf8Str[0];
	size_t s = s_sizeUTF8[c];

	// While values of 5 or 6 bytes are technically possible, it is not a valid UTF-8 sequence
	if (s > 4)
	{
		LogWriteLine("Invalid character size calculated in GetUtf8CharSize()");
		return 4;
	}

	return s;
}

// Workaround for VS 2015 bug
#if _MSC_VER == 1900
StringU16 Jinx::ConvertUtf8ToUtf16(const String & utf8Str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t, Allocator<int16_t>, Allocator<char>> convert;
	auto str = convert.from_bytes(utf8Str);
	return StringU16(reinterpret_cast<const char16_t *>(str.data()));
}
String Jinx::ConvertUtf16ToUtf8(const StringU16 & utf16Str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t, Allocator<int16_t>, Allocator<char>> convert;
	auto p = reinterpret_cast<const int16_t *>(utf16Str.data());
	return convert.to_bytes(p, p + utf16Str.size());
}
#else

StringU16 Jinx::ConvertUtf8toUtf16(String utf8Str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t, Allocator<char16_t>, Allocator<char>> convert;
	return convert.from_bytes(utf8Str);
}

String Jinx::ConvertUtf16toUtf8(StringU16 utf16_string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t, Allocator<char16_t>, Allocator<char>> convert;
	return convert.to_bytes(utf16_string);
}

#endif




