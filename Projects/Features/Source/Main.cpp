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

#include "../../../Source/Jinx.h"

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

	// Add scope block to ensure all objects are destroyed for shutdown test
	{
		GlobalParams globalParams;
		globalParams.logSymbols = true;
		globalParams.logBytecode = true;
		globalParams.enableDebugInfo = true;
        globalParams.maxInstructions = 5000;
		globalParams.allocBlockSize = 1024 * 256;
		globalParams.allocFn = [](size_t size) { return malloc(size); };
		globalParams.reallocFn = [](void * p, size_t size) { return realloc(p, size); };
		globalParams.freeFn = [](void * p) { free(p); };
		Jinx::Initialize(globalParams);

		const char * scriptText =
			u8R"(

				private function {a} minus {b}
					return a - b
				end

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		auto id = script->FindFunction(nullptr, Visibility::Private, {"{}", "minus", "{}"});
		auto val = script->CallFunction(id, {5, 2});
		REQUIRE(val == 3);
	}

	Jinx::ShutDown();

	auto stats = GetMemoryStats();
	REQUIRE(stats.currentAllocatedMemory == 0);
	REQUIRE(stats.currentUsedMemory == 0);
    
    return 0;
}