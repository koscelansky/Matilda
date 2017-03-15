#include <iostream>

#include "slovak_checkers/board.h"
#include "console_params.h"
#include "console_player.h"

#include <stdlib.h>


int main(int argc, char* argv[])
{
	ConsoleParams cp(argc, argv);

	if (cp.IsHelpPrintEnabled())
	{
		cp.PrintHelp(std::cout);
		return 0;
	}

    sc::SlovakCheckersBoard scb;

	ConsolePlayer human(std::cin, std::cout);

	while (true)
	{
		std::cout << scb;
		auto v = scb.get_moves();

		std::cout << scb.get_fen() << std::endl;

		if (scb.game_ended())
			break;

		if (scb.next_player() == sc::Color::White)
		{
			human.PerformMove(scb);
		}
		else
		{
			scb.perform_move(sc::SlovakCheckersSolver(sc::Color::Black).get_best_move(scb));
		}
	}

	getchar();

	return 0; 
}
