/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


TEST_CASE("Test Coroutines", "[Coroutines]")
{
	SECTION("Test core coroutine function with normal function")
	{
		static const char * scriptText =
			u8R"(

			import core

			-- Function declaration
			function test
				return 123
			end

			-- Execute function asynchronously and store coroutine in variable c
			set c to async call function test

			-- Wait until coroutine is finished
			wait until c is finished

			-- Retrieve return value from coroutine
			set v to c's value

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("c").IsCoroutine());
		REQUIRE(script->GetVariable("v") == 123);
	}

	SECTION("Test core coroutine functions with async function")
	{
		static const char * scriptText =
			u8R"(

			import core

			-- Function declaration
			function count to {integer y}
				set x to 0
				loop while x < y
					increment x
					wait
				end
				return x
			end

			-- Execute function asynchronously and store coroutine in variable c
			set c to async call function count to {} with 10

			-- Wait until coroutine is finished
			wait until c is finished

			-- Retrieve return value from coroutine
			set v to c's value

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("c").IsCoroutine());
		REQUIRE(script->GetVariable("v") == 10);
	}

	SECTION("Test multiple coroutines #1")
	{
		static const char * scriptText =
			u8R"(

			import core

			-- Function declaration
			function count to {integer y}
				set x to 0
				loop while x < y
					increment x
					wait
				end
				return x
			end

			-- Execute function asynchronously and store coroutine in variables
			set c1 to async call function count to {} with 15
			set c2 to async call function count to {} with 10
			set c3 to async call function count to {} with 5

			-- Wait until one of the coroutine is finished
			wait until c1 is finished or c2 is finished or c3 is finished

			-- Retrieve return value from coroutine
			set v1 to c1's value
			set v2 to c2's value
			set v3 to c3's value

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("c1").IsCoroutine());
		REQUIRE(script->GetVariable("c2").IsCoroutine());
		REQUIRE(script->GetVariable("c3").IsCoroutine());
		REQUIRE(script->GetVariable("v1") == nullptr);
		REQUIRE(script->GetVariable("v2") == nullptr);
		REQUIRE(script->GetVariable("v3") == 5);
	}

	SECTION("Test multiple coroutines #2")
	{
		static const char * scriptText =
			u8R"(

			import core

			-- Function declaration
			function count to {integer y}
				set x to 0
				loop while x < y
					increment x
					wait
				end
				return x
			end

			-- Execute function asynchronously and store coroutine in variables
			set c1 to async call function count to {} with 15
			set c2 to async call function count to {} with 10
			set c3 to async call function count to {} with 5

			-- Wait until one of the coroutine is finished
			wait until any of c1, c2, c3 is finished

			-- Retrieve return value from coroutine
			set v1 to c1's value
			set v2 to c2's value
			set v3 to c3's value

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("c1").IsCoroutine());
		REQUIRE(script->GetVariable("c2").IsCoroutine());
		REQUIRE(script->GetVariable("c3").IsCoroutine());
		REQUIRE(script->GetVariable("v1") == nullptr);
		REQUIRE(script->GetVariable("v2") == nullptr);
		REQUIRE(script->GetVariable("v3") == 5);
	}

	SECTION("Test multiple coroutines #2")
	{
		static const char * scriptText =
			u8R"(

			import core

			-- Function declaration
			function count to {integer y}
				set x to 0
				loop while x < y
					increment x
					wait
				end
				return x
			end

			-- Execute function asynchronously and store coroutine in function
			set c1 to async call function count to {} with 15
			set c2 to async call function count to {} with 10
			set c3 to async call function count to {} with 5

			-- Wait until all the coroutines are finished
			wait until c1 is finished and c2 is finished and c3 is finished

			-- Retrieve return value from coroutine
			set v1 to c1's value
			set v2 to c2's value
			set v3 to c3's value

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("c1").IsCoroutine());
		REQUIRE(script->GetVariable("c2").IsCoroutine());
		REQUIRE(script->GetVariable("c3").IsCoroutine());
		REQUIRE(script->GetVariable("v1") == 15);
		REQUIRE(script->GetVariable("v2") == 10);
		REQUIRE(script->GetVariable("v3") == 5);
	}

	SECTION("Test multiple coroutines #2")
	{
		static const char * scriptText =
			u8R"(

			import core

			-- Function declaration
			function count to {integer y}
				set x to 0
				loop while x < y
					increment x
					wait
				end
				return x
			end

			-- Execute function asynchronously and store coroutine in function
			set c1 to async call function count to {} with 15
			set c2 to async call function count to {} with 10
			set c3 to async call function count to {} with 5

			-- Wait until all the coroutines are finished
			wait until all of c1, c2, c3 are finished

			-- Retrieve return value from coroutine
			set v1 to c1's value
			set v2 to c2's value
			set v3 to c3's value

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("c1").IsCoroutine());
		REQUIRE(script->GetVariable("c2").IsCoroutine());
		REQUIRE(script->GetVariable("c3").IsCoroutine());
		REQUIRE(script->GetVariable("v1") == 15);
		REQUIRE(script->GetVariable("v2") == 10);
		REQUIRE(script->GetVariable("v3") == 5);
	}
}