#pragma once

#include <boost/container/static_vector.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <stdint.h>
#include <stdexcept>
#include <vector>

namespace SlovakCheckers
{
    enum class MoveType : int8_t
    {
        SimpleMove = 0,
        Jump = 1,
    };

	namespace detail
	{
		inline const char* MoveTypeToSeparator(MoveType type)
		{
			switch (type)
			{
			case MoveType::SimpleMove: return "-";
			case MoveType::Jump: return "x";
			default: throw std::invalid_argument("Invalid move type.");
			}
		}
	}

	using MoveVector = boost::container::static_vector<uint8_t, 8>;

    class Move
    {
    public:
        Move(MoveVector steps, MoveType type)
            : m_steps(std::move(steps))
            , m_type(type)
        {
            if (m_steps.size() <= 1)
                throw std::invalid_argument("Move should have at least two steps.");

            if (m_type == MoveType::SimpleMove && m_steps.size() != 2)
                throw std::invalid_argument("SimpleMove can only have two steps.");
        }

        MoveType GetType() const { return m_type; }

        const MoveVector& GetSteps() const { return m_steps; }

    private:
		MoveVector m_steps;
        MoveType m_type;
    };

    inline std::ostream& operator<<(std::ostream& lhs, const Move& rhs)
    {
		auto steps = rhs.GetSteps() | boost::adaptors::transformed([](uint8_t x) { return std::to_string(x + 1); });

		return lhs << boost::join(steps, detail::MoveTypeToSeparator(rhs.GetType()));
    }
}