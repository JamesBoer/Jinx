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

static uint32_t s_fastFold[128] = {
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
	0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x5B,0x5C,0x5D,0x5E,0x5F,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
};


static void ConvertUtf8ToUtf32(const char * utf8In, size_t inBufferCount, char32_t * utf32CodePoint, size_t * numCharsOut)
{
	// Validate parameters
	if (!utf8In || inBufferCount == 0 || !utf32CodePoint || !numCharsOut)
	{
		LogWriteLine("Invalid arguments passed to ConvertUtf8ToUtf32()");
		return;
	}

	// Initialize output
	*utf32CodePoint = 0;
	*numCharsOut = 1;

	// Check for all four conversion cases
	unsigned char * utf8 = (unsigned char *)utf8In;
	if ((utf8[0] & 0x80) == 0)
	{
		*utf32CodePoint = utf8[0];
		*numCharsOut = 1;
	}
	else if ((utf8[0] & 0xE0) == 0xC0)
	{
		if (inBufferCount < 2 || (utf8[1] & 0xC0) != 0x80)
		{
			LogWriteLine("Invalid character data passed to function ConvertUtf8ToUtf32()");
			return;
		}
		*utf32CodePoint = char32_t(utf8[0] & 0x1F);
		if (*utf32CodePoint < 2)
		{
			LogWriteLine("Invalid character data passed to function ConvertUtf8ToUtf32()");
			return;
		}
		*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[1] & 0x3F);
		*numCharsOut = 2;
	}
	else if ((utf8In[0] & 0xF0) == 0xE0)
	{
		if (inBufferCount < 3 || (utf8[1] & 0xC0) != 0x80 || (utf8[2] & 0xC0) != 0x80)
		{
			LogWriteLine("Invalid character data passed to function ConvertUtf8ToUtf32()");
			return;
		}
		*utf32CodePoint = char32_t(utf8[0] & 0x0F);
		*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[1] & 0x3F);
		*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[2] & 0x3F);
		*numCharsOut = 3;
	}
	else if ((utf8In[0] & 0xF8) == 0xF0)
	{
		if (inBufferCount < 4 || (utf8[1] & 0xC0) != 0x80 || (utf8[2] & 0xC0) != 0x80 || (utf8[3] & 0xC0) != 0x80)
		{
			LogWriteLine("Invalid character data passed to function ConvertUtf8ToUtf32()");
			return;
		}
		*utf32CodePoint = char32_t(utf8[0] & 0x07);
		*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[1] & 0x3F);
		*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[2] & 0x3F);
		*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[3] & 0x3F);
		*numCharsOut = 4;
	}
	else
	{
		// Invalid data
		LogWriteLine("Invalid character data passed to function ConvertUtf32ToUtf8()");
	}
}

static void ConvertUtf32ToUtf8(char32_t utf32CodePoint, char * utf8Out, size_t outBufferCount, size_t * numCharsOut)
{
	// Validate parameters
	if (!utf8Out || outBufferCount == 0 || !numCharsOut)
	{
		LogWriteLine("Invalid arguments passed to ConvertUtf32ToUtf8()");
		return;
	}

	// Initialize output
	*numCharsOut = 1;

	// Check for all four conversion cases
	if (utf32CodePoint < 0x80)
	{
		utf8Out[0] = char(utf32CodePoint & 0x7F);
		*numCharsOut = 1;
	}
	else if (utf32CodePoint < 0x0800)
	{
		if (outBufferCount < 2)
		{
			LogWriteLine("Invalid arguments passed to ConvertUtf32ToUtf8()");
			return;
		}
		utf8Out[0] = uint8_t(utf32CodePoint >> 6) | 0xC0;
		utf8Out[1] = (uint8_t(utf32CodePoint >> 0) & 0x3F) | 0x80;
		*numCharsOut = 2;
	}
	else if (utf32CodePoint < 0x010000)
	{
		if (outBufferCount < 3)
		{
			LogWriteLine("Invalid arguments passed to ConvertUtf32ToUtf8()");
			return;
		}
		utf8Out[0] = (uint8_t(utf32CodePoint >> 12) & 0x0F) | 0xE0;
		utf8Out[1] = (uint8_t(utf32CodePoint >> 6) & 0x3F) | 0x80;
		utf8Out[2] = (uint8_t(utf32CodePoint >> 0) & 0x3F) | 0x80;
		*numCharsOut = 3;
	}
	else if (utf32CodePoint < 0x110000)
	{
		if (outBufferCount < 4)
		{
			LogWriteLine("Invalid arguments passed to ConvertUtf32ToUtf8()");
			return;
		}
		utf8Out[0] = (uint8_t(utf32CodePoint >> 18) & 0x07) | 0xF0;
		utf8Out[1] = (uint8_t(utf32CodePoint >> 12) & 0x3F) | 0x80;
		utf8Out[2] = (uint8_t(utf32CodePoint >> 6) & 0x3F) | 0x80;
		utf8Out[3] = (uint8_t(utf32CodePoint >> 0) & 0x3F) | 0x80;
		*numCharsOut = 4;
	}
	else
	{
		// Invalid data
		LogWriteLine("Invalid character data passed to function ConvertUtf32ToUtf8()");
	}
}

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

StringU16 Jinx::ConvertUtf8ToUtf16(const String & utf8Str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t, Allocator<char16_t>, Allocator<char>> convert;
	return convert.from_bytes(utf8Str);
}

String Jinx::ConvertUtf16ToUtf8(const StringU16 & utf16_string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t, Allocator<char16_t>, Allocator<char>> convert;
	return convert.to_bytes(utf16_string);
}

#endif

bool Jinx::IsCaseFolded(const String & source)
{
	const char * curr = source.c_str();
	const char * end = source.c_str() + source.size();
	while (curr < end)
	{
		// ASCII characters can do a fast table-based check
		unsigned char c = *curr;
		if (!((c & 0x80) == 0x80))
		{
			if (s_fastFold[c] != c)
				return false;
			++curr;
		}
		// Non-ASCII characters have to perform a folding map lookup
		else
		{
			char32_t codepoint;
			size_t charsOut;
			ConvertUtf8ToUtf32(curr, end - curr, &codepoint, &charsOut);
			if (FindCaseFoldingData(codepoint, nullptr, nullptr))
				return false;
			curr += charsOut;
		}
	}
	return true;
}

String Jinx::FoldCase(const String & source)
{	
	// Check to see if we can simply return the original source
	if (IsCaseFolded(source))
		return source;

	String s;
	s.reserve(source.size());
	const char * curr = source.c_str();
	const char * end = source.c_str() + source.size();
	while (curr < end)
	{
		// Attempt simple (ASCII-only) folding if possible first
		unsigned char c = *curr;
		if (!((c & 0x80) == 0x80))
		{
			s.push_back(static_cast<unsigned char>(s_fastFold[c]));
			++curr;
		}
		// Non-ASCII codepoints require lookups via the global folding map
		else
		{
			char32_t codepoint;
			size_t charsOut;
			ConvertUtf8ToUtf32(curr, end - curr, &codepoint, &charsOut);
			
			char32_t cp1;
			char32_t cp2;
			if (FindCaseFoldingData(codepoint, &cp1, &cp2))
			{
				char buffer[5] = { 0, 0, 0, 0, 0 };
				ConvertUtf32ToUtf8(cp1, buffer, countof(buffer), &charsOut);
				s.append(buffer);
				if (cp2)
				{
					size_t charsOut2;
					char buffer2[5] = { 0, 0, 0, 0, 0 };
					ConvertUtf32ToUtf8(cp2, buffer2, countof(buffer2), &charsOut2);
					s.append(buffer);
					charsOut += charsOut2;
				}
			}
			else
			{
				s.append(curr, charsOut);
			}
			curr += charsOut;
		}
	}

	return s;
}




