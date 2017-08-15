/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


TEST_CASE("Test If/Else Branching", "[IfElse]")
{
	SECTION("Test variables and basic statements")
	{
		static const char * scriptText =
			u8R"(
	
			-- Simple if/else tests

			set a to false
			if true
				set a to true
			end

			set b to false
			if true
				set b to true
			else
				set b to false
			end

			set c to false
			if false
				set c to false
			else
				set c to true
			end

			set d to false
			if false
				set d to false
			else if true
				set d to true
			else
				set d to false
			end

			set e to false
			if false
				set e to false
			else if false
				set e to false
			else if true
				set e to true
			else
				set e to false
			end

			set f to false
			if false
				set f to false
			else if false
				set f to false
			else if false
				set f to false
			else
				set f to true
			end

			set g to false
			if true
				if true
					set g to true
				end
			end

			set h to false
			if false
			else
				if true
					set h to true
				end
			end

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == true);
		REQUIRE(script->GetVariable("b") == true);
		REQUIRE(script->GetVariable("c") == true);
		REQUIRE(script->GetVariable("d") == true);
		REQUIRE(script->GetVariable("e") == true);
		REQUIRE(script->GetVariable("f") == true);
		REQUIRE(script->GetVariable("g") == true);
		REQUIRE(script->GetVariable("h") == true);
	}

}