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
        static const char * tableText =
            "Name Field\tInteger Field\tFloat Field\tText Field\n"
            "Test Name A\t1\t4.5\tThis is a simple test.\r\n"
            "Test Name B\t2\t123.456\tMore to test...\r"
            "Test Name C\t3\t22.3345\tEven more tests of text\n"
            "Still Another Test Name\t4\t1.5\tStill more text\n"
            "Yet Another Test Name\t5\t99.99\tYet more text to test\r"
            ;
        
        static const char * scriptText =
        u8R"(
        
        external table
        
        set a to table["Test Name A"]["Name Field"]
        set b to table["Test Name B"]["Integer Field"]
        set c to table["Test Name C"]["Float Field"]
        set d to table["Still Another Test Name"]["Text Field"]
        set e to table["Yet Another Test Name"]["Text Field"]
        )";
        
        auto script = TestCreateScript(scriptText);
        Variant table = tableText;
        REQUIRE(table.ConvertTo(ValueType::Collection));
        script->SetVariable("table", table);
        REQUIRE(script->Execute());
        REQUIRE(script->GetVariable("table").IsCollection());
        REQUIRE(script->GetVariable("a") == "Test Name A");
        REQUIRE(script->GetVariable("b") == 2);
        REQUIRE(script->GetVariable("c").GetNumber() == 22.3345);
        REQUIRE(script->GetVariable("d") == "Still more text");
        REQUIRE(script->GetVariable("e") == "Yet more text to test");
	}
	ShutDown();
    return 0;
}
