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

#include "../../Source/Jinx.h"

using namespace Jinx;

#define REQUIRE assert

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
	Initialize(params);
	// Scope block to ensure all objects are destroyed for shutdown test
	{
		static const char * scriptText =
			u8R"(

				set private a to []
				set a["one"] to 2

				set private b to []
				set b["one"]["two"] to 3

				set private c to ["one", []]
				set c["one"]["two"]["three"] to 4

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetLibrary()->GetProperty("a").IsCollection());
        REQUIRE(script->GetLibrary()->GetProperty("a").GetCollection()->at("one") == 2);
        REQUIRE(script->GetLibrary()->GetProperty("b").IsCollection());
        REQUIRE(script->GetLibrary()->GetProperty("b").GetCollection()->at("one").GetCollection()->at("two") == 3);
        REQUIRE(script->GetLibrary()->GetProperty("c").IsCollection());
        REQUIRE(script->GetLibrary()->GetProperty("c").GetCollection()->at("one").GetCollection()->at("two").GetCollection()->at("three") == 4);
    }
	ShutDown();
    return 0;
}