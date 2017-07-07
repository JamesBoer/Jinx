/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2016 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


TEST_CASE("Test Library Functionality", "[Libraries]")
{
	SECTION("Test library functions")
	{
		static const char * scriptText1 =
			u8R"(
	
			library test

			public function return {a} plus {b}
				return a + b
			end

			private function return {a} minus {b}  
				return a - b
			end

			function return {a} times {b}
				return a * b
			end

			-- Test local function
			set a to 3 times 4
			
			)";

		static const char * scriptText2 =
			u8R"(
	
			library test
	 
			-- Test private functions
			set a to 4 minus 3

			-- Test public function
			set b to 2 plus 2		

			)";

		static const char * scriptText3 =
			u8R"(
	
			import test
	 
			-- Test public function
			set a to 5 plus 1
			
			)";


		auto runtime = TestCreateRuntime();
		auto script1 = TestExecuteScript(scriptText1, runtime);
		auto script2 = TestExecuteScript(scriptText2, runtime);
		auto script3 = TestExecuteScript(scriptText3, runtime);
		REQUIRE(script1);
		REQUIRE(script2);
		REQUIRE(script3);
		REQUIRE(script1->GetVariable("a") == 12);
		REQUIRE(script2->GetVariable("a") == 1);
		REQUIRE(script2->GetVariable("b") == 4);
		REQUIRE(script3->GetVariable("a") == 6);
	}

	SECTION("Test basic library properties")
	{
		static const char * scriptText1 =
			u8R"(
	
			library test

			set public x to 5
			set public x x to 5

			set a to x
			set a a to x x
			
			)";

		static const char * scriptText2 =
			u8R"(
	
			library test
	 
			set public y to 2
			set public y y to 2

			set b to x		
			set b b to x x	

			)";

		static const char * scriptText3 =
			u8R"(
	
			import test
	 
			set c to y
			set c c to y y
			set d to test y
			set d d to test y y

			)";

		static const char * scriptText4 =
			u8R"(
	
			import test
	 
			increment test y by 2
			increment test y y by 2
			
			)";

		auto runtime = TestCreateRuntime();
		auto script1 = TestExecuteScript(scriptText1, runtime);
		auto script2 = TestExecuteScript(scriptText2, runtime);
		auto script3 = TestExecuteScript(scriptText3, runtime);
		auto script4 = TestExecuteScript(scriptText4, runtime);
		REQUIRE(script1);
		REQUIRE(script2);
		REQUIRE(script3);
		REQUIRE(script4);
		REQUIRE(script1->GetVariable("a") == 5);
		REQUIRE(script1->GetVariable("a a") == 5);
		REQUIRE(script2->GetVariable("b") == 5);
		REQUIRE(script2->GetVariable("b b") == 5);
		REQUIRE(script3->GetVariable("c") == 2);
		REQUIRE(script3->GetVariable("c c") == 2);
		REQUIRE(script3->GetVariable("d") == 2);
		REQUIRE(script3->GetVariable("d d") == 2);
		auto library = runtime->GetLibrary("test");
		REQUIRE(library->GetProperty("x") == 5);
		library->SetProperty("x", "str");
		library->SetProperty("x x", "str");
		REQUIRE(library->GetProperty("x") == "str");
		REQUIRE(library->GetProperty("x x") == "str");
		REQUIRE(library->GetProperty("y") == 4);
		REQUIRE(library->GetProperty("y y") == 4);
	}

	SECTION("Test library property from bytecode")
	{
		static const char * scriptText1 =
			u8R"(
	
			library test

			set public prop to 333
			set public readonly prop2 to 444			

			)";

		static const char * scriptText2 =
			u8R"(
	
			import test
	 
			set a to prop
			set b to prop2

			)";

		auto runtime1 = TestCreateRuntime();
		auto scriptBytecode1 = runtime1->Compile(scriptText1);
		REQUIRE(scriptBytecode1);

		auto runtime2 = TestCreateRuntime();
		auto script1 = runtime2->CreateScript(scriptBytecode1);
		script1->Execute();
		auto script2 = TestExecuteScript(scriptText2, runtime2);
		REQUIRE(script1);
		REQUIRE(script2);
		REQUIRE(script2->GetVariable("a") == 333);
		REQUIRE(script2->GetVariable("b") == 444);
	}

	SECTION("Test library function from bytecode")
	{
		static const char * scriptText1 =
			u8R"(
	
			library test

			public function return {a} plus {b}
				return a + b
			end
			
			)";

		static const char * scriptText2 =
			u8R"(
	
			import test
	 
			set a to 5 plus 1
			
			)";

		auto runtime1 = TestCreateRuntime();
		auto scriptBytecode1 = runtime1->Compile(scriptText1);
		REQUIRE(scriptBytecode1);

		auto runtime2 = TestCreateRuntime();
		auto script1 = runtime2->CreateScript(scriptBytecode1);
		script1->Execute();
		auto script2 = TestExecuteScript(scriptText2, runtime2);
		REQUIRE(script1);
		REQUIRE(script2);
		REQUIRE(script2->GetVariable("a") == 6);
	}

}