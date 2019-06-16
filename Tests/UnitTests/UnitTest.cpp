/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;

static bool s_initializedGlobals = false;

Jinx::RuntimePtr TestCreateRuntime()
{
	if (!s_initializedGlobals)
	{
		GlobalParams globalParams;
		globalParams.enableLogging = true;
		globalParams.logBytecode = true;
		globalParams.logFn = [] (LogLevel, const char *) { };
		globalParams.allocBlockSize = 1024 * 256;
		globalParams.allocFn = [](size_t size) { return malloc(size); };
		globalParams.reallocFn = [](void * p, size_t size) { return realloc(p, size); };
		globalParams.freeFn = [](void * p) { free(p); };
		Jinx::Initialize(globalParams);

		s_initializedGlobals = true;
	}

	return Jinx::CreateRuntime();
}

ScriptPtr TestCreateScript(const char * scriptText, Jinx::RuntimePtr runtime, JinxAny userContext)
{
	if (!runtime)
		runtime = TestCreateRuntime();

	// Compile the script text to bytecode
	auto bytecode = runtime->Compile(scriptText);
	if (!bytecode)
		return nullptr;

	// Create a script with the compiled bytecode
	return runtime->CreateScript(bytecode, userContext);
}

Jinx::ScriptPtr TestExecuteScript(const char * scriptText, Jinx::RuntimePtr runtime, JinxAny userContext)
{
	if (!runtime)
		runtime = TestCreateRuntime();

	// Compile the script text to bytecode
	auto bytecode = runtime->Compile(scriptText);
	if (!bytecode)
		return nullptr;

	// Create a script with the compiled bytecode
	auto script = runtime->CreateScript(bytecode, userContext);

	// Execute script until finished
	do
	{
		if (!script->Execute())
			return nullptr;
	} 
	while (!script->IsFinished());

	return script;
}