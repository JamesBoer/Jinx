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
	params.errorOnMaxInstrunctions = false;
	params.maxInstructions = std::numeric_limits<uint32_t>::max();
	Initialize(params);
	// Scope block to ensure all objects are destroyed for shutdown test
	{
		static const char * scriptText =
			u8R"(
			import core

			function fibR {integer n}
				if n < 2
					return n
				end
				return (fibR(n - 2) + fibR(n - 1))
			end

			set N to 34
			write line "fib: ", fibR(N)

			)";
		auto runtime = TestCreateRuntime();
		auto script = TestExecuteScript(scriptText, runtime);
		REQUIRE(script);
		REQUIRE(script->GetVariable("N") == 433494437);
		auto perfStats = runtime->GetScriptPerformanceStats();
		printf("Execution time: %f\n", static_cast<float>(perfStats.executionTimeNs) / 1000000000.0f);
		auto memStats = Jinx::GetMemoryStats();
		memStats.externalAllocCount;
		printf("External alloc count: %i\n", static_cast<int>(memStats.externalAllocCount));
	}
	ShutDown();
	return 0;
}
