/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "HeaderTest.h"

using namespace Jinx;

bool TestExecuteScript(Jinx::RuntimePtr runtime)
{
	const char * scriptText =
		u8R"(
	
			set a to 123
			set b to a

			)";
	auto script = runtime->ExecuteScript(scriptText);
	return script->GetVariable("b") == 123;
}
