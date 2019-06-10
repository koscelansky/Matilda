#include <gtest/gtest.h>

#include "../../Matilda/"

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