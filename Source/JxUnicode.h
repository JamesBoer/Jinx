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

	bool IsCaseFolded(std::string_view source);
	bool IsCaseFolded(const String & source);
	String FoldCase(std::string_view source);
	String FoldCase(const String & source);

	size_t GetStringCount(const String & source);
	std::optional<String> GetUtf8CharByIndex(const String & source, int64_t index);
	std::optional<String> GetUtf8CharsByRange(const String & source, const std::pair<int64_t, int64_t> & range);
	std::optional<String> ReplaceUtf8CharAtIndex(const String & dest, const String & source, int64_t index);
	std::optional<String> ReplaceUtf8CharsAtRange(const String & dest, const String & source, const std::pair<int64_t, int64_t> & range);

} // namespace Jinx::Impl

#endif // JX_UNICODE_H__
