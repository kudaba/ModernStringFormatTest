#include "MSF_UnitTests.h"
#include "MSF_Format.h"
#include "MSF_FormatPrint.h"
#include "MSF_FormatStandardTypes.h"

#include <chrono>
#include <string>

enum TimeFlags
{
	Second,
	Milli,
	Micro,
	Nano
};

MSF_DEFINE_USER_PRINTF_TYPE_FLAG(std::chrono::seconds, 0, Second);
MSF_DEFINE_USER_PRINTF_TYPE_FLAG(std::chrono::milliseconds, 0, Milli);
MSF_DEFINE_USER_PRINTF_TYPE_FLAG(std::chrono::microseconds, 0, Micro);
MSF_DEFINE_USER_PRINTF_TYPE_FLAG(std::chrono::nanoseconds, 0, Nano);

namespace MSF_StringFormatTime
{
	static const uint32_t ValidTypes = MSF_StringFormatTypeLookup<std::chrono::seconds>::ID;

	size_t Validate(MSF_PrintData& aPrintData, MSF_StringFormatType const& aValue)
	{
		MSF_ASSERT(aValue.myType & ValidTypes);

		std::chrono::seconds const* timeValue = (std::chrono::seconds const*)aValue.myUserType;

		return MSF_CustomPrint::ValidateType('d', aPrintData, MSF_StringFormatType(timeValue->count())) + 1 + (aValue.myUserData != Second);
	}
	size_t Print(char* aBuffer, char const* aBufferEnd, MSF_PrintData const& aData)
	{
		std::chrono::seconds const* timeValue = (std::chrono::seconds const*)aData.myValue->myUserType;

		char* buffer = aBuffer + MSF_CustomPrint::PrintType('d', aBuffer, aBufferEnd, aData, MSF_StringFormatType(timeValue->count()));
		MSF_ASSERT(aBufferEnd - buffer >= 2);
		switch (aData.myValue->myUserData)
		{
		case Milli:
			*buffer++ = 'm';
			break;
		case Micro:
			*buffer++ = 'u';
			break;
		case Nano:
			*buffer++ = 'n';
			break;
		}

		*buffer++ = 's';

		return buffer - aBuffer;
	}
}

static bool locTimeRegisterd = []()
{
	MSF_CustomPrint::RegisterDefaultPrintFunction('t', MSF_StringFormatTime::ValidTypes, MSF_StringFormatTime::Validate, MSF_StringFormatTime::Print);
	return true;
}();

DEFINE_TEST_G(Format, CustomType)
{
	TestFormatResult("5s", "%t", std::chrono::seconds(5));
	TestFormatResult("5ms", "%t", std::chrono::milliseconds(5));
	TestFormatResult("5us", "%t", std::chrono::microseconds(5));
	TestFormatResult("5ns", "%t", std::chrono::nanoseconds(5));

	TestFormatResult("5s", "{}", std::chrono::seconds(5));
	TestFormatResult("5ms", "{}", std::chrono::milliseconds(5));
	TestFormatResult("5us", "{}", std::chrono::microseconds(5));
	TestFormatResult("5ns", "{}", std::chrono::nanoseconds(5));
}

MSF_DEFINE_TYPE_CONVERSION(std::string, aString.c_str());

DEFINE_TEST_G(Conversion, CustomType)
{
	TestFormatResult("This is a long string to force allocation", "{}", std::string("This is a long string to force allocation"));

	char tmp[256];
	TEST_GREATER(MSF_FormatString(MSF_MakeStringFormat("{}", std::string("This is a long string to force allocation")), tmp, sizeof(tmp)), 0);
	TEST_STR_EQ(tmp, "This is a long string to force allocation");
}