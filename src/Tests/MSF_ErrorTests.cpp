#define MSF_ValidationError(Error) do { myError = Error; return; } while(0);

#include "MSF_UnitTests.h"
#include "MSF_FormatPrint.h"

struct MSF_ErrorsFixture : TestFixture
{
	void Setup() override { MSF_CustomPrint::SetLocalErrorMode(MSF_ErrorMode::WriteString); }
	void TearDown() override { MSF_CustomPrint::ClearLocalErrorMode(); }
};

template <int Size, typename... Args>
static void TestFormatResultError(char const* aRuntimeError, char const* aCompileTimeError, MSF_STRING(char) aFormat, Args... someArgs)
{
#if MSF_VALIDATION_ENABLED
	if (aCompileTimeError)
		TEST_STR_EQ(aFormat.myError, aCompileTimeError);
	else
		TEST_EQ(aFormat.myError, nullptr);
#else
	(void)aCompileTimeError;
#endif

	char tmp[Size];
	if (aRuntimeError == nullptr)
	{
		TEST_GREATER(MSF_Format(tmp, aFormat, someArgs...), 0);
	}
	else
	{
		TEST_LESS(MSF_Format(tmp, aFormat, someArgs...), 0);
		TEST_EQ(strncmp(tmp, aRuntimeError, MSF_IntMin(strlen(aRuntimeError), sizeof(tmp) - 1)), 0);
	}
}

struct Unknown {};
MSF_DEFINE_USER_PRINTF_TYPE(Unknown, 1);

DEFINE_TEST_GF(FormatFailureStrings, MSF_Errors, MSF_ErrorsFixture)
{
	TestFormatResultError<16>("ER_NotEnoughSpace", nullptr, "%s", "This is just a really long string");

	static_assert(MSF_MAX_ARGUMENTS == 32, "update this if the default max args changes");
	TestFormatResultError<256>("ER_TooManyInputs", "Max number of arguments [" MSF_STR(MSF_MAX_ARGUMENTS) "] exceeded", "%s",
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33);

	TestFormatResultError<256>("ER_InconsistentPrintType", "Mismatched print modes, don't mix %x,{} with {N}", "{} {1}", 1.f, 2.f);
	TestFormatResultError<256>("ER_InconsistentPrintType", "Mismatched print modes, don't mix %x,{} with {N}", "%f {1}", 1.f, 2.f);

	TestFormatResultError<256>("ER_InvalidPrintCharacter", "Unexpected end of print specifier", "%123", 1);
	TestFormatResultError<256>("ER_InvalidPrintCharacter", "Unexpected end of print specifier", "%", 1);

	TestFormatResultError<256>("ER_TypeMismatch", "Type mismatch", "%d", "hi");
	TestFormatResultError<256>("ER_TypeMismatch", "Type mismatch", "{0:d}", "hi");
	TestFormatResultError<256>("ER_TypeMismatch", "Type mismatch", "{:d}", "hi");

	TestFormatResultError<256>("ER_UnregisteredChar", "Unknown type", "%q", "hi");
	TestFormatResultError<256>("ER_UnregisteredChar", "Unknown type", "{0:q}", "hi");
	TestFormatResultError<256>("ER_UnregisteredChar", "Unknown type", "{:q}", "hi");

	TestFormatResultError<256>("ER_UnexpectedBrace", "Mismatched closing brace", "}", 1);
	TestFormatResultError<256>("ER_UnexpectedBrace", "Mismatched closing brace", "{}}", 1);

	TestFormatResultError<256>("ER_IndexOutOfRange", "Index out of range", "{1}", 1);
	TestFormatResultError<256>("ER_IndexOutOfRange", "Index out of range", "%f %f", 1.f);
	TestFormatResultError<256>("ER_IndexOutOfRange", "Index out of range", "%f {}", 1.f);
	TestFormatResultError<256>("ER_IndexOutOfRange", "Index out of range", "{} {}", 1.f);
	
	TestFormatResultError<256>("ER_ExpectedWidth", "Expected digit for width", "{,}", 1);
	TestFormatResultError<256>("ER_ExpectedWidth", "Expected digit for width", "{,-}", 1);
	TestFormatResultError<256>("ER_ExpectedWidth", "Expected digit for width", "{,:d}", 1);

	TestFormatResultError<256>("ER_ExpectedClosingBrace", "Expected closing brace", "{0", 1);
	TestFormatResultError<256>("ER_ExpectedClosingBrace", "Expected closing brace", "{", 1);

	TestFormatResultError<256>("ER_UnsupportedType", nullptr, "{}", Unknown());
	TestFormatResultError<256>("ER_UnsupportedType", nullptr, "{0}", Unknown());

	TestFormatResultError<256>(nullptr, "Not all arguments were used", "{}", 1.f, 2.f);
	TestFormatResultError<256>(nullptr, "Not all arguments were used", "{1}", 1.f, 2.f);
	TestFormatResultError<256>(nullptr, "Not all arguments were used", "%f", 1.f, 2.f);
}

DEFINE_TEST_GF(Wildcards, MSF_Errors, MSF_ErrorsFixture)
{
	char testString[256];
	TEST_LESS(MSF_Format(testString, (char const*)"%**c", 1, 2, 'c'), 0);
	TEST_LESS(MSF_Format(testString, (char const*)"%5*c", 1, 2, 'c'), 0);
	TEST_LESS(MSF_Format(testString, (char const*)"%*5c", 1, 2, 'c'), 0);

	TestFormatResultError<256>("ER_InvalidPrintCharacter", "Unexpected end of print specifier", "%**c", 1, 2, 'c');
	TestFormatResultError<256>("ER_InvalidPrintCharacter", "Unexpected end of print specifier", "%5*c", 1, 'c');
	TestFormatResultError<256>("ER_InvalidPrintCharacter", "Unexpected end of print specifier", "%*5c", 1, 'c');

	TestFormatResultError<256>("ER_WildcardType", "Expected integer type for width wildcard", "%*c", "hi", 'c');
	TestFormatResultError<256>("ER_WildcardType", "Expected integer type for precision wildcard", "%.*c", "hi", 'c');

	TestFormatResultError<256>("ER_InvalidPrintCharacter", "Unexpected end of print specifier", "%*5c", 1, 'c');
	TestFormatResultError<256>("ER_InvalidPrintCharacter", "Unexpected end of print specifier", "%.*5c", 1, 'c');
}

DEFINE_TEST_GF(PedanticErrors, MSF_Errors, MSF_ErrorsFixture)
{
	bool shouldError = MSF_ERROR_PEDANTIC;
	char testString[256];

	TEST_EQ(MSF_Format(testString, (char const*)"%--c", 'c') < 0, shouldError);
	TEST_EQ(MSF_Format(testString, (char const*)"%++c", 'c') < 0, shouldError);
	TEST_EQ(MSF_Format(testString, (char const*)"%  c", 'c') < 0, shouldError);
	TEST_EQ(MSF_Format(testString, (char const*)"%##c", 'c') < 0, shouldError);
	TEST_EQ(MSF_Format(testString, (char const*)"%00c", 'c') < 0, shouldError);

#if MSF_ERROR_PEDANTIC
	TestFormatResultError<256>("ER_DuplicateFlag", "Duplicate Flag Detected", "%--c", 'c');
	TestFormatResultError<256>("ER_DuplicateFlag", "Duplicate Flag Detected", "%++c", 'c');
	TestFormatResultError<256>("ER_DuplicateFlag", "Duplicate Flag Detected", "%  c", 'c');
	TestFormatResultError<256>("ER_DuplicateFlag", "Duplicate Flag Detected", "%##c", 'c');
	TestFormatResultError<256>("ER_DuplicateFlag", "Duplicate Flag Detected", "%00c", 'c');

	TestFormatResultError<256>(nullptr, "0 flag has no effect when left aligned", "%0-c", 'c');
	TestFormatResultError<256>(nullptr, "0 flag has no effect when left aligned", "%-0c", 'c');
#endif
}

DEFINE_TEST_GF(RelaxedCSharp, MSF_Errors, MSF_ErrorsFixture)
{
	// Note that these will still trigger compile errors, but allow for bypassing runtime errors

	MSF_CustomPrint::SetLocalErrorFlags(MSF_ErrorFlags::RelaxedCSharpFormat);

	TestFormatResult("{555}", "{555}", 5);
	TestFormatResult("5{{555}}5", "{0}{{{555}}}{0}", 5);
	TestFormatResult("{blarg}", "{blarg}", 5);
	TestFormatResult("5{{555}}", "{}{{{555}}}", 5);
	TestFormatResult("{0 }", "{0 }", 5);

	TestFormatResultError<256>("ER_IndexOutOfRange", "Mismatched print modes, don't mix %x,{} with {N}", "{}{{{555}}}{}", 5);

	MSF_CustomPrint::ClearLocalErrorFlags();
}

#if MSF_ASSERTS_ENABLED

int theAssertCount = 0;

static void locLogAssertExternal(const char*, int, char const*, MSF_StringFormat const&)
{
}

static bool locDoAssert()
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

DEFINE_TEST_G(FormatFailureAsserts, MSF_Errors)
{
	MSF_CustomPrint::SetLocalErrorMode(MSF_ErrorMode::Assert);
	MSF_LogAssertExternal = &locLogAssertExternal;
	MSF_DoAssert = &locDoAssert;

	TestFormatResultAsserts("{0", 1);
	TestFormatResultAsserts("{1}", 1);
	TestFormatResultAsserts("%f %f", 1.f);

	{
		char tmp[16] = {};
		TEST_LESS(MSF_FormatString(MSF_MakeStringFormat("{0}{0}{0}{0}", "Make Me Super Long, yeah"), tmp, 16, 0, [](char*, size_t, void*) -> char* { return nullptr; }), 0);
		TEST_EQ(tmp[0], 0);
		TEST_EQ(theAssertCount, 1);
		theAssertCount = 0;
	}

	MSF_CustomPrint::ClearLocalErrorMode();

	// Allocation errors will assert in write mode as well since there's no other way to communicate the failure (initial string buffer could be 0 or deallocated
	MSF_CustomPrint::SetLocalErrorMode(MSF_ErrorMode::WriteString);
	{
		char tmp[16] = {};
		TEST_LESS(MSF_FormatString(MSF_MakeStringFormat("{0}{0}{0}{0}", "Make Me Super Long, yeah"), tmp, 16, 0, [](char*, size_t, void*) -> char* { return nullptr; }), 0);
		TEST_EQ(tmp[0], 0);
		TEST_EQ(theAssertCount, 1);
		theAssertCount = 0;
	}
	MSF_CustomPrint::ClearLocalErrorMode();

	// Silent remains silent though
	MSF_CustomPrint::SetLocalErrorMode(MSF_ErrorMode::Silent);
	{
		char tmp[16] = {};
		TEST_LESS(MSF_FormatString(MSF_MakeStringFormat("{0}{0}{0}{0}", "Make Me Super Long, yeah"), tmp, 16, 0, [](char*, size_t, void*) -> char* { return nullptr; }), 0);
		TEST_EQ(tmp[0], 0);
		TEST_EQ(theAssertCount, 0);
		theAssertCount = 0;
	}
	MSF_CustomPrint::ClearLocalErrorMode();

	MSF_DoAssert = nullptr;
	MSF_LogAssertExternal = nullptr;
}

#endif // MSF_ASSERTS_ENABLED