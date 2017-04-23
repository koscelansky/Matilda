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

    enum class GameResult
    {
        Undefined,
        WhiteWon,
        BlackWon,
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
    class Board
    {
    public:
        Board();

        Board(const std::string& serialized);

        std::string get_fen() const;

        const Color& next_player() const { return m_player; }

        void perform_move(const Move& move);

        const std::vector<Move>& get_moves() const { return m_next_moves; }

        bool game_ended() const { return m_result != GameResult::Undefined; }

        GameResult get_result() const { return m_result; }

    private:
        friend class Player;
        friend std::ostream& operator<<(std::ostream& lhs, const Board& board);

        std::vector<Move> get_moves_internal_() const;

        std::vector<move_vector> get_captures_rec_(uint8_t square, Piece piece, BitBoard enemies, Direction direction) const;

        std::vector<Move> get_captures_for_type_(Type type) const;

        std::vector<Move> get_simple_moves_() const;

		uint32_t get_state_hash_() const;

        BoardState m_board;
		Color m_player = Color::White;

		uint32_t m_reversible_moves = 0;
		std::vector<uint32_t> m_previous_states;

        std::vector<Move> m_next_moves;
        GameResult m_result = GameResult::Undefined;
    };

    std::ostream& operator<<(std::ostream& lhs, const Board& board);
}