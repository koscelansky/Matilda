#pragma once

#include "players/player.h"
#include "players/console_player.h"
#include "players/minmax_player.h"
#include "board/board.h"
#include "types.h"

#include <memory>
#include <iostream>

namespace sc
{
	class Game
	{
	public:
		Game(PlayerType white, PlayerType black)
		{
			auto create_player = [](PlayerType type) -> std::unique_ptr<Player>
			{
				switch (type)
				{
					case PlayerType::Human:
						return std::make_unique<ConsolePlayer>();
					case PlayerType::Computer:
						return std::make_unique<MinMaxPlayer>(Verbose::On);
				}

				throw std::runtime_error("Unknown player type.");
			};

			m_black = create_player(black);
			m_white = create_player(white);
		}

		void Run()
		{
			while (!m_board.game_ended())
			{
				m_white->PerformMove(m_board);
				
				if (m_board.game_ended())
					break;

				m_black->PerformMove(m_board);
			}
		}

	private:
		SlovakCheckersBoard m_board;

		std::unique_ptr<Player> m_black, m_white;
	};
}