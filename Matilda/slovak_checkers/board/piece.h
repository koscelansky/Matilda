#pragma once

#include <stdint.h>
#include <stdexcept>
#include <ostream>

namespace SlovakCheckers
{
    enum class Color : int8_t
    {
		Black = 0,
        White = 1,
    };

    enum class Type : int8_t
    {
        Man = 0,
        King = 1,
    };

	namespace detail
	{
		inline Color GetColorFromChar(char c)
		{
			switch (c)
			{
			case 'W': return Color::White;
			case 'B': return Color::Black;
			default: throw std::invalid_argument("Unsupported color specified");
			}
		}
	}

    class Piece
    {
    public:
        Piece(Color color, Type type)
            : m_color(color)
            , m_type(type)
        {
        }

        const Color& color() const { return m_color; }

        const Type& type() const { return m_type; }
    private:
        Color m_color;
        Type m_type;
    };

    inline std::ostream& operator<<(std::ostream& lhs, const Piece& rhs)
    {
        lhs << (rhs.color() == Color::Black ? 'B' : 'W');
        lhs << (rhs.type() == Type::Man ? 'M' : 'K');
        return lhs;
    }
}
