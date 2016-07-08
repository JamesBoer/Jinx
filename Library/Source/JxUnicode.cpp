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
	assert(utf8Str && utf8Str[0] != 0);

	// Retrieve UTF-8 character size from lookup table
	unsigned char c = utf8Str[0];
	size_t s = s_sizeUTF8[c];

	// While values of 5 or 6 bytes are technically possible, it is not a valid UTF-8 sequence
	assert(s <= 4);

	return s;
}

void Jinx::ConvertUtf8ToUtf32(const char * utf8In, uint32_t inBufferCount, char32_t * utf32CodePoint, uint32_t * numCharsOut)
{
	// Validate parameters
	if (!utf8In || inBufferCount == 0 || !utf32CodePoint || !numCharsOut)
		throw std::invalid_argument("Invalid arguments passed to ConvertUtf8ToUtf32()");

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
			throw std::invalid_argument("Invalid character data passed to function ConvertUtf8ToUtf32()");
		*utf32CodePoint = char32_t(utf8[0] & 0x1F);
		if (*utf32CodePoint < 2)
			throw std::invalid_argument("Invalid character data passed to function ConvertUtf8ToUtf32()");
		*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[1] & 0x3F);
		*numCharsOut = 2;
	}
	else if ((utf8In[0] & 0xF0) == 0xE0)
	{
		if (inBufferCount < 3 || (utf8[1] & 0xC0) != 0x80 || (utf8[2] & 0xC0) != 0x80)
			throw std::invalid_argument("Invalid character data passed to function ConvertUtf8ToUtf32()");
		*utf32CodePoint = char32_t(utf8[0] & 0x0F);
		*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[1] & 0x3F);
		*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[2] & 0x3F);
		*numCharsOut = 3;
	}
	else if ((utf8In[0] & 0xF8) == 0xF0)
	{
		if (inBufferCount < 4 || (utf8[1] & 0xC0) != 0x80 || (utf8[2] & 0xC0) != 0x80 || (utf8[3] & 0xC0) != 0x80)
			throw std::invalid_argument("Invalid character data passed to function ConvertUtf8ToUtf32()");
		*utf32CodePoint = char32_t(utf8[0] & 0x07);
		*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[1] & 0x3F);
		*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[2] & 0x3F);
		*utf32CodePoint = (*utf32CodePoint << 6) + char32_t(utf8[3] & 0x3F);
		*numCharsOut = 4;
	}
	else
	{
		// Invalid data
		throw std::invalid_argument("Invalid character data passed to function ConvertUtf32ToUtf8()");
	}
}

char32_t Jinx::GetUTF32CodePoint(const char * ptr)
{
	unsigned inCount = 0;
	for (; inCount < 4; ++inCount)
	{
		if (ptr[0] == 0)
		{
			break;
		}
	}
	unsigned outCount;
	char32_t codePoint = 0;
	ConvertUtf8ToUtf32(ptr, inCount, &codePoint, &outCount);
	return codePoint;
}