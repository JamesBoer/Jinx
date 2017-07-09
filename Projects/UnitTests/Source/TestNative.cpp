/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
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
			set a to that function
			set b to another function
			set c to yet "one" another "two" function "three"

			)";

		auto runtime = TestCreateRuntime();
		auto library = runtime->GetLibrary("test");
		library->RegisterFunction(Visibility::Public, {"this", "function"}, ThisFunction);
		library->RegisterFunction(Visibility::Public, { "that", "function" }, ThatFunction);
		library->RegisterFunction(Visibility::Public, { "another", "function" }, AnotherFunction);
		library->RegisterFunction(Visibility::Public, { "yet", "{}", "another", "{}", "function", "{}"}, YetAnotherFunction);
		auto script = TestExecuteScript(scriptText, runtime);
		REQUIRE(script);
		REQUIRE(s_functionCalled == true);
		REQUIRE(script->GetVariable("a") == 42);
		REQUIRE(script->GetVariable("b") == "forty two");
		REQUIRE(script->GetVariable("c") == "one two three");
	}

	SECTION("Test native function execution")
	{
		static const char * scriptText =
			u8R"(
	
			public function getvalue
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
					
			set a to someprop
			
			)";

		auto runtime = TestCreateRuntime();
		auto library = runtime->GetLibrary("test");
		library->RegisterProperty(Visibility::Public, Access::ReadWrite, "someprop", 42);
		auto script = TestExecuteScript(scriptText, runtime);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 42);
	}

	SECTION("Test native variant Guid-String conversions")
	{
		static const char * guidStr = "06DF8818-07DB-4AAB-9BF6-3365D0F2D4C9";
		Variant gs = guidStr;	
		Variant gv = gs.GetGuid();
		Variant gs2 = gv.GetString();
		REQUIRE(gs == gs2);
	}
}