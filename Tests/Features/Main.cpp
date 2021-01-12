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
	auto bytecode = runtime->Compile(scriptText, "Test Script"/*, { "core" }*/);
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

	static const char * scriptText =
		u8R"(

			import core

			-- Function declaration
			function count to {integer y}
				set x to 0
				loop while x < y
					increment x
					wait
				end
				return x
			end

			-- Execute function asynchronously and store coroutine in variables
			set c1 to async call function count to {} with 15
			set c2 to async call function count to {} with 10
			set c3 to async call function count to {} with 5

			-- Wait until all of the coroutines are finished
			wait until all c1, c2, c3 are finished

			-- Retrieve return value from coroutine
			set v1 to c1's value
			set v2 to c2's value
			set v3 to c3's value

			)";

	auto script = TestExecuteScript(scriptText);
	REQUIRE(script);
	REQUIRE(script->GetVariable("c1").IsCoroutine());
	REQUIRE(script->GetVariable("c2").IsCoroutine());
	REQUIRE(script->GetVariable("c3").IsCoroutine());
	REQUIRE(script->GetVariable("v1") == 15);
	REQUIRE(script->GetVariable("v2") == 10);
	REQUIRE(script->GetVariable("v3") == 5);

	return 0;
}
