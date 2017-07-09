/*
The Jinx library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Jinx.h for license details.
Copyright (c) 2017 James Boer
*/

#include "UnitTest.h"

using namespace Jinx;

// Disable false positive warning regarding encoding non-ASCII characters in source
#ifdef JINX_WINDOWS
#pragma warning( disable : 4566 )
#endif

TEST_CASE("Test Unicode", "[Unicode]")
{
	SECTION("Test simple Unicode variable names and strings")
	{
		static const char * scriptText =
			u8R"(
	
			set resumÉ to "my resumé text" 	
			set いろは to "いろはにほへとちりぬるをわかよたれそつねならむうゐのおくやまけふこえてあさきゆめみしゑひもせす"
			set Üben to "Falsches Üben von Xylophonmusik quält jeden größeren Zwerg"
			set Да to "В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!"
			set i18n to "Iñtërnâtiônàlizætiøn☃💩"

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable(u8"resumé") == u8"my resumé text");
		REQUIRE(script->GetVariable(u8"いろは") == u8"いろはにほへとちりぬるをわかよたれそつねならむうゐのおくやまけふこえてあさきゆめみしゑひもせす");
		REQUIRE(script->GetVariable(u8"Üben") == u8"Falsches Üben von Xylophonmusik quält jeden größeren Zwerg");
		REQUIRE(script->GetVariable(u8"Да") == u8"В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!");
		REQUIRE(script->GetVariable(u8"i18n") == u8"Iñtërnâtiônàlizætiøn☃💩");

		REQUIRE(script->GetVariable(Str(u"resumé")) == u8"my resumé text");
		REQUIRE(script->GetVariable(Str(u"いろは")) == u8"いろはにほへとちりぬるをわかよたれそつねならむうゐのおくやまけふこえてあさきゆめみしゑひもせす");
		REQUIRE(script->GetVariable(Str(u"Üben")) == u8"Falsches Üben von Xylophonmusik quält jeden größeren Zwerg");
		REQUIRE(script->GetVariable(Str(u"Да")) == u8"В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!");
		REQUIRE(script->GetVariable(Str(u"i18n")) == u8"Iñtërnâtiônàlizætiøn☃💩");

		REQUIRE(script->GetVariable(Str(L"resumé")) == u8"my resumé text");
		REQUIRE(script->GetVariable(Str(L"いろは")) == u8"いろはにほへとちりぬるをわかよたれそつねならむうゐのおくやまけふこえてあさきゆめみしゑひもせす");
		REQUIRE(script->GetVariable(Str(L"Üben")) == u8"Falsches Üben von Xylophonmusik quält jeden größeren Zwerg");
		REQUIRE(script->GetVariable(Str(L"Да")) == u8"В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!");
		REQUIRE(script->GetVariable(Str(L"i18n")) == u8"Iñtërnâtiônàlizætiøn☃💩");
	}

	SECTION("Test variant string Unicode conversion #1")
	{
		auto str8 = String(u8"いろはにほへとちりぬるをわかよたれそつねならむうゐのおくやまけふこえてあさきゆめみしゑひもせす");
		auto str16 = StringU16(u"いろはにほへとちりぬるをわかよたれそつねならむうゐのおくやまけふこえてあさきゆめみしゑひもせす");
		auto wstr = WString(L"いろはにほへとちりぬるをわかよたれそつねならむうゐのおくやまけふこえてあさきゆめみしゑひもせす");

		auto var1 = Variant(str8);
		auto var2 = Variant(str16);
		auto var3 = Variant(wstr);

		auto s1 = var2.GetString();
		auto s2 = var1.GetStringU16();
		auto s3 = var1.GetWString();

		REQUIRE(s1 == str8);
		REQUIRE(s2 == str16);
		REQUIRE(s3 == wstr);
	}

	SECTION("Test variant string Unicode conversion #2")
	{
		auto str8 = String(u8"イロハニホヘト チリヌルヲ ワカヨタレソ ツネナラム ウヰノオクヤマ ケフコエテ アサキユメミシ ヱヒモセスン");
		auto str16 = StringU16(u"イロハニホヘト チリヌルヲ ワカヨタレソ ツネナラム ウヰノオクヤマ ケフコエテ アサキユメミシ ヱヒモセスン");
		auto wstr = WString(L"イロハニホヘト チリヌルヲ ワカヨタレソ ツネナラム ウヰノオクヤマ ケフコエテ アサキユメミシ ヱヒモセスン");

		auto var1 = Variant(str8);
		auto var2 = Variant(str16);
		auto var3 = Variant(wstr);

		auto s1 = var2.GetString();
		auto s2 = var1.GetStringU16();
		auto s3 = var1.GetWString();

		REQUIRE(s1 == str8);
		REQUIRE(s2 == str16);
		REQUIRE(s3 == wstr);
	}

	SECTION("Test variant string Unicode conversion #3")
	{
		auto str8 = String(u8"Quizdeltagerne spiste jordbær med fløde, mens cirkusklovnen Wolther spillede på xylofon.");
		auto str16 = StringU16(u"Quizdeltagerne spiste jordbær med fløde, mens cirkusklovnen Wolther spillede på xylofon.");
		auto wstr = WString(L"Quizdeltagerne spiste jordbær med fløde, mens cirkusklovnen Wolther spillede på xylofon.");

		auto var1 = Variant(str8);
		auto var2 = Variant(str16);
		auto var3 = Variant(wstr);

		auto s1 = var2.GetString();
		auto s2 = var1.GetStringU16();
		auto s3 = var1.GetWString();

		REQUIRE(s1 == str8);
		REQUIRE(s2 == str16);
		REQUIRE(s3 == wstr);
	}

	SECTION("Test variant string Unicode conversion #4")
	{
		auto str8 = String(u8"Falsches Üben von Xylophonmusik quält jeden größeren Zwerg");
		auto str16 = StringU16(u"Falsches Üben von Xylophonmusik quält jeden größeren Zwerg");
		auto wstr = WString(L"Falsches Üben von Xylophonmusik quält jeden größeren Zwerg");

		auto var1 = Variant(str8);
		auto var2 = Variant(str16);
		auto var3 = Variant(wstr);

		auto s1 = var2.GetString();
		auto s2 = var1.GetStringU16();
		auto s3 = var1.GetWString();

		REQUIRE(s1 == str8);
		REQUIRE(s2 == str16);
		REQUIRE(s3 == wstr);
	}

	SECTION("Test variant string Unicode conversion #5")
	{
		auto str8 = String(u8"Γαζέες καὶ μυρτιὲς δὲν θὰ βρῶ πιὰ στὸ χρυσαφὶ ξέφωτο");
		auto str16 = StringU16(u"Γαζέες καὶ μυρτιὲς δὲν θὰ βρῶ πιὰ στὸ χρυσαφὶ ξέφωτο");
		auto wstr = WString(L"Γαζέες καὶ μυρτιὲς δὲν θὰ βρῶ πιὰ στὸ χρυσαφὶ ξέφωτο");

		auto var1 = Variant(str8);
		auto var2 = Variant(str16);
		auto var3 = Variant(wstr);

		auto s1 = var2.GetString();
		auto s2 = var1.GetStringU16();
		auto s3 = var1.GetWString();

		REQUIRE(s1 == str8);
		REQUIRE(s2 == str16);
		REQUIRE(s3 == wstr);
	}

	SECTION("Test variant string Unicode conversion #6")
	{
		auto str8 = String(u8"В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!");
		auto str16 = StringU16(u"В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!");
		auto wstr = WString(L"В чащах юга жил бы цитрус? Да, но фальшивый экземпляр!");

		auto var1 = Variant(str8);
		auto var2 = Variant(str16);
		auto var3 = Variant(wstr);

		auto s1 = var2.GetString();
		auto s2 = var1.GetStringU16();
		auto s3 = var1.GetWString();

		REQUIRE(s1 == str8);
		REQUIRE(s2 == str16);
		REQUIRE(s3 == wstr);
	}

	SECTION("Test simple case folding")
	{
		static const char * scriptText =
			u8R"(
	
			set ABCDEFGHIJKLMNOPQRSTUVWXYZ to 321

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable("abcdefghijklmnopqrstuvwxyz") == 321);
	}

	SECTION("Test complex case folding")
	{
		static const char * scriptText =
			u8R"(
	
			set resumÉ to 321 	
			set légume to 123
			set Üben to 555

			)";

		auto script = TestExecuteScript(scriptText);
		REQUIRE(script);
		REQUIRE(script->GetVariable(u8"resumé") == 321);
		REQUIRE(script->GetVariable(u8"lÉgume") == 123);
		REQUIRE(script->GetVariable(u8"üben") == 555);
	}

}

