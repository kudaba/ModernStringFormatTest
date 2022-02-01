#include "MSF_UnitTests.h"
#include "MSF_Utilities.h"
#include <stdio.h>

DEFINE_TEST_G(IsAlpha, Utitlities)
{
	for (char c = 0; c < INT8_MAX; ++c)
	{
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
			TEST_MESSAGE(MSF_IsAsciiAlphaNumeric(c), "%c is NOT an alphabet character", c);
		else
			TEST_MESSAGE(!MSF_IsAsciiAlphaNumeric(c), "%c is an alphabet character", c);
	}
}

DEFINE_TEST_G(IsDigit, Utitlities)
{
	for (char c = 0; c < INT8_MAX; ++c)
	{
		if (c >= '0' && c <= '9')
			TEST_MESSAGE(MSF_IsDigit(c), "%c is NOT a digit", c);
		else
			TEST_MESSAGE(!MSF_IsDigit(c), "%c is a digit", c);
	}
}

template <typename Type>
static void locIntMinMaxTest(Type start, Type end, Type step)
{
	for (Type i = start; i < end; i += step)
	{
		TEST_EQ(MSF_IntMax(i, start), i);
		TEST_EQ(MSF_IntMax(i, end), end);

		TEST_EQ(MSF_IntMin(i, start), start);
		TEST_EQ(MSF_IntMin(i, end), i);
	}
}

DEFINE_TEST_G(IntMinMax, Utitlities)
{
	locIntMinMaxTest<int8_t>(INT8_MIN/2, INT8_MAX/2, INT8_MAX / 7);
	locIntMinMaxTest<int16_t>(INT16_MIN/2, INT16_MAX/2, INT16_MAX / 8);
	locIntMinMaxTest<int32_t>(INT32_MIN/2, INT32_MAX/2, INT32_MAX / 8);
	locIntMinMaxTest<int64_t>(INT64_MIN/2, INT64_MAX/2, INT64_MAX / 8);

	locIntMinMaxTest<uint8_t>(0, UINT8_MAX / 2, UINT8_MAX / 7);
	locIntMinMaxTest<uint16_t>(0, UINT16_MAX / 2, UINT16_MAX / 8);
	locIntMinMaxTest<uint32_t>(0, UINT32_MAX / 2, UINT32_MAX / 8);
	locIntMinMaxTest<uint64_t>(0, UINT64_MAX / 2, UINT64_MAX / 8);
}

DEFINE_TEST_G(StrFmt, Utitlities)
{
	TEST_STR_EQ(MSF_StrFmt("{}", 5), "5");
	TEST_EQ(memcmp(MSF_StrFmtWChar<512>(L"{}", 5), L"5", sizeof(L"5")), 0);

	TestFormatResult("5", "{}", MSF_StrFmt("{}", 5));

	char tmp[MSF_DEFAULT_FMT_SIZE];
	sprintf(tmp, "%s", (char const*)MSF_StrFmt("{}", 5));
	TEST_STR_EQ(tmp, "5");
}
