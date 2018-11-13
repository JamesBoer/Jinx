/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


TEST_CASE("Test Loops", "[Loops]")
{
	SECTION("Test loop while")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 1
			loop while a < 10
				increment a
			end	
		
			set b to 1
			loop while true
				increment b
				if b = 10
					break
				end
			end	
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 10);
		REQUIRE(script->GetVariable("b") == 10);
	}

	SECTION("Test loop until")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 1
			loop until a >= 10
				increment a
			end	
		
			set b to 1
			loop until false
				increment b
				if b = 10
					break
				end
			end	
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 10);
		REQUIRE(script->GetVariable("b") == 10);
	}

	SECTION("Test loop do while")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 1
			loop
				increment a
			while a < 10
		
			set b to 1
			loop
				increment b
				if b = 10
					break
				end
			while true
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 10);
		REQUIRE(script->GetVariable("b") == 10);
	}
	
	SECTION("Test loop do until")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 1
			loop
				increment a
			until a >= 10
		
			set b to 1
			loop
				increment b
				if b = 10
					break
				end
			until false
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 10);
		REQUIRE(script->GetVariable("b") == 10);
	}
	
	SECTION("Test loop count")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 0
			loop from 1 to 10
				increment a
			end	

			set b to 0
			loop from 1 to 10 by 2
				increment b
			end	

			set c to 0
			loop from 10 to 1
				increment c
			end	

			set d to 0
			loop i from 1 to 3
				increment d by i
			end

			set e to 0
			loop from 1 to 10
				increment e
				break
			end
		
			set f to 0
			loop from 1 to 10
				increment f
				if f = 5
					break
				end
			end
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 10);
		REQUIRE(script->GetVariable("b") == 5);
		REQUIRE(script->GetVariable("c") == 10);
		REQUIRE(script->GetVariable("d") == 6);
		REQUIRE(script->GetVariable("e") == 1);
		REQUIRE(script->GetVariable("f") == 5);
	}

	SECTION("Test loop over collection")
	{
		static const char * scriptText =
			u8R"(
			import core

			set a to 0
			loop over 1, 2, 3
				increment a
			end

			set b to 0
			set x to 1, 2, 3
			loop over x
				increment b
			end

			set c to 0
			loop i over 1, 2, 3
				increment c by i value
			end

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 3);
		REQUIRE(script->GetVariable("b") == 3);
		REQUIRE(script->GetVariable("c") == 6);
	}
}