/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdexcept>

#include "../../Source/Jinx.h"

using namespace Jinx;

#ifndef NDEBUG
#define REQUIRE assert
#else
#define REQUIRE(x) { if (!(x)) throw new std::runtime_error("Failure for condition: " #x); }
#endif

Jinx::RuntimePtr TestCreateRuntime()
{
	return Jinx::CreateRuntime();
}

Jinx::ScriptPtr TestCreateScript(const char * scriptText, Jinx::RuntimePtr runtime = nullptr)
{
	if (!runtime)
		runtime = CreateRuntime();

	// Compile the text to bytecode
	auto bytecode = runtime->Compile(scriptText, "Test Script", { "core" });
	if (!bytecode)
		return nullptr;

	// Create a runtime script with the given bytecode
	return runtime->CreateScript(bytecode);
}

Jinx::ScriptPtr TestExecuteScript(const char * scriptText, Jinx::RuntimePtr runtime = nullptr)
{
	// Create a runtime script
	auto script = TestCreateScript(scriptText, runtime);
	if (!script)
		return nullptr;

	// Execute script until finished
	do
	{
		if (!script->Execute())
			return nullptr;
	}
	while (!script->IsFinished());

	return script;
}

int main(int argc, char ** argv)
{
	printf("Jinx version: %s\n", Jinx::GetVersionString().c_str());

	GlobalParams params;
	params.logBytecode = true;
	params.logSymbols = true;
	params.errorOnMaxInstrunctions = false;
	params.maxInstructions = std::numeric_limits<uint32_t>::max();
	Initialize(params);

	const char * scriptText =
		u8R"(

			import core

			function stringify {v}
				set s to ""
				if v type = collection
					loop x over v
						set s to s + (x value) as string
					end
				else
					set s to v as string
				end
				return s
			end

			set 'my list' to 3, 2, 1
			set s to ""
			loop x over 'my list'
				set s to s + stringify "key = ", x key, ", value = ", x value, " "
			end

		)";

	auto script = TestExecuteScript(scriptText);
	REQUIRE(script);
	REQUIRE(script->GetVariable("s").GetString() == "key = 1, value = 3 key = 2, value = 2 key = 3, value = 1 ");

	return 0;
}
