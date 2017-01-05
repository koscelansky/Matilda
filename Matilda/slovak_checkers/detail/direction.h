#pragma once

#include <stdint.h>
#include <initializer_list>

#include "common.h"
#include "../piece.h"

namespace sc
{
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

		const constexpr Direction operator|(Direction lhs, Direction rhs)
		{
			using T = std::underlying_type_t<Direction>;

			return static_cast<Direction>(static_cast<T>(lhs) | static_cast<T>(rhs));
		}

		const constexpr Direction operator&(Direction lhs, Direction rhs)
		{
			using T = std::underlying_type_t<Direction>;

			return static_cast<Direction>(static_cast<T>(lhs) & static_cast<T>(rhs));
		}

		const constexpr Direction operator<<(Direction lhs, size_t rhs)
		{
			using T = std::underlying_type_t<Direction>;

			return static_cast<Direction>(static_cast<T>(lhs) << rhs);
		}

		inline size_t get_next_square(size_t origin, Direction direction)
		{
			if (origin < 0 || origin > SQUARES_COUNT)
				throw std::out_of_range("Origin out of bounds.");

			// alias for shorter code
			const constexpr size_t X = INVALID_POS;

			switch (direction)
			{
				case Direction::NE:
				{
					static const size_t lookup[] =
					{
						X,  X,  X,  X,
						0,  1,  2,  3,
						5,  6,  7,  X,
						8,  9, 10, 11,
						13, 14, 15,  X,
						16, 17, 18, 19,
						21, 22, 23,  X,
						24, 25, 26, 27
					};
					return lookup[origin];
				}
				case Direction::SE:
				{
					static const size_t lookup[] =
					{
						5,  6,  7,  X,
						8,  9, 10, 11,
						13, 14, 15,  X,
						16, 17, 18, 19,
						21, 22, 23,  X,
						24, 25, 26, 27,
						29, 30, 31,  X,
						X,  X,  X,  X
					};
					return lookup[origin];
				}
				case Direction::SW:
				{
					static const size_t lookup[] =
					{
						4,  5,  6,  7,
						X,  8,  9, 10,
						12, 13, 14, 15,
						X, 16, 17, 18,
						20, 21, 22, 23,
						X, 24, 25, 26,
						28, 29, 30, 31,
						X,  X,  X,  X
					};
					return lookup[origin];
				}
				case Direction::NW:
				{
					static const size_t lookup[] =
					{
						X,  X,  X,  X,
						X,  0,  1,  2,
						4,  5,  6,  7,
						X,  8,  9, 10,
						12, 13, 14, 15,
						X, 16, 17, 18,
						20, 21, 22, 23,
						X, 24, 25, 26
					};
					return lookup[origin];
				}
				default:
					throw std::runtime_error("Unknown direction.");
			}
		}

		inline Direction get_direction(size_t start, size_t end)
		{
			for (const auto& dir : { Direction::NE, Direction::NW, Direction::SE, Direction::SW })
			{
				auto x = start;
				while (true)
				{
					x = get_next_square(x, dir);

					if (x == end)
						return dir;

					if (x == INVALID_POS)
						break;
				}
			}

			throw std::runtime_error("The is no straight path between selected points.");
		}

		/**
		* Returns all direction piece can move (in one bit mask).
		*/
		inline Direction get_directions_for_piece(Piece piece)
		{
			if (piece.type() == Type::King)
				return Direction::NE | Direction::NW | Direction::SE | Direction::SW;
			else if (piece.color() == Color::Black)
				return Direction::NE | Direction::NW;
			else
				return Direction::SE | Direction::SW;
		}

		/**
		* Get opposite direction for input direction.
		*/
		inline Direction get_opposite_direction(Direction dir)
		{
			switch (dir)
			{
				case Direction::NE:
					return Direction::SW;
				case Direction::SE:
					return Direction::NW;
				case Direction::SW:
					return Direction::NE;
				case Direction::NW:
					return Direction::SE;
			}

			throw std::invalid_argument("Invalid direction specified.");
		}
	}
}