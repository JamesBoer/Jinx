/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


TEST_CASE("Test Strings", "[Strings]")
{

	SECTION("Test string index operator var get")
	{
		const char * scriptText =
			u8R"(

			set a to "Hello world!"
			set b to a[1]
			set c to a[12]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == "Hello world!");
		REQUIRE(script->GetVariable("b") == "H");
		REQUIRE(script->GetVariable("c") == "!");
	}

	SECTION("Test string index operator prop get")
	{
		const char * scriptText =
			u8R"(

			set public a to "hello world!"
			set a[1] to "H"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		auto library = script->GetLibrary();
		REQUIRE(library->GetProperty("a") == "Hello world!");
	}

	SECTION("Test string index operator var set")
	{
		const char * scriptText =
			u8R"(

			set a to "hello world!"
			set a[1] to "H"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == "Hello world!");
	}

	SECTION("Test string index operator prop set")
	{
		const char * scriptText =
			u8R"(

			set public a to "hello world!"
			set a[1] to "H"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		auto library = script->GetLibrary();
		REQUIRE(library->GetProperty("a") == "Hello world!");
	}

	SECTION("Test string index operator var get from collection")
	{
		const char * scriptText =
			u8R"(

			set a to "cat", "dog", "Hello world!"
			set b to a[3][1]
			set c to a[3][12]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("b") == "H");
		REQUIRE(script->GetVariable("c") == "!");
	}

	SECTION("Test string index operator prop get from collection")
	{
		const char * scriptText =
			u8R"(

			set public a to "cat", "dog", "Hello world!"
			set b to a[3][1]
			set c to a[3][12]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("b") == "H");
		REQUIRE(script->GetVariable("c") == "!");
	}


}
