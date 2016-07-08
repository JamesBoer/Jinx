/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_UNICODE_H__
#define JX_UNICODE_H__


namespace Jinx
{

	size_t GetUtf8CharSize(const char * utf8Str);
	void ConvertUtf8ToUtf32(const char * utf8In, uint32_t inBufferCount, char32_t * utf32CodePoint, uint32_t * numCharsOut);
	char32_t GetUTF32CodePoint(const char * ptr);

};

#endif // JX_UNICODE_H__
