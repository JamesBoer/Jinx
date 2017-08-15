/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


TEST_CASE("Test Expressions", "[Expressions]")
{
	SECTION("Test common math operators")
	{
		const char * scriptText =
			u8R"(
	
			-- Basic assignments 
			set a to 2
			set b to 3

			-- Basic math operations
			set c to a + b
			set d to b - a
			set e to a * b
			set f to b / 1
			set g to 10 % b

			-- Floating point assignments
			set h to 123.456
			set i to 23.45

			-- Floating point operations
			set j to h * i
			set k to h / i
			set l to h + i
			set m to h - i
			set n to h % i

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 2);
		REQUIRE(script->GetVariable("b") == 3);
		REQUIRE(script->GetVariable("c") == 5);
		REQUIRE(script->GetVariable("d") == 1);
		REQUIRE(script->GetVariable("e") == 6);
		REQUIRE(script->GetVariable("f") == 3);
		REQUIRE(script->GetVariable("g") == 1);
		REQUIRE(script->GetVariable("h").GetNumber() == Approx(123.456));
		REQUIRE(script->GetVariable("i").GetNumber() == Approx(23.45));
		REQUIRE(script->GetVariable("j").GetNumber() == Approx(2895.0432));
		REQUIRE(script->GetVariable("k").GetNumber() == Approx(5.2646));
		REQUIRE(script->GetVariable("l").GetNumber() == Approx(146.906));
		REQUIRE(script->GetVariable("m").GetNumber() == Approx(100.006));
		REQUIRE(script->GetVariable("n").GetNumber() == Approx(6.206));
	}

	SECTION("Test equality and inequality operators")
	{
		const char * scriptText =
			u8R"(
			
			set a to true = true		-- true
			set b to true != true		-- false

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == true);
		REQUIRE(script->GetVariable("b") == false);
	}

	SECTION("Test logical operators")
	{
		const char * scriptText =
			u8R"(
			
			set a to true and false			-- false	
			set b to true and true			-- true
			set c to false or false			-- false
			set d to true or false			-- true		

			set e to 1 < 2 or 4 != 5        -- true
			set f to not 1 < 2 or 4 != 5    -- false
			set g to not (1 < 2 or 4 != 5)  -- false
			set h to (not 1 < 2) or 4 != 5  -- true

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == false);
		REQUIRE(script->GetVariable("b") == true);
		REQUIRE(script->GetVariable("c") == false);
		REQUIRE(script->GetVariable("d") == true);
		REQUIRE(script->GetVariable("e") == true);
		REQUIRE(script->GetVariable("f") == false);
		REQUIRE(script->GetVariable("g") == false);
		REQUIRE(script->GetVariable("h") == true);
	}

	SECTION("Test less than or greater than operators")
	{
		const char * scriptText =
			u8R"(
			
			set a to 1 < 2				
			set b to 2 < 1	
			
			set c to 1 > 2				
			set d to 2 > 1	
			
			set e to 1 <= 2
			set f to 1 <= 1
			set g to 2 <= 1

			set h to 1 >= 2
			set i to 1 >= 1
			set j to 2 >= 1

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == true);
		REQUIRE(script->GetVariable("b") == false);
		REQUIRE(script->GetVariable("c") == false);
		REQUIRE(script->GetVariable("d") == true);
		REQUIRE(script->GetVariable("e") == true);
		REQUIRE(script->GetVariable("f") == true);
		REQUIRE(script->GetVariable("g") == false);
		REQUIRE(script->GetVariable("h") == false);
		REQUIRE(script->GetVariable("i") == true);
		REQUIRE(script->GetVariable("j") == true);
	}

	SECTION("Test automatic string conversion")
	{
		const char * scriptText =
			u8R"(
			
			set a to "number " + 9
			set b to "goodness is " + true
			set c to "evil is " + null

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == "number 9");
		REQUIRE(script->GetVariable("b") == "goodness is true");
		REQUIRE(script->GetVariable("c") == "evil is null");
	}

	SECTION("Test math operator precedence")
	{
		const char * scriptText =
			u8R"(
	
			set a to 1 + 2 * 3
			set b to 1 - 4 / 2
			set c to 2 + 9 % 2
			set d to 1 + 2 * 3 + 4 * 5
			set e to 10 - 4 / 2 + 7	

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 7);
		REQUIRE(script->GetVariable("b") == -1);
		REQUIRE(script->GetVariable("c") == 3);
		REQUIRE(script->GetVariable("d") == 27);
		REQUIRE(script->GetVariable("e") == 15);
	}

	SECTION("Test math operator precedence with parentheses")
	{
		const char * scriptText =
			u8R"(
	
			set a to (1 + 2) * 3
			set b to (8 - 4) / 2
			set c to ((1 + 2) * 3) + ((1 + 2) * 3)

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 9);
		REQUIRE(script->GetVariable("b") == 2);
		REQUIRE(script->GetVariable("c") == 18);
	}

	SECTION("Test logical and comparison operator precedence")
	{
		const char * scriptText =
			u8R"(
	
			set a to true and false
			set b to 1 = 2 or 3 < 4
			set c to 1 = 2 or 3 < 4 and 5 = 6
			set d to 1 != 2 or 3 >= 3 and 5 = 5
			set e to ((1 + 2) * 3) = ((1 + 2) * 3) and (1 + 2) * 3 < (1 + 2) * 4
			set f to 1 + 5 < 1 + 6
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == false);
		REQUIRE(script->GetVariable("b") == true);
		REQUIRE(script->GetVariable("c") == false);
		REQUIRE(script->GetVariable("d") == true);
		REQUIRE(script->GetVariable("e") == true);
		REQUIRE(script->GetVariable("f") == true);
	}

	SECTION("Test mixed expression precedence")
	{
		const char * scriptText =
			u8R"(
	
			set a to true and false
			set b to 1 = 2 or 3 < 4
			set c to 1 = 2 or 3 < 4 and 5 = 6
			set d to 1 + 2 * 3 = 1 + 2 * 3 and 1 + 2 * 3 < 1 + 2 * 4
			set e to ((1 + 2) * 3) = ((1 + 2) * 3) and 1 + 2 * 3 < 1 + 2 * 4

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == false);
		REQUIRE(script->GetVariable("b") == true);
		REQUIRE(script->GetVariable("c") == false);
		REQUIRE(script->GetVariable("d") == true);
		REQUIRE(script->GetVariable("e") == true);
	}
}
