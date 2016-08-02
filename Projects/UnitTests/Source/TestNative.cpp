/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


static bool s_functionCalled;


static Variant ThisFunction(ScriptPtr script, Parameters params)
{
	s_functionCalled = true;
	return Variant();
}

static Variant ThatFunction(ScriptPtr script, Parameters params)
{
	return 42;
}

static Variant AnotherFunction(ScriptPtr script, Parameters params)
{
	return "forty two";
}

static Variant YetAnotherFunction(ScriptPtr script, Parameters params)
{
	if (params.size() != 3)
		return Variant("");
	return params[0] + Variant(" ") + params[1] + Variant(" ") + params[2];
}


TEST_CASE("Test Native", "[Native]")
{
	SECTION("Test native callback functions")
	{
		static const char * scriptText =
			u8R"(

			import test			

			this function
			a is that function
			b is another function
			c is yet "one" another "two" function "three"

			)";

		auto runtime = TestCreateRuntime();
		auto library = runtime->GetLibrary("test");
		library->RegisterFunction(true, false, {"this", "function"}, ThisFunction);
		library->RegisterFunction(true, true, { "that", "function" }, ThatFunction);
		library->RegisterFunction(true, true, { "another", "function" }, AnotherFunction);
		library->RegisterFunction(true, true, { "yet", "{}", "another", "{}", "function", "{}"}, YetAnotherFunction);
		auto script = TestExecuteScript(scriptText, runtime);
		REQUIRE(script);
		REQUIRE(s_functionCalled == true);
		REQUIRE(script->GetVariable("a").GetInteger() == 42);
		REQUIRE(script->GetVariable("b").GetString() == "forty two");
		REQUIRE(script->GetVariable("c").GetString() == "one two three");
	}

	SECTION("Test native function execution")
	{
		static const char * scriptText =
			u8R"(
    
			public function return getvalue
				return 123
			end			
			
			)";

		auto runtime = TestCreateRuntime();
		auto script = TestExecuteScript(scriptText, runtime);
		REQUIRE(script);
	}

	SECTION("Test native properties")
	{
		static const char * scriptText =
			u8R"(
			import test
					
			a is someprop
			
			)";

		auto runtime = TestCreateRuntime();
		auto library = runtime->GetLibrary("test");
		library->RegisterProperty(false, true, "someprop", 42);
		auto script = TestExecuteScript(scriptText, runtime);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetInteger() == 42);
	}
}