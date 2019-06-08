#pragma once

#include "common.h"
#include "../piece.h"

#include <stdint.h>
#include <optional>

namespace SlovakCheckers
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

            bool IsPieceAt(uint8_t position) const
            {
                return m_valid_pos.test(position);
            }

            std::optional<Piece> GetPiece(uint8_t position) const
            {
				if (!IsPieceAt(position))
					return std::nullopt;

				return (*this)[position];
            }

			bool IsAt(uint8_t position, Color color) const
			{
				return m_valid_pos.test(position) 
					&& m_player_colors[position] == static_cast<bool>(color);
			}

			bool IsAt(uint8_t position, Type type) const
			{
				return m_valid_pos.test(position)
					&& m_piece_type[position] == static_cast<bool>(type);
			}

			bool IsAt(uint8_t position, Color color, Type type) const
			{
				return m_valid_pos.test(position)
					&& m_player_colors[position] == static_cast<bool>(color)
					&& m_piece_type[position] == static_cast<bool>(type);
			}

            void SetPiece(uint8_t position, std::optional<Piece> piece)
            {
				if (piece)
				{
					m_valid_pos.set(position, true);
					m_player_colors.set(position, piece->color() == Color::White);
					m_piece_type.set(position, piece->type() == Type::King);
				}
				else
				{
					ResetPiece(position);
				}
            }

			void ResetPiece(uint8_t position)
			{
				m_valid_pos.set(position, false);
			}

			void Promote(uint8_t position)
			{
				assert(!m_piece_type[position] && "Cannot promote king.");
				assert(position < 28 && position >= 4 && "Invalid position for promotion.");

				m_piece_type.set(position);
			}

			BitBoard GetPieces(Color color) const
			{
				BitBoard result = m_valid_pos; // somebody is there

				switch (color) // mask right color
				{
				case Color::White:
					result &= m_player_colors;
					break;
				case Color::Black:
					result &= ~m_player_colors;
					break;
				}

				return result;
			}

            BitBoard GetPieces(Color color, Type type) const
            {
				BitBoard result = GetPieces(color);

                switch (type) // mask right type 
                {
                case Type::Man:
                    result &= ~m_piece_type;
                    break;
                case Type::King:
                    result &= m_piece_type;
                    break;
                }

                return result;
            }

            size_t GetPiecesCount(Color color, Type type) const
            {
                return GetPieces(color, type).count();
            }

			bool HasPieces(Color color) const
			{
				return GetPieces(color).any();
			}

			// this function assume, that there is indeed a piece on position
			// if not, then you will receive black man, and that is probably 
			// wrong and you should not do it
            Piece operator[](uint8_t position) const
            {
				assert(IsPieceAt(position));

				auto color = m_player_colors.test(position) ? Color::White : Color::Black;
				auto type = m_piece_type.test(position) ? Type::King : Type::Man;

				return Piece(color, type);
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
	template <> struct hash<SlovakCheckers::detail::BoardState>
	{
		size_t operator()(const SlovakCheckers::detail::BoardState& x) const
		{
			using SlovakCheckers::detail::BitBoard;
			std::hash<BitBoard> h;		

			return h(x.m_valid_pos) ^ h(x.m_valid_pos) ^ h(x.m_valid_pos);
		}
	};
}