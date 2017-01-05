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

        inline PieceColor opponent(PieceColor player)
        {
            switch (player)
            {
            case PieceColor::White:
                return PieceColor::Black;
            case PieceColor::Black:
                return PieceColor::White;
            }

            throw std::invalid_argument("Opponent exists only for valid player.");
        }

        // one side of the checkers board
        const constexpr size_t BOARD_SIZE = 8;

        // number of black squares, squared where pieces can be
        const constexpr size_t SQUARES_COUNT = BOARD_SIZE * BOARD_SIZE / 2;

		// invalid position on board
        const constexpr size_t INVALID_POS = std::numeric_limits<size_t>::max();
		
        using BitBoard = std::bitset<SQUARES_COUNT>;

        // tag for constructing BoardState with starting position
        struct board_start_t { };
        const constexpr board_start_t board_start;

        // tag for constructing empty BoardState 
        struct board_empty_t { };
        const constexpr board_empty_t board_empty;

        // represent one board position as densely as possible 
        // (OK there are maybe some redundant bits, but it will 
        // be mega complicated to use it in 'optimal' way)
        class BoardState
        {
        public:
            BoardState(board_empty_t) { }

            BoardState(board_start_t)
                : m_valid_pos(0xff0000ff)
                , m_player_colors(0x000000ff)
                , m_piece_type(0x00000000)
            {}

            bool IsPieceAt(size_t position) const
            {
                return m_valid_pos.test(position);
            }

            Piece GetPiece(size_t position) const
            {
                if (!IsPieceAt(position))
                    return Piece();

                auto color = m_player_colors.test(position) ? PieceColor::White : PieceColor::Black;
                auto type = m_piece_type.test(position) ? PieceType::King : PieceType::Man;

                return Piece(color, type);
            }

            void SetPiece(size_t position, Piece piece)
            {
                m_valid_pos.set(position, bool(piece));
                m_player_colors.set(position, piece.color() == PieceColor::White);
                m_piece_type.set(position, piece.type() == PieceType::King);
            }

            BitBoard GetPieces(PieceColor color, PieceType type = PieceType::Invalid) const
            {
                BitBoard ret_val = m_valid_pos; // somebody is there

                switch (color) // mask right color
                {
                case PieceColor::White:
                    ret_val &= m_player_colors;
                    break;
                case PieceColor::Black:
                    ret_val &= ~m_player_colors;
                    break;
                }

                switch (type) // mask right type 
                {
                case PieceType::Man:
                    ret_val &= ~m_piece_type;
                    break;
                case PieceType::King:
                    ret_val &= m_piece_type;
                    break;
                }

                return ret_val;
            }

            size_t GetPiecesCount(PieceColor color, PieceType type = PieceType::Invalid) const
            {
                return GetPieces(color, type).count();
            }

            Piece operator[](size_t position) const
            {
                return GetPiece(position);
            }

        private:
            BitBoard m_valid_pos; // valid - 1, invalid - 0
            BitBoard m_player_colors; // white - 1, black - 0
            BitBoard m_piece_type; // kings - 1, men - 0
        };
		
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

        std::vector<std::vector<size_t>> get_captures_rec_(size_t square, Piece piece, detail::BitBoard enemies, detail::Direction direction) const;

        std::vector<Move> get_captures_for_type(PieceType type) const;

        std::vector<Move> get_simple_moves_() const;

        detail::BoardState m_board;
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
            using namespace detail;

            int my_men = board.m_board.GetPiecesCount(m_identity, PieceType::Man);
            int enemy_men = board.m_board.GetPiecesCount(opponent(m_identity), PieceType::Man);
            int my_kings = board.m_board.GetPiecesCount(m_identity, PieceType::King); 
            int enemy_kings = board.m_board.GetPiecesCount(opponent(m_identity), PieceType::King);

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