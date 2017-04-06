#pragma once

#include "players/player.h"
#include "players/console_player.h"
#include "players/minmax_player.h"
#include "board/board.h"
#include "types.h"

#include <memory>
#include <iostream>
#include <chrono>

namespace sc
{
	class Game
	{
	public:
		struct Config
		{
			Verbose verbose = Verbose::Off;
		};

		Game(PlayerType white, PlayerType black, const Config& config)
			: m_config(config)
		{
			auto create_player = [this](PlayerType type) -> std::unique_ptr<Player>
			{
				switch (type)
				{
					case PlayerType::Human:
						return std::make_unique<ConsolePlayer>();
					case PlayerType::Computer:
						return std::make_unique<MinMaxPlayer>(m_config.verbose);
				}

				throw std::runtime_error("Unknown player type.");
			};

			m_black = create_player(black);
			m_white = create_player(white);
		}

		void Run()
		{
			auto start = std::chrono::high_resolution_clock::now();

			int i = 0;

			while (!m_board.game_ended())
			{
				m_white->PerformMove(m_board);
				
				if (m_board.game_ended())
					break;

				m_black->PerformMove(m_board);

				if (++i == 40)
					break;
			}

			// print result 
			std::cout << "Game ended, ";
			switch (m_board.get_result())
			{
				case GameResult::BlackWon:
					std::cout << "black won." << std::endl;
					break;
				case GameResult::WhiteWon:
					std::cout << "white won." << std::endl;
					break;
				case GameResult::Draw:
					std::cout << "it's a draw." << std::endl;
					break;
			}

			auto duration = std::chrono::high_resolution_clock::now() - start;
			if (m_config.verbose == Verbose::On)
			{
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

				std::cout << "Duration: " << ms << "ms." << std::endl;
			}
		}

	private:
		Board m_board;
		Config m_config;

		std::unique_ptr<Player> m_black, m_white;
	};
}