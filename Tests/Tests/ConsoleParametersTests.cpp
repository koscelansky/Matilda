#include <gtest/gtest.h>

#include "../../Matilda/console_params.h"

TEST(ConsoleParameters, NoParams)
{
	char* argv[] =
	{
		(char*)"dummpy program path",
	};
	int argc = std::size(argv);

	ConsoleParams cp(argc, argv);

	ASSERT_FALSE(cp.IsHelpPrintEnabled());
	ASSERT_FALSE(cp.HasFen());
	ASSERT_ANY_THROW(cp.GetFen(););
}

TEST(ConsoleParameters, HelpLong)
{
	char* argv[] =
	{
		(char*)"dummpy program path",
		(char*)"--help",
	};
	int argc = std::size(argv);

	ConsoleParams cp(argc, argv);

	ASSERT_TRUE(cp.IsHelpPrintEnabled());
	ASSERT_FALSE(cp.HasFen());
	ASSERT_ANY_THROW(cp.GetFen(););
}

TEST(ConsoleParameters, HelpShort)
{
	char* argv[] =
	{
		(char*)"dummpy program path",
		(char*)"-H",
	};
	int argc = std::size(argv);

	ConsoleParams cp(argc, argv);

	ASSERT_TRUE(cp.IsHelpPrintEnabled());
	ASSERT_FALSE(cp.HasFen());
	ASSERT_ANY_THROW(cp.GetFen(););
}