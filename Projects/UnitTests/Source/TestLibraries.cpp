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
			a is 3 times 4
			
			)";

		static const char * scriptText2 =
			u8R"(
    
			library test
     
			-- Test private functions
			a is 4 minus 3

			-- Test public function
			b is 2 plus 2		

			)";

		static const char * scriptText3 =
			u8R"(
    
			import test
     
			-- Test public function
			a is 5 plus 1
			
			)";


		auto runtime = TestCreateRuntime();
		auto script1 = TestExecuteScript(scriptText1, runtime);
		auto script2 = TestExecuteScript(scriptText2, runtime);
		auto script3 = TestExecuteScript(scriptText3, runtime);
		REQUIRE(script1);
		REQUIRE(script2);
		REQUIRE(script3);
		REQUIRE(script1->GetVariable("a").GetInteger() == 12);
		REQUIRE(script2->GetVariable("a").GetInteger() == 1);
		REQUIRE(script2->GetVariable("b").GetInteger() == 4);
		REQUIRE(script3->GetVariable("a").GetInteger() == 6);
	}

	SECTION("Test basic library properties")
	{
		static const char * scriptText1 =
			u8R"(
    
			library test

			public x is 5
			public x x is 5

			a is x
			a a is x x
			
			)";

		static const char * scriptText2 =
			u8R"(
    
			library test
     
			public y is 2
			public y y is 2

			b is x		
			b b is x x	

			)";

		static const char * scriptText3 =
			u8R"(
    
			import test
     
			c is y
			c c is y y
			d is test y
			d d is test y y

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
		REQUIRE(script1->GetVariable("a").GetInteger() == 5);
		REQUIRE(script1->GetVariable("a a").GetInteger() == 5);
		REQUIRE(script2->GetVariable("b").GetInteger() == 5);
		REQUIRE(script2->GetVariable("b b").GetInteger() == 5);
		REQUIRE(script3->GetVariable("c").GetInteger() == 2);
		REQUIRE(script3->GetVariable("c c").GetInteger() == 2);
		REQUIRE(script3->GetVariable("d").GetInteger() == 2);
		REQUIRE(script3->GetVariable("d d").GetInteger() == 2);
		auto library = runtime->GetLibrary("test");
		REQUIRE(library->GetProperty("x").GetInteger() == 5);
		library->SetProperty("x", "str");
		library->SetProperty("x x", "str");
		REQUIRE(library->GetProperty("x").GetString() == "str");
		REQUIRE(library->GetProperty("x x").GetString() == "str");
		REQUIRE(library->GetProperty("y").GetInteger() == 4);
		REQUIRE(library->GetProperty("y y").GetInteger() == 4);
	}

	SECTION("Test library property from bytecode")
	{
		static const char * scriptText1 =
			u8R"(
    
			library test

			public prop is 333
			readonly public prop2 is 444			

			)";

		static const char * scriptText2 =
			u8R"(
    
			import test
     
			a is prop
			b is prop2

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
		REQUIRE(script2->GetVariable("a").GetInteger() == 333);
		REQUIRE(script2->GetVariable("b").GetInteger() == 444);
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
     
			a is 5 plus 1
			
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
		REQUIRE(script2->GetVariable("a").GetInteger() == 6);
	}

}