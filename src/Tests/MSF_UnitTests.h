#pragma once

#define ERROR_ACTION if (MSF_UnitTestFailureBreak) MSF_BREAK;

#include <simpletest.h>
#include <string.h>

#include "MSF_Assert.h"
#include "MSF_Utilities.h"
#include "MSF_UTF.h"

extern bool MSF_UnitTestFailureBreak;

template <typename Char, typename... Args>
struct TestHelper
{
	static void Run(char const* anExpectedResult, char const* aFormat, Args... someArgs)
	{
		Char expected[256];
		MSF_UTFCopy(expected, anExpectedResult);
		Char format[256];
		MSF_UTFCopy(format, aFormat);

		Char tmp[256];
		TEST_EQ(MSF_Format(tmp, format, someArgs...), (intptr_t)MSF_Strlen(expected));
		TEST_MESSAGE(memcmp(tmp, expected, (MSF_Strlen(expected) + 1) * sizeof(Char)) == 0, "%s", MSF_StrFmt("%s != %s", tmp, expected));
	}
};

template <typename... Args>
struct TestHelper<char, Args...>
{
	static void Run(char const* anExpectedResult, char const* aFormat, Args... someArgs)
	{
		char tmp[256];
		TEST_EQ(MSF_Format(tmp, aFormat, someArgs...), (intptr_t)MSF_Strlen(anExpectedResult));
		TEST_STR_EQ(tmp, anExpectedResult);
	}
};

template <typename... Args>
static void TestFormatResult(char const* anExpectedResult, char const* aFormat, Args... someArgs)
{
	TestHelper<char, Args...>::Run(anExpectedResult, aFormat, someArgs...);
	TestHelper<char16_t, Args...>::Run(anExpectedResult, aFormat, someArgs...);
	TestHelper<char32_t, Args...>::Run(anExpectedResult, aFormat, someArgs...);
	TestHelper<wchar_t, Args...>::Run(anExpectedResult, aFormat, someArgs...);
}
