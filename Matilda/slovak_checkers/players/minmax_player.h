#pragma once

#include "player.h"
#include "../types.h"

#include <iostream>

namespace sc
{
	class MinMaxPlayer : public Player
	{
	public:
		MinMaxPlayer(Verbose verbose, std::ostream& output = std::cout)
			: m_verbose(verbose)
			, m_out(output) { }

		void PerformMove(SlovakCheckersBoard& board) override
		{
			if (m_verbose == Verbose::On)
			{
				m_out << board;
			}

			board.perform_move(get_best_move(board));
		}

		Move get_best_move(const SlovakCheckersBoard& board) const
		{
			double best_score = ENEMY_IS_WINNER;

			const std::vector<Move>& moves = board.get_moves();
			auto best_move = moves.front();
			for (const auto& i : moves)
			{
				auto new_board = board;
				new_board.perform_move(i);

				auto score = get_best_move_internal_(new_board, board.next_player(), 6);
				if (score > best_score)
				{
					best_score = score;
					best_move = i;
				}
			}

			return best_move;
		}

	private:
		static constexpr double ENEMY_IS_WINNER = -std::numeric_limits<double>::max();
		static constexpr double PLAYER_IS_WINNER = std::numeric_limits<double>::max();

		double evaluate_board_(const SlovakCheckersBoard& board, Color my_side) const
		{
			size_t my_kings = GetBoardState(board).GetPiecesCount(my_side, Type::King);
			size_t enemy_kings = GetBoardState(board).GetPiecesCount(opponent(my_side), Type::King);

			auto piece_value = static_cast<double>((my_kings * 5)) - (enemy_kings * 5);

			static const double white_weight[] =
			{
				1.3,  1,  1.3,  1,
				1.1,  1,  1,  1,
				1,  1, 1,  1,
				1,  1, 1, 1,
				1, 1, 1,  1,
				1, 1, 1, 1,
				1, 1, 1, 1,
				1, 1, 1, 1
			};

			auto white_men = GetBoardState(board).GetPieces(Color::White, Type::Man);
			for (size_t i = 0; i < SQUARES_COUNT; ++i)
			{
				if (!white_men.test(i))
					continue;

				piece_value += white_weight[i] * (my_side == Color::White ? 1 : -1);
			}

			static const double black_weight[] =
			{
				1,  1,  1,  1,
				1,  1,  1,  1,
				1,  1, 1,  1,
				1,  1, 1, 1,
				1, 1, 1,  1,
				1, 1, 1, 1,
				1.1, 1, 1, 1,
				1, 1.3, 1, 1.3
			};

			auto black_men = GetBoardState(board).GetPieces(Color::Black, Type::Man);
			for (size_t i = 0; i < SQUARES_COUNT; ++i)
			{
				if (!black_men.test(i))
					continue;

				piece_value += black_weight[i] * (my_side == Color::White ? -1 : 1);
			}

			return piece_value;
		}

		double get_best_move_internal_(const SlovakCheckersBoard& board, Color my_side, int depth) const
		{
			if (depth == 0)
			{
				return evaluate_board_(board, my_side);
			}

			// when current player is the player then we want to maximize, otherwise minimize
			bool is_max_pass = my_side == board.next_player();

			double best_score = is_max_pass ? ENEMY_IS_WINNER : PLAYER_IS_WINNER;
			for (const auto& i : board.get_moves())
			{
				auto new_board = board;
				new_board.perform_move(i);

				auto score = get_best_move_internal_(new_board, my_side, depth - 1);
				if (is_max_pass && score > best_score)
				{
					best_score = score;
				}

				if (!is_max_pass && score < best_score)
				{
					best_score = score;
				}
			}

			return best_score;
		}

		Verbose m_verbose;
		std::ostream& m_out;
	};
}