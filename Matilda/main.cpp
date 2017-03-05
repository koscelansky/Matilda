#include <iostream>

#include "slovak_checkers/board.h"
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


    sc::SlovakCheckersBoard scb;

	while (true)
	{
		std::cout << scb;
		auto v = scb.get_moves();

		if (scb.game_ended())
			break;

		std::cout << scb.get_fen() << std::endl;

		for (int i = 0; i < v.size(); ++i)
		{
			std::cout << i << ": " << v[i] << "   ";
		}

		std::cout << std::endl;

		if (scb.next_player() == sc::Color::White)
		{
			//scb.perform_move(sc::SlovakCheckersSolver(sc::Color::White).get_best_move(scb));
			
			int i;
			std::cin >> i;
			scb.perform_move(v[i]);
		}
		else
		{
			scb.perform_move(sc::SlovakCheckersSolver(sc::Color::Black).get_best_move(scb));
		}
	}

	getchar();

	return 0; 
}
