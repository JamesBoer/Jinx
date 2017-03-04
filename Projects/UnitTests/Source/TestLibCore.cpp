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
			d is b get size
			
            
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

			c is a empty
			d is b is empty
			
            )";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("c").GetBoolean() == true);
		REQUIRE(script->GetVariable("d").GetBoolean() == true);
	}
	
}