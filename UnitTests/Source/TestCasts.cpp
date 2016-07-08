/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


TEST_CASE("Test Casts", "[Casts]")
{
	SECTION("Test variable casting")
	{
		static const char * scriptText =
			u8R"(
    
			a is 123.456 as integer
			b is true as string
			c is "false" as boolean
			d is "456" as integer
			e is "-123.456" as number
			f is 4 + 5 + 6 as string
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsInteger());
		REQUIRE(script->GetVariable("a").GetInteger() == 123);
		REQUIRE(script->GetVariable("b").IsString());
		REQUIRE(script->GetVariable("b").GetString() == "true");
		REQUIRE(script->GetVariable("c").IsBoolean());
		REQUIRE(script->GetVariable("c").GetBoolean() == false);
		REQUIRE(script->GetVariable("d").IsInteger());
		REQUIRE(script->GetVariable("d").GetInteger() == 456);
		REQUIRE(script->GetVariable("e").IsNumber());
		REQUIRE(script->GetVariable("e").GetNumber() == Approx(-123.456));
		REQUIRE(script->GetVariable("f").IsString());
		REQUIRE(script->GetVariable("f").GetString() == "15");
	}
    
    SECTION("Test additional casts")
    {
        const char * scriptText =
			u8R"(
        
			a is 12345
			b is a as string
			c is b as integer
			d is true
			e is d as string
			f is e as boolean
			g is false
			if g type = boolean and g type = d type
				g is true
            end
            
            )";
            
            auto script = TestExecuteScript(scriptText);
        REQUIRE(script);
        REQUIRE(script->GetVariable("a").GetNumber() == 12345);
        REQUIRE(script->GetVariable("b").GetString() == "12345");
        REQUIRE(script->GetVariable("c").GetInteger() == 12345);
        REQUIRE(script->GetVariable("d").GetBoolean() == true);
        REQUIRE(script->GetVariable("e").GetString() == "true");
        REQUIRE(script->GetVariable("f").GetBoolean() == true);
        REQUIRE(script->GetVariable("g").GetBoolean() == true);
    }
    
}