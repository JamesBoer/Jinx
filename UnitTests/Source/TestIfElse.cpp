/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
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

			a is false
			if true
				a is true
			end

			b is false
			if true
				b is true
			else
				b is false
			end

			c is false
			if false
				c is false
			else
				c is true
			end

			d is false
			if false
				d is false
			else if true
				d is true
			else
				d is false
			end

			e is false
			if false
				e is false
			else if false
				e is false
			else if true
				e is true
			else
				e is false
			end

			f is false
			if false
				f is false
			else if false
				f is false
			else if false
				f is false
			else
				f is true
			end

			g is false
			if true
				if true
					g is true
				end
			end

			h is false
			if false
			else
				if true
					h is true
				end
			end

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetBoolean() == true);
		REQUIRE(script->GetVariable("b").GetBoolean() == true);
		REQUIRE(script->GetVariable("c").GetBoolean() == true);
		REQUIRE(script->GetVariable("d").GetBoolean() == true);
		REQUIRE(script->GetVariable("e").GetBoolean() == true);
		REQUIRE(script->GetVariable("f").GetBoolean() == true);
		REQUIRE(script->GetVariable("g").GetBoolean() == true);
		REQUIRE(script->GetVariable("h").GetBoolean() == true);
	}

}