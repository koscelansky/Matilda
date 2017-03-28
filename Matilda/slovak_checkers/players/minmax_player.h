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

		void PerformMove(Board& board) override
		{
			if (m_verbose == Verbose::On)
			{
				m_out << board;
			}

			board.perform_move(get_best_move(board));
		}

		Move get_best_move(const Board& board) const
		{
			bool is_max_pass = board.next_player() == Color::White;
			double best_score = is_max_pass ? BLACK_IS_WINNER : WHITE_IS_WINNER;

			const std::vector<Move>& moves = board.get_moves();
			auto best_move = moves.front();
			for (const auto& i : moves)
			{
				auto new_board = board;
				new_board.perform_move(i);

				auto score = get_best_move_internal_(new_board, 6);
				if (is_max_pass && score > best_score)
				{
					best_score = score;
					best_move = i;
				}

				if (!is_max_pass && score < best_score)
				{
					best_score = score;
					best_move = i;
				}
			}

			return best_move;
		}

	private:
		static constexpr double BLACK_IS_WINNER = -std::numeric_limits<double>::max();
		static constexpr double WHITE_IS_WINNER = std::numeric_limits<double>::max();

		static constexpr double KING_VALUE = 5;

		double evaluate_board_(const Board& board) const
		{
			size_t white_kings = GetBoardState(board).GetPiecesCount(Color::White, Type::King);
			size_t black_kings = GetBoardState(board).GetPiecesCount(Color::Black, Type::King);

			auto piece_value = white_kings * KING_VALUE;
			piece_value -= black_kings * 5;

			// form white perspectives
			static const double pawn_weight[] =
			{
				      1.3,     1.05,      1.3,    1.05,
				 1.1,        1,        1,       1,
				        1,        1,        1,       1,
				   1,     1.05,     1.05,       1,
				        1,     1.05,     1.05,       1,
				   1,        1,        1,       1,
				        1,        1,        1,       1,
				   1,        1,        1,       1,
			};

			auto white_men = GetBoardState(board).GetPieces(Color::White, Type::Man);
			for (size_t i = 0; i < SQUARES_COUNT; ++i)
			{
				if (!white_men.test(i))
					continue;

				piece_value += pawn_weight[i];
			}

			auto black_men = GetBoardState(board).GetPieces(Color::Black, Type::Man);
			for (size_t i = 0; i < SQUARES_COUNT; ++i)
			{
				if (!black_men.test(i))
					continue;

				piece_value -= pawn_weight[std::size(pawn_weight) - 1 - i];
			}

			return piece_value;
		}

		double get_best_move_internal_(const Board& board, int depth) const
		{
			auto result = board.get_result();
			if (result == GameResult::WhiteWon)
				return WHITE_IS_WINNER;
			else if (result == GameResult::BlackWon)
				return BLACK_IS_WINNER;
			else if (result == GameResult::Draw)
				return 0;

			// game has not ended yet 

			if (depth == 0)
			{
				return evaluate_board_(board);
			}

			const auto& possible_moves = board.get_moves();
			if (possible_moves.size() == 1)
			{
				// forced move, do not decrease search depth
				auto new_board = board;
				new_board.perform_move(possible_moves.front());

				return get_best_move_internal_(new_board, depth);
			}

			// when current player is the player then we want to maximize, otherwise minimize
			bool is_max_pass = board.next_player() == Color::White;

			double best_score = is_max_pass ? BLACK_IS_WINNER : WHITE_IS_WINNER;

			for (const auto& i : possible_moves)
			{
				auto new_board = board;
				new_board.perform_move(i);

				auto score = get_best_move_internal_(new_board, depth - 1);
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