#pragma once

#include <stdexcept>
#include <ostream>

namespace sc
{
    enum class PieceColor
    {
        Invalid = 0,
        White = 'W',
        Black = 'B',
    };

    enum class PieceType
    {
        Invalid = 0,
        Man = 'M',
        King = 'K',
    };

    class Piece
    {
    public:
        Piece() = default;

        Piece(PieceColor color, PieceType type)
            : m_color(color)
            , m_type(type)
        {
            if ((color == PieceColor::Invalid && type != PieceType::Invalid)
                || (color != PieceColor::Invalid && type == PieceType::Invalid))
                throw std::invalid_argument("Whole piece should be either valid or not.");
        }

        const PieceColor& color() const { return m_color; }

        const PieceType& type() const { return m_type; }

        Piece get_promoted() const
        {
            if (m_type == PieceType::King)
                throw std::runtime_error("Kings cannot be promoted.");

            return Piece(m_color, PieceType::King);
        }

        explicit operator bool() const
        {
            return m_type != PieceType::Invalid;
        }
    private:
        PieceColor m_color = PieceColor::Invalid;
        PieceType m_type = PieceType::Invalid;
    };

    inline std::ostream& operator<<(std::ostream& lhs, const Piece& rhs)
    {
        lhs << static_cast<char>(rhs.color());
        lhs << static_cast<char>(rhs.type());
        return lhs;
    }
}