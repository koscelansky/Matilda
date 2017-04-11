#pragma once

#include <stdint.h>
#include <assert.h>
#include <initializer_list>
#include <bitset>

#include "common.h"
#include "../piece.h"

namespace sc
{
	namespace detail
	{
		enum class Direction : uint32_t
		{
			NE, // up right
			SE, // down right
			SW, // down left
			NW, // up left

			Count, // helper to get direction count
		};

		constexpr uint32_t dir_to_uint32(Direction dir) noexcept
		{
			static_assert(std::is_same<std::underlying_type_t<Direction>, uint32_t>::value,
				"Underlying type of direction should be uint32_t");

			return static_cast<uint32_t>(dir);
		}

		constexpr Direction uint32_to_dir(uint32_t n) noexcept
		{
			static_assert(std::is_same<std::underlying_type_t<Direction>, uint32_t>::value,
				"Underlying type of direction should be uint32_t");

			return static_cast<Direction>(n);
		}

		// type for holding bit mask for directions
		using Directions = std::bitset<dir_to_uint32(Direction::Count)>;

		const constexpr Directions ALL_DIRECTIONS =
			1 << dir_to_uint32(Direction::NE) | 
			1 << dir_to_uint32(Direction::NW) |
			1 << dir_to_uint32(Direction::SE) | 
			1 << dir_to_uint32(Direction::SW);

		const constexpr Directions ALL_NORTH =
			1 << dir_to_uint32(Direction::NE) | 
			1 << dir_to_uint32(Direction::NW);

		const constexpr Directions ALL_SOUTH =
			1 << dir_to_uint32(Direction::SE) |
			1 << dir_to_uint32(Direction::SW);



		inline uint8_t get_next_square(uint8_t origin, Direction direction)
		{
			if (origin < 0 || origin > SQUARES_COUNT)
				throw std::out_of_range("Origin out of bounds.");

			// alias for shorter code
			const constexpr size_t X = INVALID_POS;

			switch (direction)
			{
				case Direction::NE:
				{
					static const uint8_t lookup[] =
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
					static const uint8_t lookup[] =
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
					static const uint8_t lookup[] =
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
					static const uint8_t lookup[] =
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

		inline Direction get_direction(uint8_t start, uint8_t end)
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
		inline Directions get_directions_for_piece(Piece piece)
		{
			if (piece.type() == Type::King)
				return ALL_DIRECTIONS;
			else if (piece.color() == Color::Black)
				return ALL_NORTH;
			else
				return ALL_SOUTH;
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