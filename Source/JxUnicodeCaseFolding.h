/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_UNICODE_CASE_FOLDING_H__
#define JX_UNICODE_CASE_FOLDING_H__


namespace Jinx::Impl
{

	struct CaseFoldingData
	{
		char32_t sourceCodePoint;
		char32_t destCodePoint1;
		char32_t destCodePoint2;
	};

	bool FindCaseFoldingData(char32_t sourceCodePoint, char32_t * destCodePoint1, char32_t * destCodePoint2);

} // namespace Jinx::Impl

#endif // JX_UNICODE_CASE_FOLDING_H__
