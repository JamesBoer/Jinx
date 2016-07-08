/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;

static bool s_initializedGlobals = false;

Jinx::RuntimePtr TestCreateRuntime()
{
	if (!s_initializedGlobals)
	{
		GlobalParams globalParams;
		globalParams.allocBlockSize = 1024 * 256;
		globalParams.logFn = [](const char *) {};
		globalParams.allocFn = [](size_t size) { return malloc(size); };
		globalParams.reallocFn = [](void * p, size_t size) { return realloc(p, size); };
		globalParams.freeFn = [](void * p) { free(p); };
		Jinx::Initialize(globalParams);

		s_initializedGlobals = true;
	}

	return Jinx::CreateRuntime();
}

bool TestCompileScript(const char * scriptText, Jinx::RuntimePtr runtime)
{
	if (!runtime)
		runtime = TestCreateRuntime();

	// Compile the text to bytecode
	auto bytecode = runtime->Compile(scriptText);
	return bytecode ? true : false;
}

Jinx::ScriptPtr TestExecuteScript(const char * scriptText, Jinx::RuntimePtr runtime)
{
	if (!runtime)
		runtime = TestCreateRuntime();

	// Compile the text to bytecode
	auto bytecode = runtime->Compile(scriptText);
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