#include <gtest/gtest.h>

#include "../../Matilda/slovak_checkers/board/piece.h"

#include <sstream>

using namespace SlovakCheckers;

TEST(BoardPiece, GetColorFromChar)
{
	ASSERT_EQ(Color::White, detail::GetColorFromChar('W'));
	ASSERT_EQ(Color::Black, detail::GetColorFromChar('B'));

	ASSERT_ANY_THROW(detail::GetColorFromChar('X'));
	ASSERT_ANY_THROW(detail::GetColorFromChar('\0'));
	ASSERT_ANY_THROW(detail::GetColorFromChar(' '));
}

TEST(BoardPiece, OperatorLeftShiftBM)
{
	std::stringstream ss;
	ss << Piece(Color::Black, Type::Man);

	ASSERT_EQ("BM", ss.str());
}

TEST(BoardPiece, OperatorLeftShiftBK)
{
	std::stringstream ss;
	ss << Piece(Color::Black, Type::King);

	ASSERT_EQ("BK", ss.str());
}

TEST(BoardPiece, OperatorLeftShiftWM)
{
	std::stringstream ss;
	ss << Piece(Color::White, Type::Man);

	ASSERT_EQ("WM", ss.str());
}

TEST(BoardPiece, OperatorLeftShiftWK)
{
	std::stringstream ss;
	ss << Piece(Color::White, Type::King);

	ASSERT_EQ("WK", ss.str());
}
