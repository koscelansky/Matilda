#include <gtest/gtest.h>

#include "../../Matilda/slovak_checkers/board/board.h"

#include <sstream>
#include <algorithm>

using namespace SlovakCheckers;

void AssertMovesEq(std::vector<std::string> expected, Board::BoardMoves actual)
{
	std::vector<Move> actualAll;
	auto appendMoves = [&actualAll](const std::vector<Move>& x)
	{
		actualAll.insert(actualAll.end(), x.begin(), x.end());
	};
	appendMoves(actual.kingJumps);
	appendMoves(actual.manJumps);
	appendMoves(actual.simpleMoves);

	std::sort(std::begin(expected), std::end(expected));

	std::vector<std::string> actualStr;
	for (const auto& i : actualAll)
	{
		std::stringstream ss;
		ss << i;
		actualStr.push_back(ss.str());
	}
	std::sort(std::begin(actualStr), std::end(actualStr));

	ASSERT_EQ(expected, actualStr);
}

TEST(GetBoardMoves, StartPosition)
{
	Board b;

	auto moves = b.GetMoves();
	ASSERT_NO_FATAL_FAILURE(AssertMovesEq({ "5-9", "6-10", "6-9", "7-11", "7-10", "8-12", "8-11" }, moves));
}
