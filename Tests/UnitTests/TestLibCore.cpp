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

	SECTION("Test UTF-8 string size")
	{
		const char * scriptText =
			u8R"(

			import core

			set a to "いろは"
			set b to a size

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == u8"いろは");
		REQUIRE(script->GetVariable("b") == 3);
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

	SECTION("Test 'call' no-arg no-return function with local function variable")
	{
		static const char * scriptText =
			u8R"(

			import core

			set public a to 0
			function test
				set a to 123
			end

			set f to function test
			call f

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("f").IsFunction());
		auto library = script->GetLibrary();
		REQUIRE(library->GetProperty("a") == 123);
	}

	SECTION("Test 'call' no-arg function")
	{
		static const char * scriptText =
			u8R"(

			import core

			function test
				return 123
			end

			set f to function test
			set a to call f

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("f").IsFunction());
		REQUIRE(script->GetVariable("a") == 123);
	}

	SECTION("Test 'call with' one-arg function")
	{
		static const char * scriptText =
			u8R"(

			import core

			function test {x}
				return x
			end

			set f to function test {x}
			set a to call f with 123

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("f").IsFunction());
		REQUIRE(script->GetVariable("a") == 123);
	}

	SECTION("Test 'call with' two-arg function")
	{
		static const char * scriptText =
			u8R"(

			import core

			function test {x} and {y}
				return x, y
			end

			set f to function test {x} and {y}
			set a to call f with 12, 34

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("f").IsFunction());
		REQUIRE(script->GetVariable("a").IsCollection());
		REQUIRE(script->GetVariable("a").GetCollection()->at(1) == 12);
		REQUIRE(script->GetVariable("a").GetCollection()->at(2) == 34);
	}

}