#pragma once

#include <boost/container/small_vector.hpp>

#include <stdexcept>
#include <vector>

namespace sc
{
    enum class MoveType
    {
        SimpleMove = '-',
        Jump = 'x',
    };

	using move_vector = boost::container::small_vector<size_t, 2>;

    class Move
    {
    public:
        Move(move_vector steps, MoveType type)
            : m_steps(std::move(steps))
            , m_type(type)
        {
            if (m_steps.size() <= 1)
                throw std::invalid_argument("Move should have at least two steps.");

            if (m_type == MoveType::SimpleMove && m_steps.size() != 2)
                throw std::invalid_argument("SimpleMove can only have two steps.");
        }

        const MoveType& type() const { return m_type; }

        const boost::container::small_vector_base<size_t>& steps() const { return m_steps; }

    private:
		move_vector m_steps;
        MoveType m_type;
    };

    inline std::ostream& operator<<(std::ostream& lhs, const Move& rhs)
    {
        char move_steps_separator = static_cast<char>(rhs.type());

        std::string ret_val;

        const auto& steps = rhs.steps();
        lhs << steps.front() + 1;
        for (auto it = std::begin(steps) + 1; it != std::end(steps); ++it)
        {
            lhs << move_steps_separator;
            lhs << *it + 1;
        }

        return lhs;
    }
}