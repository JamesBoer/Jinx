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

	static const char * libText =
		u8R"(

			library lib

			public function some {x} test {y}
				return x + y
			end 

			)";

	static const char * scriptText =
		u8R"(

			import core
			import lib

			set f to function some {} test {}
			set a to call f with "Hello ", "world!"

			)";

	auto runtime = TestCreateRuntime();
	auto libScript = TestExecuteScript(libText, runtime);
	auto script = TestExecuteScript(scriptText, runtime);
	REQUIRE(libScript);
	REQUIRE(script);
	REQUIRE(script->GetVariable("f").IsFunction());
	REQUIRE(script->GetVariable("a") == "Hello world!");

	return 0;
}
