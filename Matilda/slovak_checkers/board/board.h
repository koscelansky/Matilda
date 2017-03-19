#pragma once

#include <regex>
#include <string>
#include <vector>
#include <array>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <assert.h>
#include <bitset>

#include "detail/common.h"
#include "detail/board_state.h"
#include "detail/direction.h"

#include "piece.h"
#include "move.h"

namespace sc
{
	using namespace detail;

    enum class Winner
    {
        Undefined,
        White,
        Black,
        Draw,
    };

    namespace detail
    {
        inline Color opponent(Color player)
        {
            switch (player)
            {
            case Color::White:
                return Color::Black;
            case Color::Black:
                return Color::White;
            }

            throw std::invalid_argument("Opponent exists only for valid player.");
        }		
    }

    /**
    * Represents one state of board in Slovak checkers.
    *
    * Board on the start looks like
    *
    * |----|----|----|----|----|----|----|----|
    * |    |  1 |    |  2 |    |  3 |    |  4 |
    * |----|----|----|----|----|----|----|----|
    * |  5 |    |  6 |    |  7 |    |  8 |    |
    * |----|----|----|----|----|----|----|----|
    * |    |  9 |    | 10 |    | 11 |    | 12 |
    * |----|----|----|----|----|----|----|----|
    * | 13 |    | 14 |    | 15 |    | 16 |    |
    * |----|----|----|----|----|----|----|----|
    * |    | 17 |    | 18 |    | 19 |    | 20 |
    * |----|----|----|----|----|----|----|----|
    * | 21 |    | 22 |    | 23 |    | 24 |    |
    * |----|----|----|----|----|----|----|----|
    * |    | 25 |    | 26 |    | 27 |    | 28 |
    * |----|----|----|----|----|----|----|----|
    * | 29 |    | 30 |    | 31 |    | 32 |    |
    * |----|----|----|----|----|----|----|----|
    *
    * White occupies positions 1 - 8 and black 25 - 32. White's men move 'down'
    * (always to the higher number), black's men move 'up' (always to the lower
    * number).
    */
    class SlovakCheckersBoard
    {
    public:
        SlovakCheckersBoard();

        SlovakCheckersBoard(const std::string& serialized);

        std::string get_fen() const;

        const Color& next_player() const { return m_player; }

        void perform_move(Move move);

        const std::vector<Move>& get_moves() const { return m_next_moves; }

        bool game_ended() const { return m_winner != Winner::Undefined; }

        Winner get_winner() const { return m_winner; }

    private:
        friend class SlovakCheckersSolver;
        friend std::ostream& operator<<(std::ostream& lhs, const SlovakCheckersBoard& board);

        std::vector<Move> get_moves_internal_() const;

        std::vector<std::vector<size_t>> get_captures_rec_(size_t square, Piece piece, BitBoard enemies, Direction direction) const;

        std::vector<Move> get_captures_for_type(Type type) const;

        std::vector<Move> get_simple_moves_() const;

        BoardState m_board;
        std::vector<Move> m_next_moves;
        Color m_player = Color::White;
        Winner m_winner = Winner::Undefined;
    };

    std::ostream& operator<<(std::ostream& lhs, const SlovakCheckersBoard& board);

	class SlovakCheckersSolver
	{
	public:
		SlovakCheckersSolver(Color identity)
			: m_identity(identity)
		{
		}

		Move get_best_move(const SlovakCheckersBoard& board) const
		{
			bool is_max_pass = m_identity == board.next_player();

			double best_score = is_max_pass ? ENEMY_IS_WINNER : PLAYER_IS_WINNER;

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
		static constexpr double ENEMY_IS_WINNER = -std::numeric_limits<double>::max();
		static constexpr double PLAYER_IS_WINNER = std::numeric_limits<double>::max();

		double evaluate_board_(const SlovakCheckersBoard& board) const
		{
			size_t my_kings = board.m_board.GetPiecesCount(m_identity, Type::King);
			size_t enemy_kings = board.m_board.GetPiecesCount(opponent(m_identity), Type::King);

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

			auto white_men = board.m_board.GetPieces(Color::White, Type::Man);
			for (size_t i = 0; i < SQUARES_COUNT; ++i)
			{
				if (!white_men.test(i))
					continue; 

				piece_value += white_weight[i] * (m_identity == Color::White ? 1 : -1);
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

			auto black_men = board.m_board.GetPieces(Color::Black, Type::Man);
			for (size_t i = 0; i < SQUARES_COUNT; ++i)
			{
				if (!black_men.test(i))
					continue;

				piece_value += black_weight[i] * (m_identity == Color::White ? -1 : 1);
			}

			return piece_value;
		}

		double get_best_move_internal_(const SlovakCheckersBoard& board, int depth) const
		{
			if (depth == 0)
			{
				return evaluate_board_(board);
			}

			// when current player is the player then we want to maximize, otherwise minimize
			bool is_max_pass = m_identity == board.next_player();

			double best_score = is_max_pass ? ENEMY_IS_WINNER : PLAYER_IS_WINNER;
			for (const auto& i : board.get_moves())
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

		Color m_identity;
	};
}