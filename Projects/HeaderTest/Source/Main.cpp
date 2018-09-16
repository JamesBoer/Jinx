/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include <stdio.h>


#include "HeaderTest.h"

using namespace Jinx;



int main(int argc, char ** argv)
{
	printf("Jinx version: %s\n", Jinx::GetVersionString().c_str());
    
	Test::test = 999;
	CheckTestVal();

    return 0;
}