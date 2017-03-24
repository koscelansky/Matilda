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
        friend class Player;
        friend std::ostream& operator<<(std::ostream& lhs, const SlovakCheckersBoard& board);

        std::vector<Move> get_moves_internal_() const;

        std::vector<move_vector> get_captures_rec_(size_t square, Piece piece, BitBoard enemies, Direction direction) const;

        std::vector<Move> get_captures_for_type(Type type) const;

        std::vector<Move> get_simple_moves_() const;

        BoardState m_board;
        std::vector<Move> m_next_moves;
        Color m_player = Color::White;
        Winner m_winner = Winner::Undefined;
    };

    std::ostream& operator<<(std::ostream& lhs, const SlovakCheckersBoard& board);
}