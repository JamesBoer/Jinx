/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
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

			a is 1, 2, 3
			b is "test"

			c is a size
			d is b size
			
            
            )";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("c").GetInteger() == 3);
		REQUIRE(script->GetVariable("d").GetInteger() == 4);
	}

	SECTION("Test is empty functions")
	{
		const char * scriptText =
			u8R"(
			
			import core

			a is []
			b is ""

			c is a is empty
			d is b is empty
			
            )";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("c").GetBoolean() == true);
		REQUIRE(script->GetVariable("d").GetBoolean() == true);
	}

	SECTION("Test add to functions")
	{
		const char * scriptText =
			u8R"(
			
			import core

			a is [1, "one"]
			add ("two", "three") to a
            add "four" to a

            )";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection() == true);
		REQUIRE(script->GetVariable("a").GetCollection()->size() == 4);
		REQUIRE(script->GetVariable("a").GetCollection()->at(1).GetString() == "one");
		REQUIRE(script->GetVariable("a").GetCollection()->at(2).GetString() == "two");
		REQUIRE(script->GetVariable("a").GetCollection()->at(3).GetString() == "three");
		REQUIRE(script->GetVariable("a").GetCollection()->at(4).GetString() == "four");
	}

	SECTION("Test remove from functions")
	{
		const char * scriptText =
			u8R"(
			
			import core

			a is "one", "two", "three", "four"
			remove (1, 2) from a
			remove 4 from a
            
            )";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection() == true);
		REQUIRE(script->GetVariable("a").GetCollection()->size() == 1);
		REQUIRE(script->GetVariable("a").GetCollection()->at(3).GetString() == "three");
	}

	SECTION("Test remove values from functions")
	{
		const char * scriptText =
			u8R"(
			
			import core

			a is "one", "two", "three", "four"
			remove values ("one", "two") from a
			remove value "four" from a
            
            )";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection() == true);
		REQUIRE(script->GetVariable("a").GetCollection()->size() == 1);
		REQUIRE(script->GetVariable("a").GetCollection()->at(3).GetString() == "three");
	}
	
}