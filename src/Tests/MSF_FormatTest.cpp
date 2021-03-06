#include "MSF_UnitTests.h"
#include "MSF_Format.h"
#include "MSF_FormatPrint.h"
#include "MSF_Utilities.h"
#include <stdlib.h>
#include <wchar.h>

DEFINE_TEST_G(BasicPrinting, MSF_Format)
{
	TestFormatResult("%d", "%%d");
	TestFormatResult("%5", "%%%d", 5);

	TestFormatResult("{0}", "{{0}}");
	TestFormatResult("{5}", "{{{0}}}", 5);

	TestFormatResult("{}", "{{}}");
	TestFormatResult("{5}", "{{{}}}", 5);

	TestFormatResult("{%d}", "{{%%d}}");
	TestFormatResult("{0%d}", "{{0%%d}}");

	TestFormatResult("1 2 3 4", "%d %d %d %d", 1, 2, 3, 4);
	TestFormatResult("1 2 3 4", "{0} {1} {2} {3}", 1, 2, 3, 4);
	TestFormatResult("1 2 3 4", "{} {} {} {}", 1, 2, 3, 4);
	TestFormatResult("1 2 3 4", "%d {} %d {}", 1, 2, 3, 4);

	TestFormatResult("1 1 2 2", "{0} {0} {1} {1}", 1, 2);
}

DEFINE_TEST_G(PrintfFormat, MSF_Format)
{
	// Mostly test basic printing, accuracy test flexes the limits of each type
	TestFormatResult("c", "%c", 'c');
	TestFormatResult("c", "%C", 'c');

	TestFormatResult("test", "%s", "test");
	TestFormatResult("test", "%S", "test");

	TestFormatResult("(null)", "%S", (char*)nullptr);

	TestFormatResult("5", "%d", 5);
	TestFormatResult("5", "%i", 5);
	TestFormatResult("5", "%u", 5);
	TestFormatResult("5", "%o", 5);
	TestFormatResult("5", "%x", 5);
	TestFormatResult("5", "%X", 5);

	TestFormatResult("-5", "%d", -5);
	TestFormatResult("-5", "%i", -5);
	TestFormatResult("4294967291", "%u", -5);
	TestFormatResult("37777777773", "%o", -5);
	TestFormatResult("fffffffb", "%x", -5);
	TestFormatResult("FFFFFFFB", "%X", -5);

	TestFormatResult("-5", "%d", (int64_t)-5);
	TestFormatResult("-5", "%i", (int64_t)-5);
	TestFormatResult("18446744073709551611", "%u", (int64_t)-5);
	TestFormatResult("1777777777777777777773", "%o", (int64_t)-5);
	TestFormatResult("fffffffffffffffb", "%x", (int64_t)-5);
	TestFormatResult("FFFFFFFFFFFFFFFB", "%X", (int64_t)-5);

#if defined(_MSC_VER)
#if WIN64
	TestFormatResult("00000000000000FF", "%p", (void*)255);
	TestFormatResult("00000000000000FF", "%P", (void*)255);
#else
	TestFormatResult("000000FF", "%p", (void*)255);
	TestFormatResult("000000FF", "%P", (void*)255);
#endif
#else
	TestFormatResult("0xff", "%p", (void*)255);
	TestFormatResult("0XFF", "%P", (void*)255);
#endif

	TestFormatResult("1.500000", "%f", 1.5f);
	TestFormatResult("1.500000e+00", "%e", 1.5f);
	TestFormatResult("1.500000E+00", "%E", 1.5f);
	TestFormatResult("1.5", "%g", 1.5f);

	TestFormatResult("1.500000", "%f", 1.5);
	TestFormatResult("1.500000e+00", "%e", 1.5);
	TestFormatResult("1.500000E+00", "%E", 1.5);
	TestFormatResult("1.5", "%g", 1.5);
}

DEFINE_TEST_G(CSharpFormat, MSF_Format)
{
	TestFormatResult("c", "{0}", 'c');
	TestFormatResult("c", "{0:C}", 'c');

	TestFormatResult("test", "{0}", "test");
	TestFormatResult("test", "{0:S}", "test");

	TestFormatResult("-5", "{0}", -5);
	TestFormatResult("-5", "{0:d}", -5);
	TestFormatResult("-5", "{0:i}", -5);
	TestFormatResult("4294967291", "{0:u}", -5);
	TestFormatResult("37777777773", "{0:o}", -5);
	TestFormatResult("fffffffb", "{0:x}", -5);
	TestFormatResult("FFFFFFFB", "{0:X}", -5);

	TestFormatResult("-5", "%d", -5LL);
	TestFormatResult("-5", "%i", -5LL);
	TestFormatResult("18446744073709551611", "%u", -5LL);
	TestFormatResult("1777777777777777777773", "%o", -5LL);
	TestFormatResult("fffffffffffffffb", "%x", -5LL);
	TestFormatResult("FFFFFFFFFFFFFFFB", "%X", -5LL);

#if defined(_MSC_VER)
#if WIN64
	TestFormatResult("00000000000000FF", "{0}", (void*)255);
	TestFormatResult("00000000000000FF", "{0:P}", (void*)255);
#else
	TestFormatResult("000000FF", "{0}", (void*)255);
	TestFormatResult("000000FF", "{0:P}", (void*)255);
#endif
#else
	TestFormatResult("0xff", "{0}", (void*)255);
	TestFormatResult("0XFF", "{0:P}", (void*)255);
#endif

	TestFormatResult("1.5", "{0}", 1.5f);
	TestFormatResult("1.500000", "{0:f}", 1.5f);
	TestFormatResult("1.500000e+00", "{0:e}", 1.5f);
	TestFormatResult("1.500000E+00", "{0:E}", 1.5f);
	TestFormatResult("1.5", "{0:g}", 1.5f);

	TestFormatResult("1.5", "{0}", 1.5);
	TestFormatResult("1.500000", "{0:f}", 1.5);
	TestFormatResult("1.500000e+00", "{0:e}", 1.5);
	TestFormatResult("1.500000E+00", "{0:E}", 1.5);
	TestFormatResult("1.5", "{0:g}", 1.5);

	TestFormatResult("    1", "{0,5}", 1);
	TestFormatResult("1    ", "{0,-5}", 1);

	TestFormatResult("0000A", "{0:X5}", 10);
	TestFormatResult("     0000A", "{0,10:X5}", 10);
	TestFormatResult("0000A     ", "{0,-10:X5}", 10);

	TestFormatResult("10 9 8 7 6 5 4 3 2 1 0", "{10} {9} {8} {7} {6} {5} {4} {3} {2} {1} {0}", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
}

DEFINE_TEST_G(HybridFormat, MSF_Format)
{
	TestFormatResult("c", "{}", 'c');
	TestFormatResult("c", "{:C}", 'c');

	TestFormatResult("test", "{}", "test");
	TestFormatResult("test", "{:S}", "test");

	TestFormatResult("-5", "{}", -5);
	TestFormatResult("-5", "{:d}", -5);
	TestFormatResult("-5", "{:i}", -5);
	TestFormatResult("4294967291", "{:u}", -5);
	TestFormatResult("37777777773", "{:o}", -5);
	TestFormatResult("fffffffb", "{:x}", -5);
	TestFormatResult("FFFFFFFB", "{:X}", -5);

	TestFormatResult("-5", "{:d}", -5LL);
	TestFormatResult("-5", "{:i}", -5LL);
	TestFormatResult("18446744073709551611", "{:u}", -5LL);
	TestFormatResult("1777777777777777777773", "{:o}", -5LL);
	TestFormatResult("fffffffffffffffb", "{:x}", -5LL);
	TestFormatResult("FFFFFFFFFFFFFFFB", "{:X}", -5LL);

#if defined(_MSC_VER)
#if WIN64
	TestFormatResult("00000000000000FF", "{}", (void*)255);
	TestFormatResult("00000000000000FF", "{:P}", (void*)255);
#else
	TestFormatResult("000000FF", "{}", (void*)255);
	TestFormatResult("000000FF", "{:P}", (void*)255);
#endif
#else
	TestFormatResult("0xff", "{}", (void*)255);
	TestFormatResult("0XFF", "{:P}", (void*)255);
#endif

	TestFormatResult("1.5", "{}", 1.5f);
	TestFormatResult("1.500000", "{:f}", 1.5f);
	TestFormatResult("1.500000e+00", "{:e}", 1.5f);
	TestFormatResult("1.500000E+00", "{:E}", 1.5f);
	TestFormatResult("1.5", "{:g}", 1.5f);

	TestFormatResult("1.5", "{}", 1.5);
	TestFormatResult("1.500000", "{:f}", 1.5);
	TestFormatResult("1.500000e+00", "{:e}", 1.5);
	TestFormatResult("1.500000E+00", "{:E}", 1.5);
	TestFormatResult("1.5", "{:g}", 1.5);

	TestFormatResult("    1", "{,5}", 1);
	TestFormatResult("1    ", "{,-5}", 1);

	TestFormatResult("0000A", "{:X5}", 10);
	TestFormatResult("     0000A", "{,10:X5}", 10);
	TestFormatResult("0000A     ", "{,-10:X5}", 10);
}

template <typename... Args>
static void TestFormatResultFail(char const* aFormat, Args... someArgs)
{
	char tmp[256];
	TEST_LESS(MSF_Format(tmp, aFormat, someArgs...), 0);
	TEST_EQ(strncmp(tmp, "ER_", 3), 0);
}

DEFINE_TEST_G(ErrorConditions, MSF_Format)
{
	MSF_CustomPrint::SetLocalErrorMode(MSF_ErrorMode::WriteString);

	TestFormatResultFail("%d %d %d %d", 1, 2);
	TestFormatResultFail("{} {} {} {}", 1, 2);
	TestFormatResultFail("{0} {1} {2} {3}", 1, 2);

	TestFormatResultFail("{0} %d {} %d", 1, 2, 3, 4);
	TestFormatResultFail("{} %d {0} %d", 1, 2, 3, 4);
	TestFormatResultFail("%d {0} %d {}", 1, 2, 3, 4);

	TestFormatResultFail("%-123.");
	TestFormatResultFail("%f");
	TestFormatResultFail("% and then");

	TestFormatResultFail("{0", 1);
	TestFormatResultFail("}", 1);
	TestFormatResultFail("{1}", 1);
	TestFormatResultFail("{0,}", 1);
	TestFormatResultFail("{0,1:}", 1);
	TestFormatResultFail("{0,1^}", 1);

	TestFormatResultFail("{", 1);
	TestFormatResultFail("}", 1);
	TestFormatResultFail("{,}", 1);
	TestFormatResultFail("{,1:}", 1);
	TestFormatResultFail("{,1^}", 1);

	char tmp[256];
	tmp[0] = 1;
	TEST_LESS(MSF_Format((char*)tmp, 0, "foo"), 0);
	TEST_EQ(tmp[0], 1);

	TEST_LESS(MSF_Format((char*)nullptr, 0, "foo"), 0);

	MSF_CustomPrint::ClearLocalErrorMode();
}

static char* locRealloc(char* aString, size_t aSize, void* aUserData)
{
	char** outString = (char**)aUserData;
	return *outString = (char*)realloc(aString, aSize);
}

static size_t locSTLStringFormat(char*& aString, MSF_StringFormatTemplate<char> const& aFormat)
{
	return MSF_FormatString(aFormat, aString, aString ? strlen(aString) : 0, 0, &locRealloc, &aString);
}

static size_t locSTLStringAppendFormat(char*& aString, MSF_StringFormatTemplate<char> const& aFormat)
{
	return MSF_FormatString(aFormat, aString, aString ? strlen(aString) : 0, aString ? strlen(aString) : 0, &locRealloc, &aString);
}

DEFINE_TEST_G(Realloc, Format)
{
	char* string = nullptr;
	TEST_GREATER(locSTLStringFormat(string, MSF_MakeStringFormat("How is this for a really long test string? %05d %.5f", 5, 1.f)), 15);
	TEST_STR_EQ(string, "How is this for a really long test string? 00005 1.00000");

	TEST_GREATER(locSTLStringAppendFormat(string, MSF_MakeStringFormat(" This is newly added %c %s", 'b', "foo")), 0);
	TEST_STR_EQ(string, "How is this for a really long test string? 00005 1.00000 This is newly added b foo");

	free(string);
}

template <typename Type>
static void locTestLimits(char const* aFormat, Type aMin, Type aMax, char const* aResult)
{
	char tmp[128];
	MSF_Format(tmp, aFormat, aMin, aMax);
	TEST_STR_EQ(tmp, aResult);
}

DEFINE_TEST_G(FormatLimits, MSF_Format)
{
	locTestLimits<int64_t>("(%d,%d)", INT64_MIN, INT64_MAX, "(-9223372036854775808,9223372036854775807)");
	locTestLimits<int32_t>("(%d,%d)", INT32_MIN, INT32_MAX, "(-2147483648,2147483647)");
	locTestLimits<int16_t>("(%d,%d)", INT16_MIN, INT16_MAX, "(-32768,32767)");
	locTestLimits<int8_t>("(%d,%d)", INT8_MIN, INT8_MAX, "(-128,127)");
	locTestLimits<uint64_t>("(%d,%d)", 0, UINT64_MAX, "(0,-1)");
	locTestLimits<uint32_t>("(%d,%d)", 0, UINT32_MAX, "(0,-1)");
	locTestLimits<uint16_t>("(%d,%d)", 0, UINT16_MAX, "(0,-1)");
	locTestLimits<uint8_t>("(%d,%d)", 0, UINT8_MAX, "(0,-1)");

	locTestLimits<int64_t>("(%i,%i)", INT64_MIN, INT64_MAX, "(-9223372036854775808,9223372036854775807)");
	locTestLimits<int32_t>("(%i,%i)", INT32_MIN, INT32_MAX, "(-2147483648,2147483647)");
	locTestLimits<int16_t>("(%i,%i)", INT16_MIN, INT16_MAX, "(-32768,32767)");
	locTestLimits<int8_t>("(%i,%i)", INT8_MIN, INT8_MAX, "(-128,127)");
	locTestLimits<uint64_t>("(%i,%i)", 0, UINT64_MAX, "(0,-1)");
	
	locTestLimits<uint32_t>("(%i,%i)", 0, UINT32_MAX, "(0,-1)");
	locTestLimits<uint16_t>("(%i,%i)", 0, UINT16_MAX, "(0,-1)");
	locTestLimits<uint8_t>("(%i,%i)", 0, UINT8_MAX, "(0,-1)");

	locTestLimits<int64_t>("(%u,%u)", INT64_MIN, INT64_MAX, "(9223372036854775808,9223372036854775807)");
	locTestLimits<int32_t>("(%u,%u)", INT32_MIN, INT32_MAX, "(2147483648,2147483647)");
	locTestLimits<int16_t>("(%u,%u)", INT16_MIN, INT16_MAX, "(32768,32767)");
	locTestLimits<int8_t>("(%u,%u)", INT8_MIN, INT8_MAX, "(128,127)");
	locTestLimits<uint64_t>("(%u,%u)", 0, UINT64_MAX, "(0,18446744073709551615)");
	locTestLimits<uint32_t>("(%u,%u)", 0, UINT32_MAX, "(0,4294967295)");
	locTestLimits<uint16_t>("(%u,%u)", 0, UINT16_MAX, "(0,65535)");
	locTestLimits<uint8_t>("(%u,%u)", 0, UINT8_MAX, "(0,255)");
}

DEFINE_TEST_G(UTF16, MSF_Format)
{
	TestFormatResult(u8"œ", "%c", u'œ');
	TestFormatResult(u8"œ", "%c", U'œ');
	TestFormatResult(u8"œ", "%c", L'œ');

	TestFormatResult(u8"œ œ", "%s", u"œ œ");
	TestFormatResult(u8"œ œ", "%s", U"œ œ");
	TestFormatResult(u8"œ œ", "%s", L"œ œ");

	TEST_STR_EQ(MSF_StrFmt("%s", "foo"), "foo");
}

DEFINE_TEST_G(FormatCopy, MSF_Format)
{
	TEST_EQ(nullptr, MSF_CopyStringFormat(MSF_MakeStringFormat(""), [](size_t) -> void* { return nullptr; }));

	char testString[256];
	char targetString[256];
	{
		MSF_CopyChars(testString, testString + 256, "{} {} {} {}");

		auto copy = MSF_CopyStringFormat(
			MSF_MakeStringFormat((char const*)testString, 'c', 5, (char*)nullptr, "Oh and a big string"),
			[](size_t aSize) { return malloc(aSize); });

		testString[0] = 0;

		TEST_GREATER(MSF_FormatString(*copy, targetString, 256), 0);
		TEST_STR_EQ(targetString, "c 5 (null) Oh and a big string");
		free((void*)copy);
	}

	{
		MSF_CopyChars(testString, testString + 256, "{} {} {} {}");

		auto copy = MSF_CopyStringFormat(
			MSF_MakeStringFormat((char const*)testString, 'c', 5, 1.5f, "Oh and a big string"),
			[](size_t aSize) { return malloc(aSize); }, false);

		testString[0] = 0;

		TEST_EQ(MSF_FormatString(*copy, targetString, 256), 0);
		TEST_STR_EQ(targetString, "");

		MSF_CopyChars(testString, testString + 256, "{} {}");

		TEST_GREATER(MSF_FormatString(*copy, targetString, 256), 0);
		TEST_STR_EQ(targetString, "c 5");

		free((void*)copy);
	}
}

DEFINE_TEST_G(Wildcards, MSF_Format)
{
	TestFormatResult("    c", "%*c", 5, 'c');
	TestFormatResult("1.1", "%.*f", 1, 1.14f);
	TestFormatResult("  1.1", "%*.*f", 5, 1, 1.14f);
}
