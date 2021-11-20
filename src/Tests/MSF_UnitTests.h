#pragma once

#define ERROR_ACTION if (MSF_UnitTestFailureBreak) MSF_BREAK;

#include <simpletest.h>
#include "MSF_Assert.h"

extern bool MSF_UnitTestFailureBreak;

template <typename... Args>
static void TestFormatResult(char const* anExpectedResult, char const* aFormat, Args... someArgs)
{
	char tmp[256];
	TEST_GREATER(MSF_Format(tmp, aFormat, someArgs...), 0);
	TEST_STR_EQ(tmp, anExpectedResult);
}