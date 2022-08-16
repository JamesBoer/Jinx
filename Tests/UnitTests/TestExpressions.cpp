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
		REQUIRE(script->GetVariable("h").GetNumber() == Catch::Approx(123.456));
		REQUIRE(script->GetVariable("i").GetNumber() == Catch::Approx(23.45));
		REQUIRE(script->GetVariable("j").GetNumber() == Catch::Approx(2895.0432));
		REQUIRE(script->GetVariable("k").GetNumber() == Catch::Approx(5.2646));
		REQUIRE(script->GetVariable("l").GetNumber() == Catch::Approx(146.906));
		REQUIRE(script->GetVariable("m").GetNumber() == Catch::Approx(100.006));
		REQUIRE(script->GetVariable("n").GetNumber() == Catch::Approx(6.206));
	}

	SECTION("Test unary negation operator")
	{
		const char * scriptText =
			u8R"(

				set a to -1
				set b to - 2
				set c to -(3)
				set d to a - -b
				set e to - a - -b
				set f to a - -(b)
				set g to - (a) - -b
				set h to 123.456
				set i to -h
				set j to -(1 + 2)
				set k to -a - -a	
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == -1);
		REQUIRE(script->GetVariable("b") == -2);
		REQUIRE(script->GetVariable("c") == -3);
		REQUIRE(script->GetVariable("d") == -3);
		REQUIRE(script->GetVariable("e") == -1);
		REQUIRE(script->GetVariable("f") == -3);
		REQUIRE(script->GetVariable("g") == -1);
		REQUIRE(script->GetVariable("h").GetNumber() == Catch::Approx(123.456));
		REQUIRE(script->GetVariable("i").GetNumber() == Catch::Approx(-123.456));
		REQUIRE(script->GetVariable("j") == -3);
		REQUIRE(script->GetVariable("k") == 0);
	}

	SECTION("Test mod operators with signs")
	{
		const char * scriptText =
			u8R"(
	
			set a to 5 % 3
			set b to -5 % 3
			set c to 5 % -3
			set d to -5 % -3

			set e to 5.0 % 3.0
			set f to -5.0 % 3.0
			set g to 5.0 % -3.0
			set h to -5.0 % -3.0

			set i to 6 % 3
			set j to -6 % 3

			set k to 6.0 % 3.0
			set l to -6.0 % 3.0

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 2);
		REQUIRE(script->GetVariable("b") == 1);
		REQUIRE(script->GetVariable("c") == -1);
		REQUIRE(script->GetVariable("d") == -2);
		REQUIRE(script->GetVariable("e") == 2.0);
		REQUIRE(script->GetVariable("f") == 1.0);
		REQUIRE(script->GetVariable("g") == -1.0);
		REQUIRE(script->GetVariable("h") == -2.0);
		REQUIRE(script->GetVariable("i") == 0);
		REQUIRE(script->GetVariable("j") == 0);
		REQUIRE(script->GetVariable("k") == 0.0);
		REQUIRE(script->GetVariable("l") == 0.0);
	}

	SECTION("Test equality and inequality operators")
	{
		const char * scriptText =
			u8R"(
			
			set a to true = true		-- true
			set b to true != true		-- false
			set c to "apple" = 123.45   -- false
			set d to null != 5			-- true
			set e to null = null		-- true
			set f to "str" = "str"		-- true

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == true);
		REQUIRE(script->GetVariable("b") == false);
		REQUIRE(script->GetVariable("c") == false);
		REQUIRE(script->GetVariable("d") == true);
		REQUIRE(script->GetVariable("e") == true);
		REQUIRE(script->GetVariable("f") == true);
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
			set f to not 1 < 2 or 4 != 5    -- true
			set g to not (1 < 2 or 4 != 5)  -- false
			set h to (not 1 < 2) or 4 != 5  -- true
			set i to not false				-- true
			set j to not 3 = 3				-- false
			set k to not 1 + 2 + 3 > 100	-- true
			set l to not 123				-- false
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == false);
		REQUIRE(script->GetVariable("b") == true);
		REQUIRE(script->GetVariable("c") == false);
		REQUIRE(script->GetVariable("d") == true);
		REQUIRE(script->GetVariable("e") == true);
		REQUIRE(script->GetVariable("f") == true);
		REQUIRE(script->GetVariable("g") == false);
		REQUIRE(script->GetVariable("h") == true);
		REQUIRE(script->GetVariable("i") == true);
		REQUIRE(script->GetVariable("j") == false);
		REQUIRE(script->GetVariable("k") == true);
		REQUIRE(script->GetVariable("l") == false);
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

			set k to 1 <= 2 and 2 >= 1
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
		REQUIRE(script->GetVariable("k") == true);
	}

	SECTION("Test less than or greater than operators on mixed numeric types")
	{
		const char * scriptText =
			u8R"(
			
			set a to 1 < 1.5				
			set b to 2 < 1.99999	
			
			set c to 1.99999 > 2				
			set d to 2 > 1	
			
			set e to 1 <= 2.01
			set f to 1 <= 1
			set g to 2 <= 1

			set h to 1.5 >= 2
			set i to 1.1 >= 1
			set j to 2.0 >= 1

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

	SECTION("Test string concatenation")
	{
		const char * scriptText =
			u8R"(
			
			set a to "together " + "forever"
			set b to "another" + " " + "try"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == "together forever");
		REQUIRE(script->GetVariable("b") == "another try");
	}

	SECTION("Test automatic string conversion and concatenation")
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

	SECTION("Test short circuit evaluation #1")
	{
		const char * scriptText =
			u8R"(
	
			set private x to false

			function f
				set x to true
			end

			set a to 1 = 1 or f

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == true);
		REQUIRE(script->GetLibrary()->GetProperty("x") == false);
	}

	SECTION("Test short circuit evaluation #2")
	{
		const char * scriptText =
			u8R"(
	
			set private x to false

			function f
				set x to true
			end

			set a to 1 = 2 and f

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == false);
		REQUIRE(script->GetLibrary()->GetProperty("x") == false);
	}

	SECTION("Test short circuit evaluation #3")
	{
		const char * scriptText =
			u8R"(
	
			set private x to false
			set private y to false

			function f1
				set x to true
				return true
			end

			function f2
				set y to true
				return true
			end

			set a to (1 = 1 and 2 = 2) and (f1 or f2)

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == true);
		REQUIRE(script->GetLibrary()->GetProperty("x") == true);
		REQUIRE(script->GetLibrary()->GetProperty("y") == false);
	}

	SECTION("Test short circuit evaluation #4")
	{
		const char * scriptText =
			u8R"(
	
			set private x to false
			set private y to false

			function f1
				set x to true
				return true
			end

			function f2
				set y to true
				return true
			end

			set a to (1 = 1 and 2 = 2) or (f1 or f2)

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == true);
		REQUIRE(script->GetLibrary()->GetProperty("x") == false);
		REQUIRE(script->GetLibrary()->GetProperty("y") == false);
	}

	SECTION("Test short circuit evaluation #5")
	{
		const char * scriptText =
			u8R"(
	
			set private x to false
			set private y to false

			function f1
				set x to true
				return true
			end

			function f2
				set y to true
				return true
			end

			set a to (1 = 1 and 2 = 3) and (f1 or f2)

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == false);
		REQUIRE(script->GetLibrary()->GetProperty("x") == false);
		REQUIRE(script->GetLibrary()->GetProperty("y") == false);
	}

	SECTION("Test short circuit evaluation #6")
	{
		const char * scriptText =
			u8R"(
	
			set private x to false
			set private y to false

			function f1
				set x to true
				return false
			end

			function f2
				set y to true
				return true
			end

			set a to (1 = 1 and 2 = 2) and (f1 and f2)

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == false);
		REQUIRE(script->GetLibrary()->GetProperty("x") == true);
		REQUIRE(script->GetLibrary()->GetProperty("y") == false);
	}

	SECTION("Test short circuit evaluation #7")
	{
		const char * scriptText =
			u8R"(
	
			set private x to false
			set private y to false

			function f1
				set x to true
				return false
			end

			function f2
				set y to true
				return true
			end

			set a to (1 = 1 or 2 = 2) or (f1 or f2)

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == true);
		REQUIRE(script->GetLibrary()->GetProperty("x") == false);
		REQUIRE(script->GetLibrary()->GetProperty("y") == false);
	}

	SECTION("Test short circuit evaluation #8")
	{
		const char * scriptText =
			u8R"(
	
			set private x to false
			set private y to false

			function f1
				set x to true
				return true
			end

			function f2
				set y to true
				return true
			end

			set a to (1 = 2 or 2 = 3 or 3 = 4 or 4 = 5) or (f1 or f2)

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == true);
		REQUIRE(script->GetLibrary()->GetProperty("x") == true);
		REQUIRE(script->GetLibrary()->GetProperty("y") == false);
	}
}
