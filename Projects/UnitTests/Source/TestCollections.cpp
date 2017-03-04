/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


TEST_CASE("Test Collections", "[Collections]")
{
	SECTION("Test create empty collection")
	{
		static const char * scriptText =
			u8R"(
    
			-- Create empty collection
			a is []
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
	}

	SECTION("Test collection initialization list")
	{
		static const char * scriptText =
			u8R"(
    
			-- Create collection using an initialization list
			a is 3, 2, 1
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 3);
		REQUIRE(collection->find(1) != collection->end());
		REQUIRE(collection->find(1)->second.GetInteger() == 3);
		REQUIRE(collection->find(2) != collection->end());
		REQUIRE(collection->find(2)->second.GetInteger() == 2);
		REQUIRE(collection->find(3) != collection->end());
		REQUIRE(collection->find(3)->second.GetInteger() == 1);
	}


	SECTION("Test collection addition of elements using assignment")
	{
		static const char * scriptText =
			u8R"(
    
			-- Create empty collection and add three elements
			a is []
			a [1] is 3
			a [2] is 2
			a [3] is 1
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 3);
		REQUIRE(collection->find(1) != collection->end());
		REQUIRE(collection->find(1)->second.GetInteger() == 3);
		REQUIRE(collection->find(2) != collection->end());
		REQUIRE(collection->find(2)->second.GetInteger() == 2);
		REQUIRE(collection->find(3) != collection->end());
		REQUIRE(collection->find(3)->second.GetInteger() == 1);
	}

	SECTION("Test collection initialization list of key-value pairs")
	{
		static const char * scriptText =
			u8R"(
    
			-- Create collection using an initialization list of key-value pairs		
			a is [1, "red"], [2, "green"], [3, "blue"]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 3);
		REQUIRE(collection->find(1) != collection->end());
		REQUIRE(collection->find(1)->second.GetString() == "red");
		REQUIRE(collection->find(2) != collection->end());
		REQUIRE(collection->find(2)->second.GetString() == "green");
		REQUIRE(collection->find(3) != collection->end());
		REQUIRE(collection->find(3)->second.GetString() == "blue");
	}

	SECTION("Test assignment of collection element by index operator for variable")
	{
		static const char * scriptText =
			u8R"(
    
			-- Create collection using an initialization list of key-value pairs		
			a is [1, "red"], [2, "green"], [3, "blue"]
			
			-- Change one of the elements by index
			a[2] is "magenta"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 3);
		REQUIRE(collection->find(2) != collection->end());
		REQUIRE(collection->find(2)->second.GetString() == "magenta");
	}

	SECTION("Test assignment of collection element by index operator for property")
	{
		static const char * scriptText =
			u8R"(
    
			-- Create collection using an initialization list of key-value pairs		
			private a is [1, "red"], [2, "green"], [3, "blue"]
			
			-- Change one of the elements by index
			a[2] is "magenta"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		auto library = script->GetLibrary();
		REQUIRE(library);
		REQUIRE(library->GetProperty("a").IsCollection());
		auto collection = library->GetProperty("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 3);
		REQUIRE(collection->find(2) != collection->end());
		REQUIRE(collection->find(2)->second.GetString() == "magenta");
	}

	SECTION("Test assignment of collection variable by key")
	{
		static const char * scriptText =
			u8R"(
    
			-- Create collection using an initialization list of key-value pairs		
			a is [1, "red"], [2, "green"], [3, "blue"]
			
			-- Set variable to one of the collection values
			b is a[2]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("b").GetString() == "green");
	}

	SECTION("Test assignment of collection property by key")
	{
		static const char * scriptText =
			u8R"(
    
			-- Create collection using an initialization list of key-value pairs		
			private a is [1, "red"], [2, "green"], [3, "blue"]
			
			-- Set variable to one of the collection values
			b is a[2]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("b").GetString() == "green");
	}

	SECTION("Test adding auto-indexed value to existing collection")
	{
	static const char * scriptText =
		u8R"(
			import core

			-- Create collection using an initialization list of key-value pairs		
			a is [1, "red"], [2, "green"], [3, "blue"]
			
			-- Add single element to a
			a[(a size + 1)] is "purple"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 4);
		REQUIRE(collection->find(4) != collection->end());
		REQUIRE(collection->find(4)->second.GetString() == "purple");
	}

	SECTION("Test erasing single element from collection")
	{
		static const char * scriptText =
			u8R"(
			import core

			-- Create collection using an initialization list of key-value pairs		
			a is [1, "red"], [2, "green"], [3, "blue"]
			
			-- Remove element by key
			a[2] is null

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 2);
		REQUIRE(collection->find(2) == collection->end());
	}
	
}