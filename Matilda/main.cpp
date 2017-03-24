#include <iostream>

#include "slovak_checkers/game.h"
#include "console_params.h"

#include <stdlib.h>


int main(int argc, char* argv[])
{
	ConsoleParams cp(argc, argv);

	if (cp.IsHelpPrintEnabled())
	{
		cp.PrintHelp(std::cout);
		return 0;
	}

	sc::Game::Config config;
	config.verbose = cp.IsVerbose();

	sc::Game game(cp.GetWhitePlayer(), cp.GetBlackPlayer(), config);
	game.Run();

	getchar();

	return 0; 
}
