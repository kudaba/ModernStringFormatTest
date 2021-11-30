#include "MSF_UnitTests.h"
#include "MSF_Format.h"
#include "MSF_Utilities.h"

#include <chrono>
#include <float.h>
#include <locale>
#include <random>
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

	std::minstd_rand0 Engine;
	std::uniform_int_distribution<uint64_t> RandomInt;
	std::uniform_real_distribution<long double> RandomFloat;


	FormatTestRunner() : MSFTotalTime(0), STDTotalTime(0), PrintFErrors(0), TestCount(0) {}

	template <typename... Args>
	int Sprintf(char* aDest, char const* aFormat, Args const&... someArgs)
	{
		return sprintf(aDest, aFormat, someArgs...);
	}
	template <int Size, typename... Args>
	int Sprintf(wchar_t (&aDest)[Size], wchar_t const* aFormat, Args const&... someArgs)
	{
		return swprintf(aDest, Size, aFormat, someArgs...);
	}

	bool Streq(char const* aLeft, char const* aRight)
	{
		return strcmp(aLeft, aRight) == 0;
	}

	bool Streq(wchar_t const* aLeft, wchar_t const* aRight)
	{
		return wcscmp(aLeft, aRight) == 0;
	}

	template <typename Char, typename ValueType>
	void DoOneTest(Char aType, ValueType aValue, char const* aModifier, char const* aFlags, int aWidth, int aPrecision)
	{
		++TestCount;

		Char format[256];
		Char convert[] = { '{', '}', 0 };

		// build format string
		{
			Char const* formatWriteEnd = format + sizeof(format) / sizeof(format[0]);
			Char* formatWrite = format;
			*formatWrite++ = '%';

			formatWrite += MSF_Format(formatWrite, formatWriteEnd - formatWrite, convert, aFlags);

			if (aWidth >= 0) formatWrite += MSF_Format(formatWrite, formatWriteEnd - formatWrite, convert, aWidth);
			if (aPrecision >= 0) *formatWrite++ = '.';
			if (aPrecision > 0) formatWrite += MSF_Format(formatWrite, formatWriteEnd - formatWrite, convert, aPrecision - 1);

			formatWrite += MSF_Format(formatWrite, formatWriteEnd - formatWrite, convert, aModifier);
			*formatWrite++ = aType;
			*formatWrite++ = 0;
		}

		Char msfBuffer[256]; // our version of sprintf
		Char stlBuffer[256]; // stl version of sprintf
		Char valueBuffer[256]; // raw value being printed

		Char valuePrintfPrintf[8];
		MSF_Format(valuePrintfPrintf, convert, "%%%s%c");

		Char valuePrintf[8];
		MSF_Format(valuePrintf, valuePrintfPrintf, aModifier, aType);

		Sprintf(valueBuffer, valuePrintf, aValue);

		{
			auto perfTime = HighResClock::now();
			Sprintf(stlBuffer, format, aValue);
			STDTotalTime += HighResClock::now() - perfTime;
		}

		// Must come second, there's a compilersta error that's causing aValue to change or get lost
		// when optimizations are enabled
		{
			auto perfTime = HighResClock::now();
			MSF_Format(msfBuffer, format, aValue);
			MSFTotalTime += HighResClock::now() - perfTime;
		}

		bool equivalent = Streq(msfBuffer, stlBuffer);
		if (!equivalent)
		{
			auto error = MSF_StrFmtUTF8<1024>("Format Error for: \"%s\" Value[%s]\nMSF: \"%s\"\nstd: \"%s\"", format, valueBuffer, msfBuffer, stlBuffer);
			TEST_MESSAGE(equivalent, "%s", (char const*)error);
			PrintFErrors++;
		}
		else
		{
			// Don't do formatting for all conditions otherwise tests will take forever.
			TEST_MESSAGE(true, "");
		}
	}

	template <typename Char>
	void DoPrintfTestT(char aType, char const* aModifier, char const* aFlags, int aWidth = -1, int aPrecision = -1)
	{
		switch (aType)
		{
		case 'c':
#if defined(_MSC_VER)
#pragma warning(disable:4127) // expression is constant
			if (sizeof(Char) == sizeof(wchar_t))
			{
				if (strcmp(aModifier, "h") == 0)
					DoOneTest<Char>(aType, 'a' + (char)(RandomInt(Engine) % ('z' - 'a')), aModifier, aFlags, aWidth, aPrecision);
				else if (aModifier[0] == 0 || aModifier[0] == 'w' || strcmp(aModifier, "l") == 0)
					DoOneTest<Char>(aType, (wchar_t)(RandomInt(Engine) % 0xd800), aModifier, aFlags, aWidth, aPrecision);
			}
			else
#endif
			{
				if (aModifier[0] == 0 || strcmp(aModifier, "h") == 0)
					DoOneTest<Char>(aType, 'a' + (char)(RandomInt(Engine) % ('z' - 'a')), aModifier, aFlags, aWidth, aPrecision);
				else if (aModifier[0] == 'w' || strcmp(aModifier, "l") == 0)
					DoOneTest<Char>(aType, (wchar_t)(RandomInt(Engine) % 0xd800), aModifier, aFlags, aWidth, aPrecision);
			}
			break;
		case 's':
#if defined(_MSC_VER)
			if (sizeof(Char) == sizeof(wchar_t))
			{
				//if (strcmp(aModifier, "h") == 0)
				//	DoOneTest<Char>(aType, u8"无国界医生", aModifier, aFlags, aWidth, aPrecision);
				//else
					if (aModifier[0] == 0 || aModifier[0] == 'w' || strcmp(aModifier, "l") == 0)
					DoOneTest<Char>(aType, L"无国界医生", aModifier, aFlags, aWidth, aPrecision);
			}
			else
#endif
			{
				if (aModifier[0] == 0 || strcmp(aModifier, "h") == 0)
					DoOneTest<Char>(aType, u8"无国界医生", aModifier, aFlags, aWidth, aPrecision);
				else if (aModifier[0] == 'w' || strcmp(aModifier, "l") == 0)
					DoOneTest<Char>(aType, L"无国界医生", aModifier, aFlags, aWidth, aPrecision);
			}
			break;
		case 'd':
		case 'i':
		case 'o':
		case 'u':
		case 'x':
		case 'X':
			switch (aModifier[0])
			{
			case 0:
				DoOneTest<Char>(aType, 0, aModifier, aFlags, aWidth, aPrecision);
				DoOneTest<Char>(aType, (int)RandomInt(Engine), aModifier, aFlags, aWidth, aPrecision);
				break;
			case 'h':
				if (aModifier[1] == 'h')
					DoOneTest<Char>(aType, (char)RandomInt(Engine), aModifier, aFlags, aWidth, aPrecision);
				else
					DoOneTest<Char>(aType, (short)RandomInt(Engine), aModifier, aFlags, aWidth, aPrecision);
				break;
			case 'l':
				if (aModifier[1] == 'l')
					DoOneTest<Char>(aType, (long long)RandomInt(Engine), aModifier, aFlags, aWidth, aPrecision);
				else
					DoOneTest<Char>(aType, (long)RandomInt(Engine), aModifier, aFlags, aWidth, aPrecision);
				break;
			case 'j':
				DoOneTest<Char>(aType, (intmax_t)RandomInt(Engine), aModifier, aFlags, aWidth, aPrecision);
				break;
			case 'z':
				DoOneTest<Char>(aType, (size_t)RandomInt(Engine), aModifier, aFlags, aWidth, aPrecision);
				break;
			case 't':
				DoOneTest<Char>(aType, (ptrdiff_t)RandomInt(Engine), aModifier, aFlags, aWidth, aPrecision);
				break;
			case 'I':
				if (aModifier[1] == '6')
					DoOneTest<Char>(aType, (uint64_t)RandomInt(Engine), aModifier, aFlags, aWidth, aPrecision);
				else if (aModifier[1] == '3')
					DoOneTest<Char>(aType, (uint32_t)RandomInt(Engine), aModifier, aFlags, aWidth, aPrecision);
				else
					DoOneTest<Char>(aType, (size_t)RandomInt(Engine), aModifier, aFlags, aWidth, aPrecision);
				break;
			}
			break;
		case 'p':
		case 'P':
			if (aModifier[0] == 0)
			{
				DoOneTest<Char>(aType, (void*)0, aModifier, aFlags, aWidth, aPrecision);
				DoOneTest<Char>(aType, (void*)RandomInt(Engine), aModifier, aFlags, aWidth, aPrecision);
			}
			break;
			break;
		case 'e':
		case 'E':
		case 'f':
		case 'F':
		case 'g':
		case 'G':
			switch (aModifier[0])
			{
			case 0:
			{
				DoOneTest<Char>(aType, 0.f, aModifier, aFlags, aWidth, aPrecision);
				DoOneTest<Char>(aType, (double)RandomFloat(Engine), aModifier, aFlags, aWidth, aPrecision);
			}
			case 'l':
				if (aModifier[1] != 'l')
					DoOneTest<Char>(aType, (double)RandomFloat(Engine), aModifier, aFlags, aWidth, aPrecision);
				break;
			case 'L':
				DoOneTest<Char>(aType, (long double)RandomFloat(Engine), aModifier, aFlags, aWidth, aPrecision);
				break;
			}
			break;
		default:
			TestFixture::GetCurrentTest()->LogMessage(TEST_ERROR_PREFIX_ "Invalid printf test type %c", "switch (aType)", aType);
			return;
		}
	}

	void DoPrintfTest(char aType, char const* aModifier, char const* aFlags, int aWidth = -1, int aPrecision = -1)
	{
		DoPrintfTestT<char>(aType, aModifier, aFlags, aWidth, aPrecision);
		DoPrintfTestT<wchar_t>(aType, aModifier, aFlags, aWidth, aPrecision);
	}
};


// comprehensive testing for accuracy and speed
DEFINE_TEST_G(PrintfAccuracy, MSF_String)
{
	srand(73737); // whatev, just keep it consistent between runs
#if defined(_MSC_VER)
	setlocale(LC_ALL, ".UTF-8");
#else
	setlocale(LC_ALL, "C.UTF-8");
#endif
	char const types[] = { 'c', 's', 'd', 'i', 'o', 'u', 'x', 'X', 'p', 'e', 'E', 'f', 'F', 'g', 'G'};
	char const* modifiers[] =
	{
		// Global standard
		"", "hh", "h", "l", "ll", "j", "z", "t", "L"
#if defined(_MSC_VER)
		// MSVC
		, "I", "I32", "I64", "w"
#endif
	};
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

	int typeIndex = 0;
	for (char const type : types)
	{
		auto PrintFstart = Tester.MSFTotalTime;
		auto printfstart = Tester.STDTotalTime;

		for (char const* mod : modifiers)
		{
			for (char const* flag : flags)
			{
				Tester.DoPrintfTest(type, mod, flag);
				for (int width = -1; width < 15; ++width)
				{
					Tester.DoPrintfTest(type, mod, flag, width);
					for (uint32_t precision = 0; precision < 11; ++precision)
					{
						for (uint32_t iteration = 0; iteration < iterations; ++iteration)
						{
							Tester.DoPrintfTest(type, mod, flag, width, precision);
						}
					}
				}
			}
		}
		msfTimes[typeIndex] = Tester.MSFTotalTime - PrintFstart;
		stdTime[typeIndex] = Tester.STDTotalTime - printfstart;
		++typeIndex;
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
		Printf("%-5c %10s %10s(%%% 3.1f)\n", types[i], timeFromHighRes(msfTimes[i], msfBuffer), timeFromHighRes(stdTime[i], stdBuffer), double(msfTimes[i].count()) / double(stdTime[i].count()) * 100.0);
	}
	Printf("%-5s %10s %10s(%%% 3.1f)\n", "Total", timeFromHighRes(Tester.MSFTotalTime, msfBuffer), timeFromHighRes(Tester.STDTotalTime, stdBuffer), double(Tester.MSFTotalTime.count()) / double(Tester.STDTotalTime.count()) * 100.0);
}
