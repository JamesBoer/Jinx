/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


TEST_CASE("Test Statements", "[Statements]")
{
	SECTION("Test comments")
	{
		const char * scriptText =
			u8R"(

			-- Single line comment

			--- Multiline 
				comment ---
				
			---------
			Alternate
			multiline
			comment
			---------

---------
No indentation
multiline comment
---------

			set a --- some comment --- to 123
   
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 123);
	}

	SECTION("Test variables and basic statements")
	{
		const char * scriptText =
			u8R"(

			set a to "Hello world!"
			set b to 5.5
			set c to 123
			set d to true
			set e to null

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == "Hello world!");
		REQUIRE(script->GetVariable("b") == 5.5);
		REQUIRE(script->GetVariable("c") == 123);
		REQUIRE(script->GetVariable("d") == true);
		REQUIRE(script->GetVariable("e") == nullptr);
	}

	SECTION("Test multiple word variables and basic statements")
	{
		const char * scriptText =
			u8R"(

			set a a to "Hello world!"
			set b b to 5.5
			set c c to 123
			set d d to true
			set e e to null

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a a") == "Hello world!");
		REQUIRE(script->GetVariable("b b") == 5.5);
		REQUIRE(script->GetVariable("c c") == 123);
		REQUIRE(script->GetVariable("d d") == true);
		REQUIRE(script->GetVariable("e e") == nullptr);
	}

	SECTION("Test multiple word variables with keywords")
	{
		const char * scriptText =
			u8R"(

			set some string to "Hello world!"
			set some number to 5.5
			set some integer to 123
			set some boolean to true
			set some null to null

			external some other string

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("some string") == "Hello world!");
		REQUIRE(script->GetVariable("some number") == 5.5);
		REQUIRE(script->GetVariable("some integer") == 123);
		REQUIRE(script->GetVariable("some boolean") == true);
		REQUIRE(script->GetVariable("some null") == nullptr);
	}

	SECTION("Test variable and property type")
	{
		const char * scriptText =
			u8R"(

			set a to 123
			set private b to 234
			set c to a type
			set d to false
			if a type = b type
				set d to true
			end
			set e to false
			if a type = integer
				set e to true
			end
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("c").IsValType());
		REQUIRE(script->GetVariable("c").GetValType() == ValueType::Integer);
		REQUIRE(script->GetVariable("d") == true);
		REQUIRE(script->GetVariable("e") == true);
	}

	SECTION("Test alternate numeric form parsing")
	{
		const char * scriptText =
			u8R"(
    
			set a to -375.5
			set b to .11111
			set c to -999
			set d to 00001

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == -375.5);
		REQUIRE(script->GetVariable("b").GetNumber() == Approx(.11111));
		REQUIRE(script->GetVariable("c") == -999);
		REQUIRE(script->GetVariable("d") == 1);
	}

	SECTION("Test variable scope")
	{
		const char * scriptText =
			u8R"(

			-- Scope test - a will not be visible after end
			begin
				set a to 42
			end

			-- Scope test - g will be visible inside scope block
			set b to 999
			begin
				set b to 55
			end

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == nullptr);
		REQUIRE(script->GetVariable("b") == 55);
	}

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

			-- Multiple operations with and without parentheses
			set h to 1 + 2 * 3     -- h = 9
			set i to 1 + (2 * 3)   -- i = 7

			-- Floating point assignments
			set j to 123.456
			set k to 23.45

			-- Floating point operations
			set l to j * k
			set m to j / k
			set n to j + k
			set o to j - k
			set p to j % k

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
		REQUIRE(script->GetVariable("h") == 9);
		REQUIRE(script->GetVariable("i") == 7);
		REQUIRE(script->GetVariable("j").GetNumber() == Approx(123.456));
		REQUIRE(script->GetVariable("k").GetNumber() == Approx(23.45));
		REQUIRE(script->GetVariable("l").GetNumber() == Approx(2895.0432));
		REQUIRE(script->GetVariable("m").GetNumber() == Approx(5.2646));
		REQUIRE(script->GetVariable("n").GetNumber() == Approx(146.906));
		REQUIRE(script->GetVariable("o").GetNumber() == Approx(100.006));
		REQUIRE(script->GetVariable("p").GetNumber() == Approx(6.206));
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

	SECTION("Test increment and decrement operators")
	{
		const char * scriptText =
			u8R"(
    
			-- Increment and decrement test
			set a to 1
			increment a
			set b to 1
			decrement b
			set c to 1
			increment c by 4
			set d to 1
			decrement d by 3
			set e to 1
			increment e by 4 * (2 + 2)

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 2);
		REQUIRE(script->GetVariable("b") == 0);
		REQUIRE(script->GetVariable("c") == 5);
		REQUIRE(script->GetVariable("d") == -2);
		REQUIRE(script->GetVariable("e") == 17);
	}


	SECTION("Test wait statement")
	{
		const char * scriptText =
			u8R"(
    
			set private counter to 0
			
			function return counter to finished
				increment counter
				return counter >= 10
			end

			loop while not counter to finished
				wait
			end

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		auto library = script->GetLibrary();
		REQUIRE(library->GetProperty("counter") == 10);
	}

	SECTION("Test wait while statement")
	{
		const char * scriptText =
			u8R"(
    
			set private counter to 0
			
			function return counter to finished
				increment counter
				return counter >= 10
			end

			wait while not counter to finished

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		auto library = script->GetLibrary();
		REQUIRE(library->GetProperty("counter") == 10);
	}

	SECTION("Test wait until statement")
	{
		const char * scriptText =
			u8R"(
    
			set private counter to 0
			
			function return counter to finished
				increment counter
				return counter >= 10
			end

			wait until counter to finished

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		auto library = script->GetLibrary();
		REQUIRE(library->GetProperty("counter") == 10);
	}

	SECTION("Test external statement")
	{
		const char * scriptText =
			u8R"(
    
			external some var
			set another var to some var

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
		script->SetVariable("some var", 123);
		script->Execute();
		REQUIRE(script->GetVariable("another var") == 123);
	}

}
