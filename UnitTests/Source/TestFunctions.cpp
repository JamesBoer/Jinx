/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
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

	SECTION("Test local variable name collision with function")
	{
		static const char * scriptText =
			u8R"(
    
			do is 1

			function do nothing/nada/ziltch
				-- do nothing
			end

			nothing is 2
			nada is 3
			ziltch is 3

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
    
			function return some constant integer
				return 42
			end

			function return some string val
				return "some string"
			end

			a is some constant integer
			b is some string val
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetInteger() == 42);
		REQUIRE(script->GetVariable("b").GetString() == "some string");
	}

	SECTION("Test return value validation #1")
	{
		static const char * scriptText =
			u8R"(
    
			function return somefunc
				if (true)
					return "some string"
				else
					return "some string"
				end
			end

			a is somefunc

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetString() == "some string");
	}


	SECTION("Test return value validation #2")
	{
		static const char * scriptText =
			u8R"(
    
			function return somefunc
				if (true)
					return "some string"
				else
				end
				return "some string"
			end

			a is somefunc

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetString() == "some string");
	}

	SECTION("Test return value validation #3")
	{
		static const char * scriptText =
			u8R"(
    
			function return somefunc
				if (true)
				else
					return "some string"
				end
				return "some string"
			end

			a is somefunc

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetString() == "some string");
	}

	SECTION("Test return value validation #4")
	{
		static const char * scriptText =
			u8R"(
    
			function return somefunc
				if (true)
				else if (false)
					return "some string"
				end
				return "some string"
			end

			a is somefunc

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetString() == "some string");
	}

	SECTION("Test simple parameters")
	{
		static const char * scriptText =
			u8R"(
    
			function return {a} plus {b}  
				return a + b
			end

			x is 1
			a is x plus 2
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetInteger() == 3);
	}
	
	SECTION("Test properties as function parameters")
	{
		static const char * scriptText =
			u8R"(
    
			readonly public x is 1

			function return {a} plus {b}  
				return a + b
			end

			a is x plus 2
		
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetInteger() == 3);
	}
	
	SECTION("Test simple chained functions")
	{
		static const char * scriptText =
			u8R"(
    
			function return {a} minus {b}  
				return a - b
			end
     
			a is (5 minus 3) minus 1	
			b is 5 minus 3 minus 1
			c is (4 + 3) minus (3 minus 1)

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetInteger() == 1);
		REQUIRE(script->GetVariable("b").GetInteger() == 3);
		REQUIRE(script->GetVariable("c").GetInteger() == 5);
	}

	SECTION("Test functional recursion")
	{
		static const char * scriptText =
			u8R"(
    
			function return factorial { x }
				if x <= 1
					return 1
				end
				return x * factorial (x - 1)
			end

			a is factorial 7

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetInteger() == 5040);
	}

	SECTION("Test keyword-named functions")
	{
		static const char * scriptText =
			u8R"(
    
			-- Functions can be made up of keywords
			function return loop function while
				return 42
			end

			function return begin is while
				return 99
			end
     
			a is loop function while
			b is begin is while

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetInteger() == 42);
		REQUIRE(script->GetVariable("b").GetInteger() == 99);
	}

	SECTION("Test parameter casts in functions")
	{
		static const char * scriptText =
			u8R"(
    
			function return convert {string x}
				return x
			end

			function return convertback {integer x}
				return x
			end

			a is convert 123
			b is convertback a

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsString());
		REQUIRE(script->GetVariable("a").GetString() == "123");
		REQUIRE(script->GetVariable("b").IsInteger());
		REQUIRE(script->GetVariable("b").GetInteger() == 123);
	}

	SECTION("Test potential function collision test")
	{
		static const char * scriptText1 =
			u8R"(
    
			-- This local function should be called from both scripts
			function return localfunc
				return 123
			end

			private function return privatefunc
				return localfunc
			end
			
			a is privatefunc

			)";

		static const char * scriptText2 =
			u8R"(
    
			-- This function should not be called, even though it
			-- has the same name.
			function return localfunc
				return 456
			end
			
			a is privatefunc

			)";

		auto runtime = TestCreateRuntime();
		auto script1 = TestExecuteScript(scriptText1, runtime);
		auto script2 = TestExecuteScript(scriptText2, runtime);
		REQUIRE(script1);
		REQUIRE(script2);
		REQUIRE(script1->GetVariable("a").GetInteger() == 123);
		REQUIRE(script2->GetVariable("a").GetInteger() == 123);
	}

}