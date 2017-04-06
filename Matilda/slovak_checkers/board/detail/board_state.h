#pragma once

#include "common.h"
#include "../piece.h"

namespace sc
{
    namespace detail
    {
		// forward declaration for defining friends later
		class BoardState;
		template<> class std::hash<BoardState>;

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

                auto color = m_player_colors.test(position) ? Color::White : Color::Black;
                auto type = m_piece_type.test(position) ? Type::King : Type::Man;

                return Piece(color, type);
            }

            void SetPiece(size_t position, Piece piece)
            {
                m_valid_pos.set(position, bool(piece));
                m_player_colors.set(position, piece.color() == Color::White);
                m_piece_type.set(position, piece.type() == Type::King);
            }

			void ResetPiece(size_t position)
			{
				m_valid_pos.set(position, false);
			}

			void Promote(size_t position)
			{
				assert(!m_piece_type[position] && "Cannot promote king.");
				assert(position < 28 && position >= 4 && "Invalid position for promotion.");

				m_piece_type.set(position);
			}

            BitBoard GetPieces(Color color, Type type = Type::Invalid) const
            {
                BitBoard ret_val = m_valid_pos; // somebody is there

                switch (color) // mask right color
                {
                case Color::White:
                    ret_val &= m_player_colors;
                    break;
                case Color::Black:
                    ret_val &= ~m_player_colors;
                    break;
                }

                switch (type) // mask right type 
                {
                case Type::Man:
                    ret_val &= ~m_piece_type;
                    break;
                case Type::King:
                    ret_val &= m_piece_type;
                    break;
                }

                return ret_val;
            }

            size_t GetPiecesCount(Color color, Type type = Type::Invalid) const
            {
                return GetPieces(color, type).count();
            }

			bool HasPieces(Color color) const
			{
				return GetPieces(color, Type::Invalid).any();
			}

            Piece operator[](size_t position) const
            {
                return GetPiece(position);
            }

        private:
            BitBoard m_valid_pos; // valid - 1, invalid - 0
            BitBoard m_player_colors; // white - 1, black - 0
            BitBoard m_piece_type; // kings - 1, men - 0

			friend struct std::hash<BoardState>;
        };
    }
}

// specialize std::hash for board state
namespace std 
{
	template <> struct hash<sc::detail::BoardState>
	{
		size_t operator()(const sc::detail::BoardState& x) const
		{
			using sc::detail::BitBoard;
			std::hash<BitBoard> h;		

			return h(x.m_valid_pos) ^ h(x.m_valid_pos) ^ h(x.m_valid_pos);
		}
	};
}