/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "JxInternal.h"

using namespace Jinx;


const FunctionSignature * FunctionTable::Find(const FunctionSignatureParts & parts) const
{
	std::lock_guard<Mutex> lock(m_mutex);

	// Store the longest match
	const FunctionSignature * longestMatch = nullptr;

	for (const auto & s : m_signatures)
	{
		// Check to see if we have at least a partial match
		if (s.IsMatch(parts))
		{
			// Determine if this is the new longest match
			if (!longestMatch)
				longestMatch = &s;
			else
			{
				if (s.GetLength() > longestMatch->GetLength())
					longestMatch = &s;
			}
		}
	}

	return longestMatch;
}

bool FunctionTable::Register(const FunctionSignature & signature, bool checkForDuplicates)
{
	std::lock_guard<Mutex> lock(m_mutex);

	// Check for duplicates
	if (checkForDuplicates)
	{
		for (auto & s : m_signatures)
		{
			if (s == signature)
				return false;
		}
	}
	m_signatures.push_back(signature);
	return true;
}
