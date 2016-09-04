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
#ifdef _WINDOWS
#include <conio.h>
#endif

#include "../../../Source/Jinx.h"

using namespace Jinx;

#define REQUIRE assert

Jinx::ScriptPtr TestExecuteScript(const char * scriptText, Jinx::RuntimePtr runtime = nullptr)
{
	if (!runtime)
		runtime = CreateRuntime();

	// Compile the text to bytecode
	auto bytecode = runtime->Compile(scriptText, "Test Script");
	if (!bytecode)
		return nullptr;

	// Create a runtime script with the given bytecode
	auto script = runtime->CreateScript(bytecode);

	// Execute script and update runtime until script is finished
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
	printf("Jinx version: %s\n", Jinx::VersionString);

	// Add scope block to ensure all objects are destroyed for shutdown test
	{
		GlobalParams globalParams;
		globalParams.logSymbols = true;
		globalParams.logBytecode = true;
		globalParams.allocBlockSize = 1024 * 256;
		globalParams.allocFn = [](size_t size) { return malloc(size); };
		globalParams.reallocFn = [](void * p, size_t size) { return realloc(p, size); };
		globalParams.freeFn = [](void * p) { free(p); };
		Jinx::Initialize(globalParams);
	
		auto runtime = Jinx::CreateRuntime();

		static const char * scriptText =
		u8R"(
			import core

			public a is []
			public b is []

			a ["coll"] is b
			b ["coll"] is a

		)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		//auto a = script->GetVariable("a");
		//REQUIRE(a.IsCollection());
		//auto c = a.GetCollection();
	}

	Jinx::ShutDown();

	auto stats = GetMemoryStats();

#ifdef _WINDOWS
	printf("Press any key to continue...");
	_getch();
#endif
    
    return 0;
}