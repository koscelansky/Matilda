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

namespace SlovakCheckers
{
    namespace detail
    {
        inline Color Opponent(Color player)
        {
            switch (player)
            {
            case Color::White:
                return Color::Black;
            case Color::Black:
                return Color::White;
            }

			throw std::runtime_error("Opponent exists only for valid player.");
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
		Board() = default;

        Board(const std::string& fen);

		/**
		* White will alway be first in the FEN. So this one can be produced.
		* W:W10:B18, but the followong one cannot be W:B18:W10. Also the pieces
		* are reported in square number order, so B1,2,3 is possible, but 
		* B2,1,3 is not. 
		*/
        std::string GetFen() const;

        Color NextTurn() const { return m_player; }

		std::vector<Move> GetMoves() const { return GetMoves_(); }

        void perform_move(const Move& move);

		size_t GetStateHash() const;

    private:
        friend class Player;
        friend std::ostream& operator<<(std::ostream& lhs, const Board& board);

        std::vector<Move> GetMoves_() const;

        std::vector<MoveVector> GetCapturesRec_(uint8_t square, Piece piece, detail::BitBoard enemies, detail::Direction direction) const;

        std::vector<Move> GetCapturesForType_(Type type) const;

        std::vector<Move> get_simple_moves_() const;

		detail::BoardState m_board{ detail::kBoardStart };
		Color m_player = Color::White;
    };

    std::ostream& operator<<(std::ostream& lhs, const Board& board);
}