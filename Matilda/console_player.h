#pragma once

#include "slovak_checkers/player.h"
#include "slovak_checkers/board.h"

#include <ostream>
#include <istream>

class ConsolePlayer : public sc::Player
{
public:
	ConsolePlayer(std::istream& input, std::ostream& output)
		: m_out(output)
		, m_in(input) { }

	void PerformMove(sc::SlovakCheckersBoard& board) override
	{
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