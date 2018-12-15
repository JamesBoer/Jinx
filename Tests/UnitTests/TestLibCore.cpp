/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


TEST_CASE("Test Core Library", "[Core]")
{

	SECTION("Test write functions")
	{
		const char * scriptText =
			u8R"(
			
			import core

			write "test 1, ", "test 2, ", "test 3", newline
			write line "test 1, ", "test 2 ", "test 3"
			
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);

	}

	SECTION("Test size functions")
	{
		const char * scriptText =
			u8R"(
			
			import core

			set a to 1, 2, 3
			set b to "test"

			set c to a size
			set d to b get size
			
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("c") == 3);
		REQUIRE(script->GetVariable("d") == 4);
	}

	SECTION("Test to empty functions")
	{
		const char * scriptText =
			u8R"(
			
			import core

			set a to []
			set b to ""

			set c to a empty
			set d to b is empty
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("c") == true);
		REQUIRE(script->GetVariable("d") == true);
	}

	SECTION("Test get call stack function with local functions")
	{
		const char * scriptText =
			u8R"(
			
			import core

			function func4 {integer a}
				return call stack
			end

			function func3 {a}
				return func4 a
			end

			function func2/func22 (opt1/opt2)
				return func3 123
			end

			function func1/func11 (optional)
				return func2
			end

			set a to func1
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		REQUIRE(script->GetVariable("a").GetCollection()->at(1) == "root");
		REQUIRE(script->GetVariable("a").GetCollection()->at(2) == "func1/func11 (optional)");
		REQUIRE(script->GetVariable("a").GetCollection()->at(3) == "func2/func22 (opt1/opt2)");
		REQUIRE(script->GetVariable("a").GetCollection()->at(4) == "func3 {}");
		REQUIRE(script->GetVariable("a").GetCollection()->at(5) == "func4 {integer}");
	}

	SECTION("Test get call stack function with local functions")
	{
		const char * scriptText =
			u8R"(
			
			import core

			library test

			function func one
				return call stack
			end

			set a to func one
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		REQUIRE(script->GetVariable("a").GetCollection()->at(1) == "root");
		REQUIRE(script->GetVariable("a").GetCollection()->at(2) == "test func one");
	}

}