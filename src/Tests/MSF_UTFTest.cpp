#include "MSF_UnitTests.h"
#include "MSF_UTF.h"
#include "MSF_Utilities.h"

#ifndef __STDC_UTF_16__
#define __STDC_UTF_16__
#endif

#include <string.h>

#if !defined(__APPLE__)
#include <uchar.h>

DEFINE_TEST(UTF)
{
	char utf8TestString[32];
	char16_t utf16TestString[32];
	mbstate_t state = {};
	for (uint32_t codePoint = 0; codePoint < 100000; ++codePoint) // there are more than 100k unicode character, I think 100k is enough of a test here
	{
		size_t const utf8CharsWritten = c32rtomb(utf8TestString, codePoint, &state);
		if ((intptr_t)utf8CharsWritten > 0)
		{
			utf8TestString[utf8CharsWritten] = 0;

			uint32_t utf16CharsWritten = 1;

			if (codePoint == 0)
			{
				TEST_EQ(mbrtoc16(utf16TestString, utf8TestString, utf8CharsWritten, &state), 0);
			}
			else
			{
				TEST_EQ(mbrtoc16(utf16TestString, utf8TestString, utf8CharsWritten, &state), (size_t)utf8CharsWritten);
			}

			if (mbrtoc16(&utf16TestString[1], utf8TestString + utf8CharsWritten, 1, &state) == (size_t)-3)
			{
				utf16TestString[2] = 0;
				utf16CharsWritten = 2;
			}
			else
			{
				utf16TestString[1] = 0;
			}

			{
				auto read = MSF_ReadCodePoint(utf8TestString);
				TEST_EQ(read.CodePoint, codePoint);
				TEST_EQ(read.CharsRead, utf8CharsWritten);

				char utf8Write[5];
				utf8Write[MSF_WriteCodePoint(codePoint, utf8Write)] = 0;
				TEST_STR_EQ(utf8Write, utf8TestString);
			}
			{
				auto read = MSF_ReadCodePoint(utf16TestString);
				TEST_EQ(read.CodePoint, codePoint);
				TEST_EQ(read.CharsRead, utf16CharsWritten);

				char16_t utf16Write[5];
				MSF_WriteCodePoint(codePoint, utf16Write);
				TEST_EQ(memcmp(utf16Write, utf16TestString, utf16CharsWritten * sizeof(char16_t)), 0);
			}
		}
	}
}

#endif

template <typename A, typename B>
static void locTestUTFCopy(A const* testA, B const* testB)
{
	{
		A writeA[256];
		TEST_EQ(MSF_UTFCopyLength<A>(testB).Elements, MSF_Strlen(testA));
		TEST_EQ(MSF_UTFCopy(writeA, sizeof(writeA) / sizeof(A), testB).Elements, MSF_Strlen(testA));
		TEST_EQ(memcmp(writeA, testA, (MSF_Strlen(testA) + 1) * sizeof(A)), 0);
	}
	{
		B writeB[256];
		TEST_EQ(MSF_UTFCopyLength<B>(testA).Elements, MSF_Strlen(testB));
		TEST_EQ(MSF_UTFCopy(writeB, sizeof(writeB) / sizeof(B), testA).Elements, MSF_Strlen(testB));
		TEST_EQ(memcmp(writeB, testB, (MSF_Strlen(testB) + 1) * sizeof(B)), 0);
	}
}

DEFINE_TEST(UTFConversion)
{
	char const* testUTF8 = u8"😁 emojies 🐐	🐑	🐒	🐓	🐔	🐕	🐖	🐗	🐘	🐙	🐚	🐛	🐜	🐝	🐞	🐟";
	char16_t const* testUTF16 = u"😁 emojies 🐐	🐑	🐒	🐓	🐔	🐕	🐖	🐗	🐘	🐙	🐚	🐛	🐜	🐝	🐞	🐟";
	char32_t const* testUTF32 = U"😁 emojies 🐐	🐑	🐒	🐓	🐔	🐕	🐖	🐗	🐘	🐙	🐚	🐛	🐜	🐝	🐞	🐟";
	wchar_t const* testWChar = L"😁 emojies 🐐	🐑	🐒	🐓	🐔	🐕	🐖	🐗	🐘	🐙	🐚	🐛	🐜	🐝	🐞	🐟";

	locTestUTFCopy(testUTF8, testUTF16);
	locTestUTFCopy(testUTF8, testUTF32);
	locTestUTFCopy(testUTF8, testWChar);

	locTestUTFCopy(testUTF16, testUTF32);
	locTestUTFCopy(testUTF16, testWChar);

	locTestUTFCopy(testUTF32, testWChar);
}