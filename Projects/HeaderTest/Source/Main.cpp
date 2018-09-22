/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include <iostream>
#include <cassert>

#include "HeaderTest.h"

using namespace Jinx;

int main(int argc, char ** argv)
{
	std::cout << "Jinx version: " << Jinx::GetVersionString().c_str() << std::endl;  
	const char * scriptText =
		u8R"(
	
			set private counter to 0
			
			function counter is finished
				increment counter
				return counter >= 10
			end

			loop until counter is finished
			end

			)";
	auto runtime = CreateRuntime();
	auto script = runtime->ExecuteScript(scriptText);
	auto library = script->GetLibrary();
	if (library->GetProperty("counter") != 10 || !TestExecuteScript(runtime))
	{
		std::cerr << "Execution failure!" << std::endl;
		return 1;
	}
    return 0;
}
