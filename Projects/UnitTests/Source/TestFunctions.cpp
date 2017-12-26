/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


TEST_CASE("Test Functions", "[Functions]")
{
	SECTION("Test simple function")
	{
		static const char * scriptText =
			u8R"(
	
			function do nothing
				-- do nothing
			end

			do nothing
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
	}

	SECTION("Test no return assignment function")
	{
		static const char * scriptText =
			u8R"(
	
			function do nothing
				-- do nothing
			end

			set a to do nothing
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == nullptr);
	}

	SECTION("Test no return assignment function")
	{
		static const char * scriptText =
			u8R"(
	
			function return nothing
				return
			end

			set a to return nothing
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == nullptr);
	}

	SECTION("Test function with multiple return values and index operator")
	{
		static const char * scriptText =
			u8R"(

			function some values
				return "wolf", "goat", "cabbage"
			end

			set wolf to some values [1]
			set goat to some values [2]
			set cabbage to some values [3]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("wolf") == "wolf");
		REQUIRE(script->GetVariable("goat") == "goat");
		REQUIRE(script->GetVariable("cabbage") == "cabbage");
	}

	SECTION("Test alternate names function")
	{
		static const char * scriptText =
			u8R"(
	
			function do nothing/nada/ziltch
				-- do nothing
			end

			do nothing
			do nada
			do ziltch

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
	}

	SECTION("Test alternate names function")
	{
		static const char * scriptText =
			u8R"(
	
			function (a) b/c/d (e)
				-- do nothing
			end

			a b e
			a c e
			a d e
			b e
			c e
			d e
			b
			c
			d			

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
	}

	SECTION("Test optional names function")
	{
		static const char * scriptText =
			u8R"(
	
			function (opt/optional) (blah) {num} stuff
				-- do nothing
			end

			optional 123 stuff
			opt 456 stuff
			789 stuff
			blah 000 stuff
			opt blah 111 stuff

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
	}

	SECTION("Test local variable name collision with function")
	{
		static const char * scriptText =
			u8R"(
	
			set do to 1

			function do nothing/nada/ziltch
				-- do nothing
			end

			set nothing to 2
			set nada to 3
			set ziltch to 3

			do nothing
			do nada
			do ziltch
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
	}

	SECTION("Test simple return val")
	{
		static const char * scriptText =
			u8R"(
	
			function some constant integer
				return 42
			end

			function some string val
				return "some string"
			end

			set a to some constant integer
			set b to some string val
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 42);
		REQUIRE(script->GetVariable("b") == "some string");
	}

	SECTION("Test simple parameters")
	{
		static const char * scriptText =
			u8R"(
	
			function {a} plus {b}  
				return a + b
			end

			set x to 1
			set a to x plus 2
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 3);
	}

	SECTION("Test multi-part named parameters")
	{
		static const char * scriptText =
			u8R"(
	
			function {var a} plus {var b}  
				return var a + var b
			end

			set x to 1
			set a to x plus 2
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 3);
	}

	SECTION("Test properties as function parameters")
	{
		static const char * scriptText =
			u8R"(
	
			set public readonly x to 1

			function {a} plus {b}  
				return a + b
			end

			set a to x plus 2
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 3);
	}

	SECTION("Test multi-part properties as function parameters")
	{
		static const char * scriptText =
			u8R"(
	
			set public readonly x x to 1

			function {a} plus {b}  
				return a + b
			end

			set a to x x plus 2
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 3);
	}

	SECTION("Test complex expressions as function parameters #1")
	{
		static const char * scriptText =
			u8R"(
	
			function {x} test
				return x
			end
	 
			set a to 1 + 2 * 3 - 1 test
			set b to true and true test

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 6);
		REQUIRE(script->GetVariable("b") == true);
	}

	SECTION("Test complex expressions as function parameters #2")
	{
		static const char * scriptText =
			u8R"(
	
			function test {x} test
				return x
			end
	 
			set a to test 1 + 2 * 3 - 1 test
			set b to test true and true test

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 6);
		REQUIRE(script->GetVariable("b") == true);
	}

	SECTION("Test complex expressions as function parameters #3")
	{
		static const char * scriptText =
			u8R"(
	
			function test {x}
				return x
			end
	 
			set a to test 1 + 2 * 3 - 1
			set b to test true and true

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 6);
		REQUIRE(script->GetVariable("b") == true);
	}

/*
	SECTION("Test simple chained functions")
	{
		static const char * scriptText =
			u8R"(
	
			function {a} minus {b}  
				return a - b
			end
	 
			set a to (5 minus 3) minus 1	
			set b to 5 minus 3 minus 1
			set c to (4 + 3) minus (3 minus 1)

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 1);
		REQUIRE(script->GetVariable("b") == 3);
		REQUIRE(script->GetVariable("c") == 5);
	}
*/

	SECTION("Test functional recursion")
	{
		static const char * scriptText =
			u8R"(
	
			function factorial { x }
				if x <= 1
					return 1
				end
				return x * factorial (x - 1)
			end

			set a to factorial 7

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 5040);
	}

	SECTION("Test keyword-named functions")
	{
		static const char * scriptText =
			u8R"(
	
			-- Functions can be made up of keywords
			function loop function while
				return 42
			end

			function begin to while
				return 99
			end
	 
			set a to loop function while
			set b to begin to while

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 42);
		REQUIRE(script->GetVariable("b") == 99);
	}

	SECTION("Test parameter casts in functions")
	{
		static const char * scriptText =
			u8R"(
	
			function convert {string x}
				return x
			end

			function convertback {integer x}
				return x
			end

			set a to convert 123
			set b to convertback a
			set test a to convert 1234567890
			set test b to convertback test a

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsString());
		REQUIRE(script->GetVariable("a") == "123");
		REQUIRE(script->GetVariable("b").IsInteger());
		REQUIRE(script->GetVariable("b") == 123);
		REQUIRE(script->GetVariable("test a").IsString());
		REQUIRE(script->GetVariable("test a") == "1234567890");
		REQUIRE(script->GetVariable("test b").IsInteger());
		REQUIRE(script->GetVariable("test b") == 1234567890);
	}

	SECTION("Test potential function collision test")
	{
		static const char * scriptText1 =
			u8R"(
	
			-- Thto local function should be called from both scripts
			function localfunc
				return 123
			end

			private function privatefunc
				return localfunc
			end
			
			set a to privatefunc

			)";

		static const char * scriptText2 =
			u8R"(
	
			-- This function should not be called, even though it
			-- has the same name.
			function localfunc
				return 456
			end
			
			set a to privatefunc

			)";

		auto runtime = TestCreateRuntime();
		auto script1 = TestExecuteScript(scriptText1, runtime);
		auto script2 = TestExecuteScript(scriptText2, runtime);
		REQUIRE(script1);
		REQUIRE(script2);
		REQUIRE(script1->GetVariable("a") == 123);
		REQUIRE(script2->GetVariable("a") == 123);
	}

}