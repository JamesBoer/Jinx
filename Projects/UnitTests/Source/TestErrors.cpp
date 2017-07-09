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
	SECTION("Test number parsing error error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to 34.56.78
			
			)";

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
	
			set a to 3 + - 4
			
			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(!script);
	}
	
	SECTION("Test too many operators #2 error")
	{
		static const char * scriptText =
			u8R"(
	
			set a to (3 +) - 4
			
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

	SECTION("Test function declaration duplicate error")
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

	SECTION("Test function declaration keyword match error")
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

	SECTION("Test duplicate property error #1")
	{
		static const char * scriptText =
			u8R"(
			
			import test

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
			
			import test

			set private a a to 123
			set public a a to 345
			
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
		script1->Execute();
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
		script1->Execute();
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
		REQUIRE(!script->Execute());
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
		REQUIRE(!script->Execute());
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
		REQUIRE(!script->Execute());
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
		REQUIRE(!script->Execute());
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
		REQUIRE(!script->Execute());
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
		REQUIRE(!script->Execute());
	}

}
