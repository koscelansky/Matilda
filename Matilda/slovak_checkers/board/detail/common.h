#pragma once

#include <limits>
#include <bitset>

namespace SlovakCheckers
{
    namespace detail
    {
        // one side of the checkers board
        const constexpr uint8_t BOARD_SIZE = 8;

        // number of black squares, squares where pieces can be
        const constexpr uint8_t SQUARES_COUNT = BOARD_SIZE * BOARD_SIZE / 2;

        // invalid position on board
        const constexpr uint8_t INVALID_POS = std::numeric_limits<uint8_t>::max();

        using BitBoard = std::bitset<SQUARES_COUNT>;
    }
}