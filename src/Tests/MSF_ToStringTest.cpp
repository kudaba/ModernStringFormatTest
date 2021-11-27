#include "MSF_UnitTests.h"
#include "MSF_ToString.h"

template <typename Type>
void TestIntegerToString(Type aValue, char const* aDecimal, char const* aHex, char const* anOctal, char const* aBinary)
{
	TEST_STR_EQ(MSF_IntegerToString(aValue), aDecimal);
	TEST_STR_EQ(MSF_IntegerToString(aValue, 16), aHex);
	TEST_STR_EQ(MSF_IntegerToString(aValue, 8), anOctal);
	TEST_STR_EQ(MSF_IntegerToString(aValue, 2), aBinary);
}

DEFINE_TEST_G(IntegerLimitsToString, ToString)
{
	TestIntegerToString(5, "5", "5", "5", "101");
	TestIntegerToString(-5, "-5", "fffffffb", "37777777773", "11111111111111111111111111111011");
	TestIntegerToString((uint8_t)UINT8_MAX, "255", "ff", "377", "11111111");
	TestIntegerToString((int8_t)INT8_MIN, "-128", "80", "200", "10000000");
	TestIntegerToString((int8_t)INT8_MAX, "127", "7f", "177", "1111111");
	TestIntegerToString((uint16_t)UINT16_MAX, "65535", "ffff", "177777", "1111111111111111");
	TestIntegerToString((int16_t)INT16_MIN, "-32768", "8000", "100000", "1000000000000000");
	TestIntegerToString((int16_t)INT16_MAX, "32767", "7fff", "77777", "111111111111111");
	TestIntegerToString((uint32_t)UINT32_MAX, "4294967295", "ffffffff", "37777777777", "11111111111111111111111111111111");
	TestIntegerToString((int32_t)INT32_MIN, "-2147483648", "80000000", "20000000000", "10000000000000000000000000000000");
	TestIntegerToString((int32_t)INT32_MAX, "2147483647", "7fffffff", "17777777777", "1111111111111111111111111111111");
	TestIntegerToString((uint64_t)UINT64_MAX, "18446744073709551615", "ffffffffffffffff", "1777777777777777777777", "1111111111111111111111111111111111111111111111111111111111111111");
	TestIntegerToString((int64_t)INT64_MIN, "-9223372036854775808", "8000000000000000", "1000000000000000000000", "1000000000000000000000000000000000000000000000000000000000000000");
	TestIntegerToString((int64_t)INT64_MAX, "9223372036854775807", "7fffffffffffffff", "777777777777777777777", "111111111111111111111111111111111111111111111111111111111111111");
}

DEFINE_TEST_G(IntegerOddBasesToString, ToString)
{
	TEST_STR_EQ(MSF_IntegerToString(-5, 16, 'A'), "FFFFFFFB");
	TEST_STR_EQ(MSF_IntegerToString(-5, 3), "102002022201221111122");
	TEST_STR_EQ(MSF_IntegerToString(-5, 4), "3333333333333323");
	TEST_STR_EQ(MSF_IntegerToString(-5, 32), "3vvvvvr");
}

DEFINE_TEST_G(BadIntegerToString, ToString)
{
	TEST_STR_EQ(MSF_IntegerToString(5, 0), "");
	TEST_STR_EQ(MSF_IntegerToString(5, 1), "");
}

void TestDoubleToString(double aValue, char const* aResult, char aFormat = 'f', uint32_t aWidth = 0, uint32_t aPrecision = 6, uint32_t someFlags = 0)
{
	char buffer[256];
	TEST_GREATER(MSF_DoubleToString(aValue, buffer, aFormat, aWidth, aPrecision, someFlags), 0);
	TEST_STR_EQ(buffer, aResult);
}

void TestDoubleToStrings(double aValue, char const* anFFormat, char const* anEFormat, char const* aGFormat)
{
	TestDoubleToString(aValue, anFFormat, 'f');
	TestDoubleToString(aValue, anEFormat, 'e');
	TestDoubleToString(aValue, aGFormat, 'g');
}

DEFINE_TEST_G(DoubleToString, ToString)
{
	TestDoubleToStrings(5, "5.000000", "5.000000e+00", "5");

	char buffer[256];
	TEST_EQ(MSF_DoubleToString(5, buffer), MSF_DoubleToStringLength(5));
	TEST_EQ(MSF_DoubleToString(5, buffer, 2), -1);
}
