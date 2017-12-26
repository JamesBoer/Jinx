/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
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
			set a to []
			
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
			set a to 3, 2, 1
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 3);
		REQUIRE(collection->find(1) != collection->end());
		REQUIRE(collection->find(1)->second == 3);
		REQUIRE(collection->find(2) != collection->end());
		REQUIRE(collection->find(2)->second == 2);
		REQUIRE(collection->find(3) != collection->end());
		REQUIRE(collection->find(3)->second == 1);
	}

	SECTION("Test collection initialization list on new lines")
	{
		static const char * scriptText =
			u8R"(
	
			-- Create collection using an initialization list
			set a to... 
				3, 
				2, 
				1
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 3);
		REQUIRE(collection->find(1) != collection->end());
		REQUIRE(collection->find(1)->second == 3);
		REQUIRE(collection->find(2) != collection->end());
		REQUIRE(collection->find(2)->second == 2);
		REQUIRE(collection->find(3) != collection->end());
		REQUIRE(collection->find(3)->second == 1);
	}

	SECTION("Test collection initialization list on new lines with spacing")
	{
		static const char * scriptText =
			u8R"(
	
			-- Create collection using an initialization list
			set a to... 

				3, 

				2, 

				1
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 3);
		REQUIRE(collection->find(1) != collection->end());
		REQUIRE(collection->find(1)->second == 3);
		REQUIRE(collection->find(2) != collection->end());
		REQUIRE(collection->find(2)->second == 2);
		REQUIRE(collection->find(3) != collection->end());
		REQUIRE(collection->find(3)->second == 1);
	}

	SECTION("Test initialization list of pairs")
	{
		static const char * scriptText =
			u8R"(
			
			set a to (1, 2), (3, 4), (5, 6)

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").GetCollection()->at(1).GetCollection()->at(1) == 1);
		REQUIRE(script->GetVariable("a").GetCollection()->at(1).GetCollection()->at(2) == 2);
		REQUIRE(script->GetVariable("a").GetCollection()->at(2).GetCollection()->at(1) == 3);
		REQUIRE(script->GetVariable("a").GetCollection()->at(2).GetCollection()->at(2) == 4);
		REQUIRE(script->GetVariable("a").GetCollection()->at(3).GetCollection()->at(1) == 5);
		REQUIRE(script->GetVariable("a").GetCollection()->at(3).GetCollection()->at(2) == 6);
	}

	SECTION("Test collection addition of elements using assignment")
	{
		static const char * scriptText =
			u8R"(
	
			-- Create empty collection and add three elements
			set a to []
			set a [1] to 3
			set a [2] to 2
			set a [3] to 1
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 3);
		REQUIRE(collection->find(1) != collection->end());
		REQUIRE(collection->find(1)->second == 3);
		REQUIRE(collection->find(2) != collection->end());
		REQUIRE(collection->find(2)->second == 2);
		REQUIRE(collection->find(3) != collection->end());
		REQUIRE(collection->find(3)->second == 1);
	}

	SECTION("Test collection initialization list of key-value pairs")
	{
		static const char * scriptText =
			u8R"(
	
			-- Create collection using an initialization list of key-value pairs		
			set a to [1, "red"], [2, "green"], [3, "blue"]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 3);
		REQUIRE(collection->find(1) != collection->end());
		REQUIRE(collection->find(1)->second == "red");
		REQUIRE(collection->find(2) != collection->end());
		REQUIRE(collection->find(2)->second == "green");
		REQUIRE(collection->find(3) != collection->end());
		REQUIRE(collection->find(3)->second == "blue");
	}

	SECTION("Test collection initialization list of key-value pairs on new lines")
	{
		static const char * scriptText =
			u8R"(
	
			-- Create collection using an initialization list of key-value pairs		
			set a to...
				[1, "red"], 
				[2, "green"], 
				[3, "blue"]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 3);
		REQUIRE(collection->find(1) != collection->end());
		REQUIRE(collection->find(1)->second == "red");
		REQUIRE(collection->find(2) != collection->end());
		REQUIRE(collection->find(2)->second == "green");
		REQUIRE(collection->find(3) != collection->end());
		REQUIRE(collection->find(3)->second == "blue");
	}

	SECTION("Test collection initialization list of key-value pairs on new lines with spacing")
	{
		static const char * scriptText =
			u8R"(
	
			-- Create collection using an initialization list of key-value pairs		
			set a to...

				[1, "red"], 

				[2, "green"], 

				[3, "blue"]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 3);
		REQUIRE(collection->find(1) != collection->end());
		REQUIRE(collection->find(1)->second == "red");
		REQUIRE(collection->find(2) != collection->end());
		REQUIRE(collection->find(2)->second == "green");
		REQUIRE(collection->find(3) != collection->end());
		REQUIRE(collection->find(3)->second == "blue");
	}

	SECTION("Test assignment of collection element by index operator for variable")
	{
		static const char * scriptText =
			u8R"(
	
			-- Create collection using an initialization list of key-value pairs		
			set a to [1, "red"], [2, "green"], [3, "blue"]
			
			-- Change one of the elements by index
			set a[2] to "magenta"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 3);
		REQUIRE(collection->find(2) != collection->end());
		REQUIRE(collection->find(2)->second == "magenta");
	}

	SECTION("Test assignment of collection element by index operator for property")
	{
		static const char * scriptText =
			u8R"(
	
			-- Create collection using an initialization list of key-value pairs		
			set private a to [1, "red"], [2, "green"], [3, "blue"]
			
			-- Change one of the elements by index
			set a[2] to "magenta"

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
		REQUIRE(collection->find(2)->second == "magenta");
	}

	SECTION("Test assignment of collection variable by key")
	{
		static const char * scriptText =
			u8R"(
	
			-- Create collection using an initialization list of key-value pairs		
			set a to [1, "red"], [2, "green"], [3, "blue"]
			
			-- Set variable to one of the collection values
			set b to a[2]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("b") == "green");
	}

	SECTION("Test assignment of collection property by key")
	{
		static const char * scriptText =
			u8R"(
	
			-- Create collection using an initialization list of key-value pairs		
			set private a to [1, "red"], [2, "green"], [3, "blue"]
			
			-- Set variable to one of the collection values
			set b to a[2]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("b") == "green");
	}

	SECTION("Test adding auto-indexed value to existing collection")
	{
		static const char * scriptText =
			u8R"(
			import core

			-- Create collection using an initialization list of key-value pairs		
			set a to [1, "red"], [2, "green"], [3, "blue"]
			
			-- Add single element to a
			set a[(a size + 1)] to "purple"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 4);
		REQUIRE(collection->find(4) != collection->end());
		REQUIRE(collection->find(4)->second == "purple");
	}

	SECTION("Test erasing single element from collection by key")
	{
		static const char * scriptText =
			u8R"(
			import core

			-- Create collection using an initialization list of key-value pairs		
			set a to [1, "red"], [2, "green"], [3, "blue"]
			
			-- Erase element by key
			erase a[2] 

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 2);
		REQUIRE(collection->find(2) == collection->end());
	}

	SECTION("Test erasing single element from collection in loop")
	{
		static const char * scriptText =
			u8R"(
			import core

			-- Create collection using an initialization list of key-value pairs		
			set a to [1, "red"], [2, "green"], [3, "blue"]

			loop i over a
				if i value = "blue"
					erase i
				end
			end

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		auto collection = script->GetVariable("a").GetCollection();
		REQUIRE(collection);
		REQUIRE(collection->size() == 2);
		REQUIRE(collection->find(3) == collection->end());
	}

}