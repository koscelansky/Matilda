#pragma once

#include "player.h"
#include "../board/board.h"

#include <iostream>

namespace sc
{
	class ConsolePlayer : public Player
	{
	public:
		ConsolePlayer(std::istream& input = std::cin, std::ostream& output = std::cout)
			: m_out(output)
			, m_in(input) { }

		void PerformMove(SlovakCheckersBoard& board) override
		{
			m_out << board;

			auto moves = board.get_moves();

			m_out << "Possible moves: " << std::endl;
			for (size_t i = 0; i < moves.size(); ++i)
			{
				m_out << i << ": " << moves[i] << " ";
			}
			m_out << std::endl;

			int option;
			m_in >> option;
			board.perform_move(moves[option]);

			m_out << std::endl;
		}

	private:
		std::ostream& m_out;
		std::istream& m_in;
	};
}