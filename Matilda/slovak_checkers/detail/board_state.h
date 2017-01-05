#pragma once

#include "common.h"

namespace sc
{
    namespace detail
    {
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
}