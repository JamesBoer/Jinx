/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;


static bool s_functionCalled;

class TestClass
{
public:
	TestClass(int testValue) : m_testValue(testValue) { }

	int GetTestValue() const { return m_testValue; }

private:
	int m_testValue;
};

static TestClass s_testClass(99);

class TestContext
{
public:
	TestContext(Jinx::RuntimePtr runtime)
	{
		const char * scriptText =
			u8R"(
			
			-- Call local override function 
			test user context 9999
			
			)";
		m_script = TestCreateScript(scriptText, runtime, this);
	}

	bool ExecuteScript()
	{
		if (!m_script)
			return false;
		return m_script->Execute();
	}

	void SetValue(int64_t val) { m_testVal = val; }
	int64_t GetTestValue() const { return m_testVal; }

private:

	ScriptPtr m_script;
	int64_t m_testVal = 0;
};

class TestClass2
{
public:
	TestClass2(int, float, std::string)
	{

	}
};


static Variant ThisFunction(ScriptPtr script, Parameters params)
{
	s_functionCalled = true;
	return Variant();
}

static Variant ThatFunction(ScriptPtr script, Parameters params)
{
	return 42;
}

static Variant AnotherFunction(ScriptPtr script, Parameters params)
{
	return "forty two";
}

static Variant YetAnotherFunction(ScriptPtr script, Parameters params)
{
	if (params.size() != 3)
		return Variant("");
	return params[0] + Variant(" ") + params[1] + Variant(" ") + params[2];
}

static Variant MemberFunction(ScriptPtr script, Parameters params)
{
	TestClass * testClass = JinxAnyCast<TestClass *>(script->GetUserContext());
	return testClass->GetTestValue();
}



TEST_CASE("Test Native", "[Native]")
{
	SECTION("Test native callback functions")
	{
		static const char * scriptText =
			u8R"(

			import test			

			this function
			set a to that function
			set b to another function
			set c to yet "one" another "two" function "three"
			set d to member function
			set e to lambda function

			)";

		auto runtime = TestCreateRuntime();
		auto library = runtime->GetLibrary("test");
		library->RegisterFunction(Visibility::Public, {"this function"}, ThisFunction);
		library->RegisterFunction(Visibility::Public, { "that function" }, ThatFunction);
		library->RegisterFunction(Visibility::Public, { "another function" }, AnotherFunction);
		library->RegisterFunction(Visibility::Public, { "yet {} another {} function {}"}, YetAnotherFunction);
		library->RegisterFunction(Visibility::Public, { "member function" }, MemberFunction);
		library->RegisterFunction(Visibility::Public, { "lambda function" }, [](ScriptPtr script, Parameters params)->Variant
		{
			return "lambda lambda lambda";
		});

		auto script = TestExecuteScript(scriptText, runtime, &s_testClass);
		REQUIRE(script);
		REQUIRE(s_functionCalled == true);
		REQUIRE(script->GetVariable("a") == 42);
		REQUIRE(script->GetVariable("b") == "forty two");
		REQUIRE(script->GetVariable("c") == "one two three");
		REQUIRE(script->GetVariable("d") == 99);
		REQUIRE(script->GetVariable("e") == "lambda lambda lambda");
	}

	SECTION("Test script user context data")
	{
		auto runtime = TestCreateRuntime();
		runtime->GetLibrary("")->RegisterFunction(Visibility::Private, { "test user context {integer}" }, [](ScriptPtr script, Parameters params) -> Variant
		{
			auto classPtr = JinxAnyCast<TestContext *>(script->GetUserContext());
			classPtr->SetValue(params[0].GetInteger());
			return nullptr;
		});

		TestContext obj(runtime);
		REQUIRE(obj.ExecuteScript());
		REQUIRE(obj.GetTestValue() == 9999);
	}

	SECTION("Test native function execution")
	{
		static const char * scriptText =
			u8R"(
	
			public function getvalue
				return 123
			end			
			
			)";

		auto runtime = TestCreateRuntime();
		auto script = TestExecuteScript(scriptText, runtime);
		REQUIRE(script);
	}

	SECTION("Test native properties")
	{
		static const char * scriptText =
			u8R"(
			import test
					
			set a to someprop
			
			)";

		auto runtime = TestCreateRuntime();
		auto library = runtime->GetLibrary("test");
		library->RegisterProperty(Visibility::Public, Access::ReadWrite, "someprop", 42);
		auto script = TestExecuteScript(scriptText, runtime);
		REQUIRE(script);
		REQUIRE(script->GetVariable("a") == 42);
	}

	SECTION("Test native variant Guid-String conversions")
	{
		static const char * guidStr = "06DF8818-07DB-4AAB-9BF6-3365D0F2D4C9";
		Variant gs = guidStr;	
		Variant gv = gs.GetGuid();
		Variant gs2 = gv.GetString();
		REQUIRE(gs == gs2);
	}

	SECTION("Test multiple executions of a script")
	{
		static const char * scriptText =
			u8R"(
					
			set a to 123
			
			)";

		auto runtime = TestCreateRuntime();
		auto script = runtime->CreateScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->Execute());
		REQUIRE(script->IsFinished());
		REQUIRE(script->GetVariable("a") == 123);
		script->SetVariable("a", 1);
		REQUIRE(script->Execute());
		REQUIRE(script->IsFinished());
		REQUIRE(script->GetVariable("a") == 123);
	}

	SECTION("Test Jinx function execution from C++")
	{
		const char * scriptText =
			u8R"(

				private function {a} minus {b}
					return a - b
				end

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		auto id = script->FindFunction(nullptr, { "{} minus {}" });
		REQUIRE(id != InvalidID);
		auto val = script->CallFunction(id, { 5, 2 });
		REQUIRE(val == 3);
	}

	SECTION("Test native function execution from C++")
	{
		auto runtime = TestCreateRuntime();
		auto library = runtime->GetLibrary("test");
		library->RegisterFunction(Visibility::Public, { "native call" }, [](ScriptPtr script, Parameters params)->Variant
		{
			return "Mary had a little lambda";
		});

		auto script = TestExecuteScript("", runtime);
		REQUIRE(script);
		auto id = script->FindFunction(library, { "native call" });
		REQUIRE(id != InvalidID);
		auto val = script->CallFunction(id, {});
		REQUIRE(val == "Mary had a little lambda");
	}

	SECTION("Test native function with concurrent execution from C++")
	{
		const char * scriptText =
			u8R"(

				private function {a} minus {b}
					return a - b
				end

				set x to 0
				loop from 1 to 3
					increment x
					wait
				end

			)";

		auto script = TestCreateScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->Execute());
		auto id = script->FindFunction(nullptr, { "{} minus {}" });
		REQUIRE(id != InvalidID);
		while (!script->IsFinished())
		{
			auto val = script->CallFunction(id, { 5, 2 });
			REQUIRE(val == 3);
			REQUIRE(script->Execute());
		}
		REQUIRE(script->GetVariable("x") == 3);
	}

	SECTION("Test native allocation / deallocation")
	{
		void * p = Jinx::MemAllocate(64);
		memset(p, 7, 64);
		Jinx::MemFree(p);
	}

	SECTION("Test native realloc")
	{
		void * p1 = Jinx::MemAllocate(64);
		memset(p1, 7, 64);
		void * p2 = Jinx::MemAllocate(64);
		memset(p2, 7, 64);
		REQUIRE(memcmp(p1, p2, 64) == 0);
		p2 = Jinx::MemReallocate(p2, 128);
		REQUIRE(memcmp(p1, p2, 64) == 0);
		Jinx::MemFree(p1);
		Jinx::MemFree(p2);
	}

}