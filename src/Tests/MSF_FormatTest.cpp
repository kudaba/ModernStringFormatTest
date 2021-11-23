#include "MSF_UnitTests.h"
#include "MSF_Format.h"
#include "MSF_FormatPrint.h"
#include "MSF_Utilities.h"
#include <string.h>
#include <stdlib.h>

DEFINE_TEST_G(BasicPrinting, MSF_String)
{
	TestFormatResult("%d", "%%d", 5);
	TestFormatResult("%5", "%%%d", 5);

	TestFormatResult("{0}", "{{0}}", 5);
	TestFormatResult("{5}", "{{{0}}}", 5);

	TestFormatResult("{}", "{{}}", 5);
	TestFormatResult("{5}", "{{{}}}", 5);

	TestFormatResult("{%d}", "{{%%d}}", 1);
	TestFormatResult("{0%d}", "{{0%%d}}", 1);

	TestFormatResult("1 2 3 4", "%d %d %d %d", 1, 2, 3, 4);
	TestFormatResult("1 2 3 4", "{0} {1} {2} {3}", 1, 2, 3, 4);
	TestFormatResult("1 2 3 4", "{} {} {} {}", 1, 2, 3, 4);
	TestFormatResult("1 2 3 4", "%d {} %d {}", 1, 2, 3, 4);

	TestFormatResult("1 1 2 2", "{0} {0} {1} {1}", 1, 2, 3, 4);
}

DEFINE_TEST_G(PrintfFormat, MSF_String)
{
	// Mostly test basic printing, accuracy test flexes the limits of each type
	TestFormatResult("c", "%c", 'c');
	TestFormatResult("c", "%C", 'c');

	TestFormatResult("test", "%s", "test");
	TestFormatResult("test", "%S", "test");

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

DEFINE_TEST_G(CSharpFormat, MSF_String)
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

	TestFormatResult("-5", "%d", (int64_t)-5, "-5");
	TestFormatResult("-5", "%i", (int64_t)-5, "-5");
	TestFormatResult("18446744073709551611", "%u", (int64_t)-5, "");
	TestFormatResult("1777777777777777777773", "%o", (int64_t)-5, "");
	TestFormatResult("fffffffffffffffb", "%x", (int64_t)-5, "");
	TestFormatResult("FFFFFFFFFFFFFFFB", "%X", (int64_t)-5, "");

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
}

DEFINE_TEST_G(HybridFormat, MSF_String)
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

	TestFormatResult("-5", "{:d}", (int64_t)-5, "-5");
	TestFormatResult("-5", "{:i}", (int64_t)-5, "-5");
	TestFormatResult("18446744073709551611", "{:u}", (int64_t)-5, "");
	TestFormatResult("1777777777777777777773", "{:o}", (int64_t)-5, "");
	TestFormatResult("fffffffffffffffb", "{:x}", (int64_t)-5, "");
	TestFormatResult("FFFFFFFFFFFFFFFB", "{:X}", (int64_t)-5, "");

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

DEFINE_TEST_G(ErrorConditions, MSF_String)
{
	MSF_CustomPrint::SetLocalErrorMode(MSF_CustomPrint::WriteString);

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
	TEST_LESS(MSF_Format(tmp, 0, "foo"), 0);
	TEST_EQ(tmp[0], 1);

	TEST_LESS(MSF_Format(nullptr, 0, "foo"), 0);

	MSF_CustomPrint::ClearLocalErrorMode();
}

template <int Size, typename... Args>
static void TestFormatResultError(char const* anErrorMessage, char const* aFormat, Args... someArgs)
{
	char tmp[Size];
	TEST_LESS(MSF_Format(tmp, aFormat, someArgs...), 0);
	TEST_EQ(strncmp(tmp, anErrorMessage, MSF_IntMin(strlen(anErrorMessage), sizeof(tmp))), 0);
}

DEFINE_TEST_G(FormatFailureStrings, MSF_String)
{
	MSF_CustomPrint::SetLocalErrorMode(MSF_CustomPrint::WriteString);

	TestFormatResultError<256>("ER_ExpectedClosingBrace", "{0", 1);
	TestFormatResultError<256>("ER_IndexOutOfRange", "{1}", 1);
	TestFormatResultError<256>("ER_IndexOutOfRange", "%f %f", 1.f);
	TestFormatResultError<16>("ER_NotEnoughSpace", "%s", "This is just a really long string");

	MSF_CustomPrint::ClearLocalErrorMode();
}

#if MSF_ASSERTS_ENABLED

int theAssertCount = 0;

static void locLogAssertExternal(const char*, int, char const*, MSF_StringFormat const&)
{
}

static bool locLogAssert()
{
	++theAssertCount;
	return false;
}

template <typename... Args>
static void TestFormatResultAsserts(char const* aFormat, Args... someArgs)
{
	char tmp[256];
	TEST_LESS(MSF_Format(tmp, aFormat, someArgs...), 0);
	TEST_EQ(tmp[0], 0);
	TEST_EQ(theAssertCount, 1);
	theAssertCount = 0;
}

DEFINE_TEST_G(FormatFailureAsserts, MSF_String)
{
	MSF_CustomPrint::SetLocalErrorMode(MSF_CustomPrint::Assert);
	MSF_LogAssertExternal = &locLogAssertExternal;
	MSF_LogAssertBreak = &locLogAssert;

	TestFormatResultAsserts("{0", 1);
	TestFormatResultAsserts("{1}", 1);
	TestFormatResultAsserts("%f %f", 1.f);

	MSF_LogAssertBreak = nullptr;
	MSF_LogAssertExternal = nullptr;
	MSF_CustomPrint::ClearLocalErrorMode();
}

#endif // MSF_ASSERTS_ENABLED

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

DEFINE_TEST_G(FormatLimits, MSF_String)
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
