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

	size_t ValidateUTF8(MSF_PrintData& aData, MSF_StringFormatType const& aValue)
	{
		MSF_ASSERT(aValue.myType & ValidTypes);

		std::chrono::seconds const* timeValue = (std::chrono::seconds const*)aValue.myUserType;

		return MSF_CustomPrint::ValidateTypeUTF8('d', aData, MSF_StringFormatType(timeValue->count())) + 1 + (aValue.myUserData != Second);
	}
	size_t ValidateUTF16(MSF_PrintData& aData, MSF_StringFormatType const& aValue)
	{
		MSF_ASSERT(aValue.myType & ValidTypes);

		std::chrono::seconds const* timeValue = (std::chrono::seconds const*)aValue.myUserType;

		return MSF_CustomPrint::ValidateTypeUTF16('d', aData, MSF_StringFormatType(timeValue->count())) + 1 + (aValue.myUserData != Second);
	}
	size_t ValidateUTF32(MSF_PrintData& aData, MSF_StringFormatType const& aValue)
	{
		MSF_ASSERT(aValue.myType & ValidTypes);

		std::chrono::seconds const* timeValue = (std::chrono::seconds const*)aValue.myUserType;

		return MSF_CustomPrint::ValidateTypeUTF32('d', aData, MSF_StringFormatType(timeValue->count())) + 1 + (aValue.myUserData != Second);
	}

	template <typename Char>
	size_t PrintShared(Char* aBuffer, Char const* aBufferEnd, MSF_PrintData const& aData)
	{
		std::chrono::seconds const* timeValue = (std::chrono::seconds const*)aData.myValue->myUserType;

		Char* buffer = aBuffer + MSF_CustomPrint::PrintType('d', aBuffer, aBufferEnd, aData, MSF_StringFormatType(timeValue->count()));
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

	size_t PrintUTF8(char* aBuffer, char const* aBufferEnd, MSF_PrintData const& aData)
	{
		return PrintShared(aBuffer, aBufferEnd, aData);
	}
	size_t PrintUTF16(char16_t* aBuffer, char16_t const* aBufferEnd, MSF_PrintData const& aData)
	{
		return PrintShared(aBuffer, aBufferEnd, aData);
	}
	size_t PrintUTF32(char32_t* aBuffer, char32_t const* aBufferEnd, MSF_PrintData const& aData)
	{
		return PrintShared(aBuffer, aBufferEnd, aData);
	}
}

static bool locTimeRegisterd = []()
{
	MSF_CustomPrint::RegisterDefaultPrintFunction('t', MSF_StringFormatTime::ValidTypes,
		{ MSF_StringFormatTime::ValidateUTF8, MSF_StringFormatTime::ValidateUTF16, MSF_StringFormatTime::ValidateUTF32,
		MSF_StringFormatTime::PrintUTF8, MSF_StringFormatTime::PrintUTF16, MSF_StringFormatTime::PrintUTF32 });
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