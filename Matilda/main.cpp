#include <iostream>

#include "console_params.h"
#include "slovak_checkers/board/board.h"

#include <stdlib.h>

int main(int argc, char* argv[])
{
	ConsoleParams cp(argc, argv);

	if (cp.IsHelpPrintEnabled())
	{
		cp.PrintHelp(std::cout);
		return 0;
	}

	SlovakCheckers::Board game(cp.GetFen());
	std::cout << game << std::endl;

	return 0; 
}
