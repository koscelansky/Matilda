#pragma once

#include <regex>
#include <string>
#include <vector>
#include <array>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <assert.h>

#include "piece.h"
#include "move.h"

namespace sc
{

    enum class Winner
    {
        Undefined,
        White,
        Black,
        Draw,
    };

    namespace detail
    {
        enum class Direction : uint32_t
        {
            Invalid = 0,
            Begin = 1, // help iteration

            NE = 1 << 0, // up right
            SE = 1 << 1, // down right
            SW = 1 << 2, // down left
            NW = 1 << 3, // up left

            End, // mark last direction to help iteration
        };

        // one side of the checkers board
        constexpr size_t BOARD_SIZE = 8;

        // number of black squares, squared where pieces can be
        constexpr size_t SQUARES_COUNT = BOARD_SIZE * BOARD_SIZE / 2;

		// invalid position on board
		constexpr size_t INVALID_POS = std::numeric_limits<size_t>::max();
		/*
        // represent one board position as densely as possible 
        // (OK there are maybe some redundant bits, but it will 
        // be mega complicated to use it in 'optimal' way)
        class BoardState
        {
        public:
            BoardState() 
                : m_valid_pos(0xff0000ff)
                , m_player_colors(0x000000ff)
                , m_piece_type(0x00000000)
            {}

        private:
            uint32_t m_valid_pos; // valid - 1, invalid - 0
            uint32_t m_player_colors; // white - 1, black - 0
            uint32_t m_piece_type; // kings - 1, men - 0
        };
		*/
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

        const PieceColor& next_player() const { return m_player; }

        void perform_move(Move move);

        const std::vector<Move>& get_moves() const { return m_next_moves; }

        bool game_ended() const { return m_winner != Winner::Undefined; }

        Winner get_winner() const { return m_winner; }

    private:
        friend class SlovakCheckersSolver;
        friend std::ostream& operator<<(std::ostream& lhs, const SlovakCheckersBoard& board);

        std::vector<Move> get_moves_internal_() const;

        std::vector<std::vector<size_t>> get_captures_rec_(size_t square, Piece piece, std::vector<size_t> enemies, detail::Direction direction) const;

        std::vector<Move> get_captures_for_type_(PieceType type) const;

        std::vector<Move> get_simple_moves_() const;

		std::string get_fen_for_player(PieceColor player) const;

        std::array<Piece, detail::BOARD_SIZE * detail::BOARD_SIZE / 2> m_pieces;
        std::vector<Move> m_next_moves;
        PieceColor m_player = PieceColor::White;
        Winner m_winner = Winner::Undefined;
    };

    std::ostream& operator<<(std::ostream& lhs, const SlovakCheckersBoard& board);

    class SlovakCheckersSolver
    {
    public:
        SlovakCheckersSolver(PieceColor identity)
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

                auto score = get_best_move_internal_(new_board, 5);
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
            int my_men = 0, enemy_men = 0;
            int my_kings = 0, enemy_kings = 0;

            for (const auto& i : board.m_pieces)
            {
                if (!i)
                    continue;

                if (i.color() == m_identity)
                {
                    if (i.type() == PieceType::Man)
                    {
                        ++my_men;
                    }
                    else
                    {
                        ++my_kings;
                    }
                }
                else
                {
                    if (i.type() == PieceType::Man)
                    {
                        ++enemy_men;
                    }
                    else
                    {
                        ++enemy_kings;
                    }
                }
            }

            return (my_men)+(my_kings * 5) - (enemy_men)-(enemy_kings * 5);
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

        PieceColor m_identity;
    };
}