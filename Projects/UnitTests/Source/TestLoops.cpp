/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


TEST_CASE("Test Loops", "[Loops]")
{
	SECTION("Test loop while")
	{
		static const char * scriptText =
			u8R"(
    
			a is 1
			loop while a < 10
				increment a
			end	
		
			b is 1
			loop while true
				increment b
				if b = 10
					break
				end
			end	
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetInteger() == 10);
		REQUIRE(script->GetVariable("b").GetInteger() == 10);
	}

	SECTION("Test loop do while")
	{
		static const char * scriptText =
			u8R"(
    
			a is 1
			loop
				increment a
			while a < 10
		
			b is 1
			loop
				increment b
				if b = 10
					break
				end
			while true
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetInteger() == 10);
		REQUIRE(script->GetVariable("b").GetInteger() == 10);
	}

	SECTION("Test loop count")
	{
		static const char * scriptText =
			u8R"(
    
			a is 0
			loop from 1 to 10
				increment a
			end	

			b is 0
			loop from 1 to 10 by 2
				increment b
			end	

			c is 0
			loop from 10 to 1
				increment c
			end	

			d is 0
			loop i from 1 to 3
				increment d by i
			end

			e is 0
			loop from 1 to 10
				increment e
				break
			end
		
			f is 0
			loop from 1 to 10
				increment f
				if f = 5
					break
				end
			end
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetInteger() == 10);
		REQUIRE(script->GetVariable("b").GetInteger() == 5);
		REQUIRE(script->GetVariable("c").GetInteger() == 10);
		REQUIRE(script->GetVariable("d").GetInteger() == 6);
		REQUIRE(script->GetVariable("e").GetInteger() == 1);
		REQUIRE(script->GetVariable("f").GetInteger() == 5);
	}

	SECTION("Test loop over collection")
	{
		static const char * scriptText =
			u8R"(
    
			a is 0
			loop over 1, 2, 3
				increment a
			end

			b is 0
			x is 1, 2, 3
			loop over x
				increment b
			end

			c is 0
			loop i over 1, 2, 3
				increment c by i
			end

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetInteger() == 3);
		REQUIRE(script->GetVariable("b").GetInteger() == 3);
		REQUIRE(script->GetVariable("c").GetInteger() == 6);
	}
}