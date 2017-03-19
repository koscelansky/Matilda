#pragma once

#include <stdexcept>
#include <ostream>

namespace sc
{
    enum class Color
    {
        Invalid = 0,
        White = 'W',
        Black = 'B',
    };

    enum class Type
    {
        Invalid = 0,
        Man = 'M',
        King = 'K',
    };

    class Piece
    {
    public:
        Piece() = default;

        Piece(Color color, Type type)
            : m_color(color)
            , m_type(type)
        {
            if ((color == Color::Invalid && type != Type::Invalid)
                || (color != Color::Invalid && type == Type::Invalid))
                throw std::invalid_argument("Whole piece should be either valid or not.");
        }

        const Color& color() const { return m_color; }

        const Type& type() const { return m_type; }

        Piece get_promoted() const
        {
            if (m_type == Type::King)
                throw std::runtime_error("Kings cannot be promoted.");

            return Piece(m_color, Type::King);
        }

        explicit operator bool() const
        {
            return m_type != Type::Invalid;
        }
    private:
        Color m_color = Color::Invalid;
        Type m_type = Type::Invalid;
    };

    inline std::ostream& operator<<(std::ostream& lhs, const Piece& rhs)
    {
        lhs << static_cast<char>(rhs.color());
        lhs << static_cast<char>(rhs.type());
        return lhs;
    }
}