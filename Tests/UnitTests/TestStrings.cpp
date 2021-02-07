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
			R"(

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

	SECTION("Test string index pair operator var get")
	{
		const char * scriptText =
			R"(

			set a to "Hello world!"
			set b to a[1, 5]
			set c to a[7, 11]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == "Hello world!");
		REQUIRE(script->GetVariable("b") == "Hello");
		REQUIRE(script->GetVariable("c") == "world");
	}

	SECTION("Test UTF-8 string index operator var get")
	{
		const char * scriptText =
			u8R"(

			set a to "いろは"
			set b to a[1]
			set c to a[2]
			set d to a[3]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == u8"いろは");
		REQUIRE(script->GetVariable("b") == u8"い");
		REQUIRE(script->GetVariable("c") == u8"ろ");
		REQUIRE(script->GetVariable("d") == u8"は");
	}

	SECTION("Test UTF-8 string index pair operator var get")
	{
		const char * scriptText =
			u8R"(

			set a to "いろは"
			set b to a[1, 2]
			set c to a[2, 3]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == u8"いろは");
		REQUIRE(script->GetVariable("b") == u8"いろ");
		REQUIRE(script->GetVariable("c") == u8"ろは");
	}

	SECTION("Test string index operator var set")
	{
		const char * scriptText =
			R"(

			set a to "hello world!"
			set a[1] to "H"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == "Hello world!");
	}

	SECTION("Test string index pair operator var set")
	{
		const char * scriptText =
			R"(

			set a to "hello world!"
			set a[1, 5] to "HELLO"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == "HELLO world!");
	}

	SECTION("Test UTF-8 string index operator var set")
	{
		const char * scriptText =
			u8R"(

			set a to "いろは"
			set a[1] to "は"
			set a[3] to "い"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == u8"はろい");
	}

	SECTION("Test UTF-8 string index pair operator var set")
	{
		const char * scriptText =
			u8R"(

			set a to "いろは"
			set a[1, 2] to "はろ"
			set a[2, 3] to "ろい"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == u8"はろい");
	}

	SECTION("Test UTF-8 string index operator var set mixed #1")
	{
		const char * scriptText =
			u8R"(

			set a to "Hello"
			set a[2] to "は"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == u8"Hはllo");
	}

	SECTION("Test UTF-8 string index operator var set mixed #2")
	{
		const char * scriptText =
			u8R"(

			set a to "いろは"
			set a[2] to "-"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == u8"い-は");
	}

	SECTION("Test UTF-8 string index operator var set mixed #3")
	{
		const char * scriptText =
			u8R"(

			set a to "いろは"
			set a[2] to "---"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == u8"い---は");
	}

	SECTION("Test UTF-8 string index operator var set mixed #4")
	{
		const char * scriptText =
			u8R"(

			set a to "Hello"
			set a[2] to "ははは"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == u8"Hはははllo");
	}

	SECTION("Test UTF-8 string index pair operator var set mixed #1")
	{
		const char * scriptText =
			u8R"(

			set a to "Hello"
			set a[2, 4] to "は"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == u8"Hはo");
	}


	SECTION("Test UTF-8 string index pair operator var set mixed #2")
	{
		const char * scriptText =
			u8R"(

			set a to "Hello"
			set a[2, 4] to "いろは"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == u8"Hいろはo");
	}

	SECTION("Test string index operator prop get")
	{
		const char * scriptText =
			R"(

			set public a to "Hello world!"
			set b to a[1]
			set c to a[12]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		auto library = script->GetLibrary();
		REQUIRE(library->GetProperty("a") == "Hello world!");
		REQUIRE(script->GetVariable("b") == "H");
		REQUIRE(script->GetVariable("c") == "!");
	}

	SECTION("Test string index pair operator prop get")
	{
		const char * scriptText =
			R"(

			set public a to "Hello world!"
			set b to a[1, 5]
			set c to a[7, 11]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		auto library = script->GetLibrary();
		REQUIRE(library->GetProperty("a") == "Hello world!");
		REQUIRE(script->GetVariable("b") == "Hello");
		REQUIRE(script->GetVariable("c") == "world");
	}

	SECTION("Test string index operator prop set")
	{
		const char * scriptText =
			R"(

			set public a to "hello world!"
			set a[1] to "H"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		auto library = script->GetLibrary();
		REQUIRE(library->GetProperty("a") == "Hello world!");
	}

	SECTION("Test string index pair operator prop set")
	{
		const char * scriptText =
			R"(

			set public a to "hello world!"
			set a[1, 5] to "HELLO"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		auto library = script->GetLibrary();
		REQUIRE(library->GetProperty("a") == "HELLO world!");
	}

	SECTION("Test string index operator var get from collection")
	{
		const char * scriptText =
			R"(

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
			R"(

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
