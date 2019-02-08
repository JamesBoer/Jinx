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

	SECTION("Test assignment from collection property by key")
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

	SECTION("Test collections in collections assignment")
	{
		static const char * scriptText =
			u8R"(

			set a to []
			set a[1] to []
			set a[1][1] to []
			set b to a[1][1]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a").IsCollection());
		REQUIRE(script->GetVariable("a").GetCollection()->at(1).IsCollection());
		REQUIRE(script->GetVariable("b").IsCollection());
	}

	SECTION("Test nested collection declaration")
	{
		static const char * scriptText =
			u8R"(

				set t to ["one", ["two", 2], ["three", 3], ["four", 4]]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->Execute());
		REQUIRE(script->GetVariable("t").IsCollection());
		auto t = script->GetVariable("t").GetCollection();
		auto t1 = t->at("one");
		auto t2 = t1.GetCollection()->at("two");
		REQUIRE(t2 == 2);
	}

	SECTION("Test nested collection access reads")
	{
		static const char * scriptText =
			u8R"(

				set t to ["one", ["two", 2], ["three", 3], ["four", 4]]
				set a to t["one"]["two"]

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->Execute());
		REQUIRE(script->GetVariable("a") == 2);
	}

	SECTION("Test deeply nested collection access reads on vars")
	{
		static const char * scriptText =
			u8R"(

				set t to ["one", ["two", ["three", ["four", ["five", 5]]]]]
				set a to t["one"]["two"]["three"]["four"]["five"]
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script->Execute());
		REQUIRE(script->GetVariable("a") == 5);
	}

    SECTION("Test deeply nested collection access reads on properties")
    {
        static const char * scriptText =
            u8R"(

				set private t to ["one", ["two", ["three", ["four", ["five", 5]]]]]
				set private a to t["one"]["two"]["three"]["four"]["five"]
			)";

        auto script = TestExecuteScript(scriptText);
        REQUIRE(script->Execute());
        REQUIRE(script->GetLibrary()->GetProperty("a") == 5);
    }

    SECTION("Test mixed nested collection var assignment with existing and non-existing keys")
    {
        static const char * scriptText =
            u8R"(

				set a to []
				set a["one"] to 2

				set b to []
				set b["one"]["two"] to 3

				set c to ["one", []]
				set c["one"]["two"]["three"] to 4

			)";

        auto script = TestExecuteScript(scriptText);
        REQUIRE(script);
        REQUIRE(script->GetVariable("a").IsCollection());
        REQUIRE(script->GetVariable("a").GetCollection()->at("one") == 2);
        REQUIRE(script->GetVariable("b").IsCollection());
        REQUIRE(script->GetVariable("b").GetCollection()->at("one").GetCollection()->at("two") == 3);
        REQUIRE(script->GetVariable("c").IsCollection());
        REQUIRE(script->GetVariable("c").GetCollection()->at("one").GetCollection()->at("two").GetCollection()->at("three") == 4);
    }

    SECTION("Test mixed nested collection property assignment with existing and non-existing keys")
    {
        static const char * scriptText =
            u8R"(

				set private a to []
				set a["one"] to 2

				set private b to []
				set b["one"]["two"] to 3

				set private c to ["one", []]
				set c["one"]["two"]["three"] to 4

			)";

        auto script = TestExecuteScript(scriptText);
        REQUIRE(script);
        REQUIRE(script->GetLibrary()->GetProperty("a").IsCollection());
        REQUIRE(script->GetLibrary()->GetProperty("a").GetCollection()->at("one") == 2);
        REQUIRE(script->GetLibrary()->GetProperty("b").IsCollection());
        REQUIRE(script->GetLibrary()->GetProperty("b").GetCollection()->at("one").GetCollection()->at("two") == 3);
        REQUIRE(script->GetLibrary()->GetProperty("c").IsCollection());
        REQUIRE(script->GetLibrary()->GetProperty("c").GetCollection()->at("one").GetCollection()->at("two").GetCollection()->at("three") == 4);
    }

	SECTION("Test comma-delimited table string conversion to collection")
	{
		static const char * tableText =
			u8R"(
Name Field,Integer Field,Float Field,Text Field
Test Name A,1,4.5,This is a simple test.
Test Name B,2,123.456,More to test…
Test Name C,3,22.3345,Even more tests of text
Still Another Test Name,4,1.5,Still more text
Yet Another Test Name,5,99.99,Yet more text to test
)";

		static const char * scriptText =
			u8R"(

				external text
				
				set table to text as collection

				set a to table["Test Name A"]["Name Field"]
				set b to table["Test Name B"]["Integer Field"]
				set c to table["Test Name C"]["Float Field"]
				set d to table["Still Another Test Name"]["Text Field"]
				set e to table["Yet Another Test Name"]["Text Field"]
			)";

		auto script = TestCreateScript(scriptText);
		script->SetVariable("text", tableText);
		REQUIRE(script->Execute());
		REQUIRE(script->GetVariable("table").IsCollection());
		REQUIRE(script->GetVariable("a") == "Test Name A");
		REQUIRE(script->GetVariable("b") == 2);
		REQUIRE(script->GetVariable("c").GetNumber() == Approx(22.3345));
		REQUIRE(script->GetVariable("d") == "Still more text");
		REQUIRE(script->GetVariable("e") == "Yet more text to test");
	}

	SECTION("Test tab-delimited table string conversion to collection")
	{
		static const char * tableText =
			u8R"(
Name Field	Integer Field	Float Field	Text Field
Test Name A	1	4.5	This is a simple test.
Test Name B	2	123.456	More to test…
Test Name C	3	22.3345	Even more tests of text
Still Another Test Name	4	1.5	Still more text
Yet Another Test Name	5	99.99	Yet more text to test
)";

		static const char * scriptText =
			u8R"(

				external table
				
				set a to table["Test Name A"]["Name Field"]
				set b to table["Test Name B"]["Integer Field"]
				set c to table["Test Name C"]["Float Field"]
				set d to table["Still Another Test Name"]["Text Field"]
				set e to table["Yet Another Test Name"]["Text Field"]
			)";

		auto script = TestCreateScript(scriptText);
		Variant table = tableText;
		REQUIRE(table.ConvertTo(ValueType::Collection));
		script->SetVariable("table", tableText);
		REQUIRE(script->Execute());
		REQUIRE(script->GetVariable("table").IsCollection());
		REQUIRE(script->GetVariable("a") == "Test Name A");
		REQUIRE(script->GetVariable("b") == 2);
		REQUIRE(script->GetVariable("c").GetNumber() == Approx(22.3345));
		REQUIRE(script->GetVariable("d") == "Still more text");
		REQUIRE(script->GetVariable("e") == "Yet more text to test");
	}

}