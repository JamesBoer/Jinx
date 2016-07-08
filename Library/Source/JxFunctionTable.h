/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once
#ifndef JX_FUNCTION_TABLE_H__
#define JX_FUNCTION_TABLE_H__


namespace Jinx
{

	class FunctionTable
	{
	public:
		// Find signature based on parts - returns nullptr if not found
		const FunctionSignature * Find(const FunctionSignatureParts & parts) const;

		// Store a new function signature
		bool Register(const FunctionSignature & signature, bool checkForDuplicates);

	private:

		mutable Mutex m_mutex;
		std::vector<FunctionSignature, Allocator<FunctionSignature>> m_signatures;
	};

};

#endif // JX_FUNCTION_TABLE_H__
