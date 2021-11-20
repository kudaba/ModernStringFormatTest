#include "MSF_UnitTests.h"
#include "MSF_Format.h"
#include "MSF_Utilities.h"

#include <chrono>
#include <float.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern int MSF_AccuracyTestIterations;

using HighResClock = std::chrono::high_resolution_clock;
using HighResTime = std::chrono::nanoseconds;

class FormatTestRunner
{
public:
	HighResTime MSFTotalTime;
	HighResTime STDTotalTime;
	int PrintFErrors;
	int TestCount;

	FormatTestRunner() : MSFTotalTime(0), STDTotalTime(0), PrintFErrors(0), TestCount(0) {}

	template <typename ValueType>
	void DoOneTest(char aType, ValueType aValue, char const* aFlags, int aWidth, int aPrecision)
	{
		++TestCount;

		char format[256];

		// build format string
		{
			char const* formatWriteEnd = format + sizeof(format);
			char* formatWrite = format;
			*formatWrite++ = '%';

			{
				size_t const flagsLen = MSF_Strlen(aFlags);
				MSF_CopyChars(formatWrite, formatWriteEnd, aFlags, flagsLen);
				formatWrite += flagsLen;
			}

			if (aWidth >= 0) formatWrite += MSF_Format(formatWrite, formatWriteEnd - formatWrite, "{}", aWidth);
			if (aPrecision >= 0) *formatWrite++ = '.';
			if (aPrecision > 0) formatWrite += MSF_Format(formatWrite, formatWriteEnd - formatWrite, "{}", aPrecision - 1);
			*formatWrite++ = aType;
			*formatWrite++ = 0;
		}

		char msfBuffer[256]; // our version of sprintf
		char stlBuffer[256]; // stl version of sprintf
		char valueBuffer[256]; // raw value being printed

		char valuePrintf[3] = { '%', aType, '\0' };

		sprintf(valueBuffer, valuePrintf, aValue);

		{
			auto perfTime = HighResClock::now();
			sprintf(stlBuffer, format, aValue);
			STDTotalTime += HighResClock::now() - perfTime;
		}

		// Must come second, there's a compiler error that's causing aValue to change or get lost
		// when optimizations are enabled
		{
			auto perfTime = HighResClock::now();
			MSF_Format(msfBuffer, format, aValue);
			MSFTotalTime += HighResClock::now() - perfTime;
		}

		bool equivalent = strcmp(msfBuffer, stlBuffer) == 0;
		TEST_MESSAGE(equivalent, "Format Error for: \"%s\" Value[%s]\nMSF: \"%s\"\nstd: \"%s\"",
			format, valueBuffer, msfBuffer, stlBuffer);
		PrintFErrors += !equivalent;
	}

	void DoPrintfTest(char aType, char const* aFlags, int aWidth = -1, int aPrecision = -1)
	{
		switch (aType)
		{
		case 'c':
			DoOneTest<char>(aType, 'a' + (rand() % ('z' - 'a')), aFlags, aWidth, aPrecision);
			break;
		case 's':
			DoOneTest(aType, "test", aFlags, aWidth, aPrecision);
			break;
		case 'd':
		case 'i':
		case 'o':
		case 'u':
		case 'x':
		case 'X':
		case 'p':
		case 'P':
			DoOneTest(aType, rand() * rand(), aFlags, aWidth, aPrecision);
			break;
		break;
		case 'e':
		case 'E':
		case 'f':
		case 'F':
		case 'g':
		case 'G':
			static double v;
			// Test really big and really small numbers
			if (rand() % 1)
				v = (double)rand() * (double)rand();
			else
				v = (double)rand() / ((double)rand() + FLT_EPSILON);

			DoOneTest(aType, v, aFlags, aWidth, aPrecision);
			break;
		default:
			TestFixture::GetCurrentTest()->LogMessage(TEST_ERROR_PREFIX_ "Invalid printf test type %c", "switch (aType)", aType);
			return;
		}
	}
};


// comprehensive testing for accuracy and speed
DEFINE_TEST_G(PrintfAccuracy, MSF_String)
{
	srand(73737); // whatev, just keep it consistent between runs

	char types[] = { 'c', 's', 'd', 'i', 'o', 'u', 'x', 'X', 'p', 'e', 'E', 'f', 'F', 'g', 'G'};
	char const* flags[] =
	{
		"", "-", "+", "0", " ", "#",
		"-+", "-0", "- ", "-#", "-+0", "-+ ", "-+#", "-0 ", "-0#", "- #", "-+0 ", "-+0#", "-+ #", "-0 #", "-+0 #",
		"+0", "+ ", "+#", "+0 ", "+0#", "+ #", "+0 #",
		"0 ", "0#", "0 #",
		" #"
	};

#if defined(NDEBUG)
	uint32_t const iterations = MSF_IntMax(10, MSF_AccuracyTestIterations);
#else
	uint32_t const iterations = MSF_IntMax(1, MSF_AccuracyTestIterations);
#endif

	HighResTime msfTimes[sizeof(types)];
	HighResTime stdTime[sizeof(types)];

	FormatTestRunner Tester;

	for (uint32_t i = 0; i < sizeof(types); ++i)
	{
		auto PrintFstart = Tester.MSFTotalTime;
		auto printfstart = Tester.STDTotalTime;
		for (uint32_t j = 0; j < sizeof(flags) / sizeof(flags[0]); ++j)
		{
			Tester.DoPrintfTest(types[i], flags[j]);
			for (int w = -1; w < 15; ++w)
			{
				Tester.DoPrintfTest(types[i], flags[j], w);
				for (uint32_t p = 0; p < 11; ++p)
				{
					for (uint32_t c = 0; c < iterations; ++c)
					{
						Tester.DoPrintfTest(types[i], flags[j], w, p);
					}
				}
			}
		}
		msfTimes[i] = Tester.MSFTotalTime - PrintFstart;
		stdTime[i] = Tester.STDTotalTime - printfstart;
	}
	if (Tester.PrintFErrors)
	{
		Printf("Total printf tests: %d \nTotal printf errors: %d\n", Tester.TestCount, Tester.PrintFErrors);
	}

	auto timeFromHighRes = [](HighResTime time, char buffer[64])
	{
		MSF_Format(buffer, 64, "%0.3fms", std::chrono::duration_cast<std::chrono::milliseconds>(time).count() / 1000.f);
		return buffer;
	};

	char msfBuffer[64];
	char stdBuffer[64];

	Printf("%-5s %10s %10s\n", "Test", "MSF", "std");
	for (uint32_t i = 0; i < sizeof(types); ++i)
	{
		Printf("%-5c %10s %10s\n", types[i], timeFromHighRes(msfTimes[i], msfBuffer), timeFromHighRes(stdTime[i], stdBuffer));
	}
	Printf("%-5s %10s %10s\n", "Total", timeFromHighRes(Tester.MSFTotalTime, msfBuffer), timeFromHighRes(Tester.STDTotalTime, stdBuffer));
}
