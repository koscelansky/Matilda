#include <gtest/gtest.h>

#include "../../Matilda/slovak_checkers/board/board.h"

using namespace SlovakCheckers;

TEST(Fen, TotallyInvalidString)
{
	EXPECT_ANY_THROW(Board("Invalid string."));
	EXPECT_ANY_THROW(Board(""));
}

TEST(Fen, InvalidString)
{
	EXPECT_ANY_THROW(Board("B:B10:B1"));
	EXPECT_ANY_THROW(Board("X:B1:W2"));
	EXPECT_ANY_THROW(Board("B:B1,2,3,4,5,6,7,8,9:W2"));
	EXPECT_ANY_THROW(Board("B:B1,2,3,33,7,8,9:W2"));
	EXPECT_ANY_THROW(Board("B:B:B"));
	EXPECT_ANY_THROW(Board("B:W:W"));
	EXPECT_ANY_THROW(Board("B:WK0:B"));
}

TEST(Fen, Empty)
{
	EXPECT_ANY_THROW(Board("B:B:W"));
	EXPECT_ANY_THROW(Board("B:W:B"));
	EXPECT_ANY_THROW(Board("W:W:B"));
	EXPECT_ANY_THROW(Board("W:B:W"));
	EXPECT_ANY_THROW(Board("B:B:B"));
}

TEST(Fen, SetAndGet)
{
	std::string fen = "W:W1,2,3,4,5,6,7,8:B25,26,27,28,29,30,31,32";
	EXPECT_EQ(fen, Board(fen).GetFen());

	fen = "W:WK1,K3,4,5,6,7,8:BK18,25,26,27,29,31,32";
	EXPECT_EQ(fen, Board(fen).GetFen());
}