/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


TEST_CASE("Test Casts", "[Casts]")
{
	SECTION("Test variable casting")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 123.456 as integer
			set b to true as string
			set c to "false" as boolean
			set d to "456" as integer
			set e to "-123.456" as number
			set f to (4 + 5 + 6) as string
			set g to 0 as null
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsInteger());
		REQUIRE(script->GetVariable("a") == 123);
		REQUIRE(script->GetVariable("b").IsString());
		REQUIRE(script->GetVariable("b") == "true");
		REQUIRE(script->GetVariable("c").IsBoolean());
		REQUIRE(script->GetVariable("c") == false);
		REQUIRE(script->GetVariable("d").IsInteger());
		REQUIRE(script->GetVariable("d") == 456);
		REQUIRE(script->GetVariable("e").IsNumber());
		REQUIRE(script->GetVariable("e").GetNumber() == Catch::Approx(-123.456));
		REQUIRE(script->GetVariable("f").IsString());
		REQUIRE(script->GetVariable("f") == "15");
		REQUIRE(script->GetVariable("g").IsNull());
		REQUIRE(script->GetVariable("g") == nullptr);
	}
	
	SECTION("Test additional casts")
	{
		const char * scriptText =
			u8R"(
		
			set a to 12345
			set b to a as string
			set c to b as integer
			set d to true
			set e to d as string
			set f to e as boolean
			set g to false
			if g type = boolean and g type = d type
				set g to true
			end
			set h to (3 / 2) as integer   -- 1
			)";
			
		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 12345);
		REQUIRE(script->GetVariable("b") == "12345");
		REQUIRE(script->GetVariable("c") == 12345);
		REQUIRE(script->GetVariable("d") == true);
		REQUIRE(script->GetVariable("e") == "true");
		REQUIRE(script->GetVariable("f") == true);
		REQUIRE(script->GetVariable("g") == true);
		REQUIRE(script->GetVariable("h") == 1);
	}

	SECTION("Test guid cast")
	{
		const char * scriptText =
			u8R"(
		
			set a to "06DF8818-07DB-4AAB-9BF6-3365D0F2D4C9" as guid

			)";

		Guid guid = { 0x06DF8818, 0x07DB, 0x4AAB, {0x9B, 0xF6, 0x33, 0x65, 0xD0, 0xF2, 0xD4, 0xC9} };
		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsGuid());
		REQUIRE(script->GetVariable("a").GetGuid() == guid);
	}

}
