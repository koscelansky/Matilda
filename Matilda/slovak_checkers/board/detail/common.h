#pragma once

#include <limits>
#include <bitset>

namespace sc
{
    namespace detail
    {
        // one side of the checkers board
        const constexpr size_t BOARD_SIZE = 8;

        // number of black squares, squares where pieces can be
        const constexpr size_t SQUARES_COUNT = BOARD_SIZE * BOARD_SIZE / 2;

        // invalid position on board
        const constexpr size_t INVALID_POS = std::numeric_limits<size_t>::max();

        using BitBoard = std::bitset<SQUARES_COUNT>;
    }
}