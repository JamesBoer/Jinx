/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_UNICODE_H__
#define JX_UNICODE_H__


namespace Jinx::Impl
{

	size_t GetUtf8CharSize(const char * utf8Str);

	StringU16 ConvertUtf8ToUtf16(const String & utf8Str);
	String ConvertUtf16ToUtf8(const StringU16 & utf16Str);

	WString ConvertUtf8ToWString(const String & utf8Str);
	String ConvertWStringToUtf8(const WString & wStr);

	bool IsCaseFolded(const String & source);
	String FoldCase(const String & source);

} // namespace Jinx::Impl

#endif // JX_UNICODE_H__
