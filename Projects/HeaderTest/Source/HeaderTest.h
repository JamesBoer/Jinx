/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#pragma once

#include "../../../Include/Jinx.hpp"

class Test
{
public:
/*
	static int & TestVal()
	{
		static int test = 123;
		return test;
	}
*/	
	static inline int test = 123;

private:
};


void CheckTestVal();