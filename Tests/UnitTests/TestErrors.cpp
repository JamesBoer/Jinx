/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


TEST_CASE("Test Syntax, Parsing, and Runtime Errors", "[Errors]")
{
	SECTION("Test missing comment block error")
	{
		static const char * scriptText =
			u8R"(
	
			---
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test missing ellipse")
	{
		static const char * scriptText =
			u8R"(
	
			set a to ..
            1
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test invalid name")
	{
		static const char * scriptText =
			u8R"(
	
			set 1a to 1
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test number parsing #1 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 34.56.78
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test number parsing #2 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 34r78
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test invalid ! symbol")
	{
		static const char * scriptText =
			u8R"(
	
			!
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test invalid name start")
	{
		static const char * scriptText =
			u8R"(
	
			set ' to 1
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test invalid name")
	{
		static const char * scriptText =
			u8"set x\b to 1";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test invalid numeric parse end of line")
	{
		static const char * scriptText =
			u8R"(
	
			.
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test invalid numeric parse end of file")
	{
		static const char * scriptText =
			u8R"(
	
			.)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test unassigned variable error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 3 + x
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test mismatched quote error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to "Invalid string
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test too many operators #1 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 3 + * 4
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}
	
	SECTION("Test too many operators #2 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to (3 +) * 4
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}
	
	SECTION("Test too many operands #1 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 3 4

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test too many operands #2 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 3 (4)

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test too many operands #3 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to (((3)) 4)

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test too many operands #4 error")
	{
		static const char * scriptText =
			u8R"(
			
			function f
				return 23
			end
	
			set a to 3 f

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test too many operands #5 error")
	{
		static const char * scriptText =
			u8R"(
			
			function f
				return 23
			end
	
			set a to 3(f)

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test too few operands #1 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 3 +

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}
	
	SECTION("Test too few operands #2 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 3 * (4 / )

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test logic operator #1 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to true and

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test logic operator #2 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to true or

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}


	SECTION("Test logic operator #3 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to and true

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test logic operator #4 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to or true

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test logic operator #5 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to not and

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test logic operator #6 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to not or

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test divide by zero error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 1 / 0

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
		REQUIRE(!script->Execute());
	}

	SECTION("Test mod by zero error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 1 % 0

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
		REQUIRE(!script->Execute());
	}

	SECTION("Test invalid comparison operands #1 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 1 < "cow"

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
		REQUIRE(!script->Execute());
	}

	SECTION("Test invalid comparison operands #2 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to null >= null

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
		REQUIRE(!script->Execute());
	}

	SECTION("Test invalid comparison operands #3 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to "lemur" > false

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
		REQUIRE(!script->Execute());
	}

	SECTION("Test invalid increment #1 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to "grasshopper"
			increment a

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
		REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test invalid increment #2 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 12
			increment a by "weasel"

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test invalid increment #3 error")
	{
		static const char * scriptText =
			u8R"(
	
			increment 3

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test invalid decrement #1 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to "grasshopper"
			decrement a

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test invalid decrement #2 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 12
			decrement a by "weasel"

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test invalid decrement #3 error")
	{
		static const char * scriptText =
			u8R"(
	
			decrement 3

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test variable access scope error")
	{
		static const char * scriptText =
			u8R"(
	
			begin
				set a to 123
			end
	
			set b to a
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test function declaration scope error")
	{
		static const char * scriptText =
			u8R"(
	
			begin
				function somefunction
				end
			end
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test function declaration execution frame error")
	{
		static const char * scriptText =
			u8R"(
	
			function somefunction
				function someotherfunction
				end
			end
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test local function declaration duplicate error")
	{
		static const char * scriptText =
			u8R"(
	
			function somefunction
			end

			function somefunction
			end
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

    SECTION("Test non-local function declaration duplicate error")
    {
        static const char * scriptText =
                u8R"(

			public function somefunction
			end

			public function somefunction
			end

			)";

        auto script = TestCreateScript(scriptText);
        REQUIRE(!script);
    }

    SECTION("Test multi-script function declaration duplicate error")
	{
		static const char * scriptText1 =
			u8R"(
	
			private function collisiontest
				return 123
			end

			set a to collisiontest

			)";

		static const char * scriptText2 =
			u8R"(
	
			private function collisiontest
				return 456
			end

			set a to collisiontest

			)";

		auto runtime = TestCreateRuntime();
		auto script1 = TestCreateScript(scriptText1, runtime);
		auto script2 = TestCreateScript(scriptText2, runtime);
		REQUIRE(script1);
		REQUIRE(!script2);
	}

	SECTION("Test function declaration keyword match error")
	{
		static const char * scriptText =
			u8R"(
	
			function while
			end
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test function declaration optional name part error #1")
	{
		static const char * scriptText =
			u8R"(

			function (test) {x} test {y} (test)
			end

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test function declaration optional name part error #2")
	{
		static const char * scriptText =
			u8R"(

			function test {x} (test) {y} (test)
			end

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test function declaration optional name part error #3")
	{
		static const char * scriptText =
			u8R"(

			function test {x} (test) (test) {y} (test)
			end

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test function subscript operator error")
	{
		static const char * scriptText =
			u8R"(
	
			function yadda
				return 1
			end

			set a to yadda [1]
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test import error #1")
	{
		static const char * scriptText =
			u8R"(
	
			import junk
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test import error #2")
	{
		static const char * scriptText =
			u8R"(
	
			import 123
			
			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test library property scope error #1")
	{
		static const char * scriptText1 =
			u8R"(
	
			library test

			set private x to 5
			
			)";

		static const char * scriptText2 =
			u8R"(
	
			import test
	 
			set b to x		

			)";

		auto runtime = TestCreateRuntime();
		auto script1 = TestExecuteScript(scriptText1, runtime);
		auto script2 = TestExecuteScript(scriptText2, runtime);
		REQUIRE(script1);
		REQUIRE(!script2);
	}

	SECTION("Test library property scope error #2")
	{
		static const char * scriptText1 =
			u8R"(
	
			library test

			set private x to 5
			
			)";

		static const char * scriptText2 =
			u8R"(
	
			import test
	 
			set b to test x		

			)";

		auto runtime = TestCreateRuntime();
		auto script1 = TestExecuteScript(scriptText1, runtime);
		auto script2 = TestExecuteScript(scriptText2, runtime);
		REQUIRE(script1);
		REQUIRE(!script2);
	}

	SECTION("Test library function scope error")
	{
		static const char * scriptText1 =
			u8R"(
	
			library test

			private function func
			end
			
			)";

		static const char * scriptText2 =
			u8R"(
	
			import test
	 
			func	

			)";

		auto runtime = TestCreateRuntime();
		auto script1 = TestExecuteScript(scriptText1, runtime);
		auto script2 = TestExecuteScript(scriptText2, runtime);
		REQUIRE(script1);
		REQUIRE(!script2);
	}

	SECTION("Test ambiguous function name")
	{
		static const char * scriptText1 =
			u8R"(
	
			library test1

			public function func
            end
			
			)";

		static const char * scriptText2 =
			u8R"(
	
			library test2

			public function func
            end
			
			)";

		static const char * scriptText3 =
			u8R"(
	
			import test1
			import test2
	 
			func

			)";

		auto runtime = TestCreateRuntime();
		auto script1 = TestExecuteScript(scriptText1, runtime);
		auto script2 = TestExecuteScript(scriptText2, runtime);
		auto script3 = TestExecuteScript(scriptText3, runtime);
		REQUIRE(script1);
		REQUIRE(script2);
	}

	SECTION("Test duplicate property error #1")
	{
		static const char * scriptText =
			u8R"(
			
			set private a to 123
			set public a to 345
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test duplicate property error #2")
	{
		static const char * scriptText =
			u8R"(

			set private a a to 123
			set public a a to 345
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test property duplicates")
	{
		static const char * scriptText =
			u8R"(
	
			set private a to 123
			set private a to 234
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test property unexpected end of file")
	{
		static const char * scriptText =
			u8R"(
	
			set private
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test unassigned readonly property")
	{
		static const char * scriptText =
			u8R"(
	
			set public readonly x
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test property name collision with function parameter name")
	{
		static const char * scriptText =
			u8R"(
	
			set private a to 123

			function test {a}
			return
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test property readonly syntax order")
	{
		static const char * scriptText =
			u8R"(
	
			set readonly private a to 123
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test property readonly attribute")
	{
		static const char * scriptText =
			u8R"(
	
			set private readonly a to 123
			set a to 456
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test property readonly attribute #1")
	{
		static const char * scriptText1 =
			u8R"(
	
			library test

			set public readonly prop to 333

			)";

		static const char * scriptText2 =
			u8R"(
	
			import test
	 
			set prop to 12345

			)";

		auto runtime1 = TestCreateRuntime();
		auto scriptBytecode1 = runtime1->Compile(scriptText1);
		REQUIRE(scriptBytecode1);

		auto runtime2 = TestCreateRuntime();
		auto script1 = runtime2->CreateScript(scriptBytecode1);
        REQUIRE(TestExecuteScript(script1));
		auto script2 = TestExecuteScript(scriptText2, runtime2);
		REQUIRE(script1);
		REQUIRE(!script2);
	}

	SECTION("Test property readonly attribute #2")
	{
		static const char * scriptText1 =
			u8R"(
	
			library test

			set public readonly prop to 333

			)";

		static const char * scriptText2 =
			u8R"(
	
			import test
	 
			decrement prop

			)";

		auto runtime1 = TestCreateRuntime();
		auto scriptBytecode1 = runtime1->Compile(scriptText1);
		REQUIRE(scriptBytecode1);

		auto runtime2 = TestCreateRuntime();
		auto script1 = runtime2->CreateScript(scriptBytecode1);
        REQUIRE(TestExecuteScript(script1));
		auto script2 = TestExecuteScript(scriptText2, runtime2);
		REQUIRE(script1);
		REQUIRE(!script2);
	}

	SECTION("Test collection initialization list error #1")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 3, 2, 1,
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test collection initialization list error #2")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 3, 2 1
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test collection initialization list of key-value pairs error #1")
	{
		static const char * scriptText =
			u8R"(
	
			-- Missing bracket
			set a to [1, "red"], [2, "green", [3, "blue"]

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test collection initialization list of key-value pairs error #2")
	{
		static const char * scriptText =
			u8R"(
	
			-- Missing key
			set a to [1, "red"], [2, "green"], ["blue"]

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test collection initialization list of key-value pairs error #3")
	{
		static const char * scriptText =
			u8R"(
	
			-- Missing comma
			set a to [1, "red"], [2, "green"] [3, "blue"]

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test external variable with variable name collision")
	{
		const char * scriptText =
			u8R"(
			
			set some var to 345
			external some var
			set another var to some var

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test external variable with property name collision")
	{
		const char * scriptText =
			u8R"(
			
			set private some var to 345
			external some var
			set another var to some var

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test external variable scope")
	{
		const char * scriptText =
			u8R"(
			
			begin
				external some var
			end

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test missing block after if")
	{
		const char * scriptText =
			u8R"(
			
			if 1 < 2

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test missing block after else")
	{
		const char * scriptText =
			u8R"(
			
			if 1 < 2
                set a to 1
			else

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test external variable frame")
	{
		const char * scriptText =
			u8R"(
			
			function something
				external some var
			end

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test invalid addition")
	{
		const char * scriptText =
			u8R"(
		
			set x to 3
			set y to "3"
			set z to x + y
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test invalid subtraction")
	{
		const char * scriptText =
			u8R"(
		
			set x to 3
			set y to "3"
			set z to x - y
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test invalid multiplication")
	{
		const char * scriptText =
			u8R"(
		
			set x to 3
			set y to "3"
			set z to x * y
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test invalid division")
	{
		const char * scriptText =
			u8R"(
		
			set x to 3
			set y to "3"
			set z to x / y
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test invalid mod")
	{
		const char * scriptText =
			u8R"(
		
			set x to 3
			set y to "3"
			set z to x % y
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test invalid function parameter cast")
	{
		const char * scriptText =
			u8R"(
			
			function convert test {integer x}
			end

			convert test "string"
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test missing function name")
	{
		const char * scriptText =
			u8R"(
			
			function
			end
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test function consecutive params")
	{
		const char * scriptText =
			u8R"(
			
			function test {x} {y}
				return x + y
			end
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test function missing param name")
	{
		const char * scriptText =
			u8R"(
			
			function test {}
			end
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test invalid function name")
	{
		const char * scriptText =
			u8R"(
			
			function "test" {x}
			end
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test invalid function name in alternative")
	{
		const char * scriptText =
			u8R"(
			
			function test {x} alternative/"alternatives"
			end
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test duplicate alternative function name part")
	{
		const char * scriptText =
			u8R"(
			
			function test {x} alternative/alternative
			end
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test missing ending param for optional function name part")
	{
		const char * scriptText =
			u8R"(
			
			function test {x} (optional
			end
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test required minimum one non-optional name part")
	{
		const char * scriptText =
			u8R"(
			
			function (test) (optional)
			end
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test name collision in counting loop variable")
	{
		const char * scriptText =
			u8R"(
			
			set i to 100
			loop i from 1 to 10
			end
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test name collision in nexted counting loop variable")
	{
		const char * scriptText =
			u8R"(
			
			loop i from 1 to 10
				loop i from 1 to 10
				end
			end
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test name collision in collection iteration variable")
	{
		const char * scriptText =
			u8R"(
			
			set list to "apple", "orange", "banana"
			set i to null
			loop i over list
			end
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test out of bounds string index var set low")
	{
		const char * scriptText =
			u8R"(
			
			set a to "Hello world!"
			set b to a[0]
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test out of bounds string index var set high")
	{
		const char * scriptText =
			u8R"(
			
			set a to "Hello world!"
			set b to a[13]
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test out of bounds string index var set low")
	{
		const char * scriptText =
			u8R"(
			
			set a to "Hello world!"
			set b to a[0]
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test out of bounds string index range var set low")
	{
		const char * scriptText =
			u8R"(
			
			set a to "Hello world!"
			set b to a[0, 5]
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test out of bounds string index var set high")
	{
		const char * scriptText =
			u8R"(
			
			set a to "Hello world!"
			set b to a[7, 14]
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test reversed range indeces var")
	{
		const char * scriptText =
			u8R"(
			
			set a to "Hello world!"
			set b to a[5, 3]
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test out of bounds string index prop set low")
	{
		const char * scriptText =
			u8R"(
			
			set public a to "Hello world!"
			set b to a[0]
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test out of bounds string index prop set high")
	{
		const char * scriptText =
			u8R"(
			
			set public a to "Hello world!"
			set b to a[13]
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test out of bounds string index var get low")
	{
		const char * scriptText =
			u8R"(
			
			set a to "hello world!"
			set a[0] to "H"
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test out of bounds string index var get high")
	{
		const char * scriptText =
			u8R"(
			
			set a to "hello world!"
			set a[13] to "!"
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test out of bounds string index pair var get low")
	{
		const char * scriptText =
			u8R"(
			
			set a to "hello world!"
			set a[0, 5] to "Howdy"
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test out of bounds string index pair var get high")
	{
		const char * scriptText =
			u8R"(
			
			set a to "hello world!"
			set a[7, 13] to "globe"
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test out of bounds string index pair var reversed")
	{
		const char * scriptText =
			u8R"(
			
			set a to "hello world!"
			set a[6, 3] to "!"
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test out of bounds string index prop get low")
	{
		const char * scriptText =
			u8R"(
			
			set public a to "hello world!"
			set a[0] to "H"
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test out of bounds string index prop get high")
	{
		const char * scriptText =
			u8R"(
			
			set public a to "hello world!"
			set a[13] to "!"
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test '{} (get) key' with invalid parameter")
	{
		static const char * scriptText =
			u8R"(

			import core

			set a to 1
			set b to a's key

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test '{} (get) value' with invalid parameter")
	{
		static const char * scriptText =
			u8R"(

			import core

			set a to 1
			set b to a's value

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test misnamed function declaration")
	{
		static const char * scriptText =
			u8R"(

			function test {x}
				return x
			end

			set f to function test_error {}

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}

	SECTION("Test 'call' with invalid parameter")
	{
		static const char * scriptText =
			u8R"(

			import core

			function test
			end

			set f to 0
			call f
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

	SECTION("Test 'call with' with invalid parameter")
	{
		static const char * scriptText =
			u8R"(

			import core

			function test {x}
			end

			set f to 0
			call f with 3
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
        REQUIRE(!TestExecuteScript(script));
	}

}
