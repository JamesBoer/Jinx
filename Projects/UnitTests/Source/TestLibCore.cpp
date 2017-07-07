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
	
}