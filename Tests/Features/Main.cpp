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
		const char * tableText =
u8R"(
Name Field,Integer Field,Float Field,Test Field
Test Name A,1,4.5,This is a simple test.
Test Name B,2,123.456,More to test…
Test Name C,3,22.3345,Even more tests of text
Still Another Test Name,4,1.5,Still more text
Yet Another Test Name,5,99.99,Yet more text to test
)";


		const char * scriptText =
			u8R"(

				external table text
				
				set table to table text as collection
				set row to table["Test Name C"]
				set test1 to row["Integer Field"]
			)";

		auto runtime = TestCreateRuntime();
		auto script = TestCreateScript(scriptText, runtime);
		script->SetVariable("table text", tableText);
		REQUIRE(script->Execute());
		REQUIRE(script->GetVariable("table").IsCollection());
		auto rowColl = script->GetVariable("table").GetCollection();
		auto columnColl = rowColl->at("Test Name C");
		auto val = columnColl.GetCollection()->at("Integer Field");
		REQUIRE(val == 3);
		//REQUIRE(script->GetVariable("test1") == 3);
	}
	ShutDown();
    return 0;
}