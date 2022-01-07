#include "Tests/MSF_UnitTests.h"
#include "MSF_FormatPrint.h"
#include <string.h>

bool MSF_UnitTestFailureBreak;
int MSF_AccuracyTestIterations;

int main(int argc, char* argv[])
{
	char const* groupFilter = nullptr;
	char const* testFilter = nullptr;
	for (int i = 1; i < argc; ++i)
	{
		char const* line = argv[i];
		if (line[0] == '/') ++line;
		else if (line[0] == '-' && line[1] == '-') line += 2;
		else if (line[0] == '-') ++line;

		// Any unit test failure will generate an assert and break execution where it failed
		if (strcmp(line, "break") == 0)
		{
			MSF_UnitTestFailureBreak = true;
		}
		// Any string errors will generate an assert by default (unless overidden by a specific test)
		else if (strcmp(line, "assert") == 0)
		{
			MSF_CustomPrint::SetGlobalErrorMode(MSF_CustomPrint::Assert);
		}
		// Perform the same number of accuracy tests in debug that release uses
		else if (strcmp(line, "all") == 0)
		{
			MSF_AccuracyTestIterations = 10;
		}
		else if (strncmp(line, "group:", strlen("group:")) == 0)
		{
			groupFilter = line + strlen("group:");
		}
		else if (strncmp(line, "test:", strlen("test:")) == 0)
		{
			testFilter = line + strlen("test:");
		}
	}

	TestFixture::ExecuteAllTests(groupFilter, testFilter, TestFixture::Normal);
}
