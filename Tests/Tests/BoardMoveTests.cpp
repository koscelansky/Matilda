#include <gtest/gtest.h>

#include "../../Matilda/slovak_checkers/board/move.h"

#include <sstream>

using namespace SlovakCheckers;

TEST(BoardMove, MoveTypeToSeparator)
{
	ASSERT_STREQ("-", detail::MoveTypeToSeparator(MoveType::SimpleMove));
	ASSERT_STREQ("x", detail::MoveTypeToSeparator(MoveType::Jump));

	ASSERT_ANY_THROW(detail::MoveTypeToSeparator(static_cast<MoveType>(127)));
}

TEST(BoardMove, MoveConstructorThrow)
{
	ASSERT_ANY_THROW(Move({ 1, 2, 3 }, MoveType::SimpleMove));
	ASSERT_ANY_THROW(Move({ 1 }, MoveType::SimpleMove));
	ASSERT_ANY_THROW(Move({ }, MoveType::SimpleMove));

	ASSERT_ANY_THROW(Move({ 1, 2, 3, 4, 5, 6, 7, 8, 9 }, MoveType::Jump));
	ASSERT_ANY_THROW(Move({ 1 }, MoveType::Jump));
	ASSERT_ANY_THROW(Move({ }, MoveType::Jump));
}

TEST(BoardMove, MoveConstructor)
{
	ASSERT_NO_THROW(Move({ 1, 2 }, MoveType::SimpleMove));

	ASSERT_NO_THROW(Move({ 1, 2 }, MoveType::Jump));
	ASSERT_NO_THROW(Move({ 1, 2, 3 }, MoveType::Jump));
	ASSERT_NO_THROW(Move({ 1, 2, 3, 4 }, MoveType::Jump));
	ASSERT_NO_THROW(Move({ 1, 2, 3, 4, 5 }, MoveType::Jump));
	ASSERT_NO_THROW(Move({ 1, 2, 3, 4, 5, 6 }, MoveType::Jump));
	ASSERT_NO_THROW(Move({ 1, 2, 3, 4, 5, 6, 7 }, MoveType::Jump));
	ASSERT_NO_THROW(Move({ 1, 2, 3, 4, 5, 6, 7, 8 }, MoveType::Jump));
}

TEST(BoardMove, MoveOperatorLeftShiftSimple)
{
	std::stringstream ss;
	ss << Move({ 0, 1 }, MoveType::SimpleMove);

	ASSERT_STREQ("1-2", ss.str().c_str());
}

TEST(BoardMove, MoveOperatorLeftShiftJump)
{
	std::stringstream ss;
	ss << Move({ 0, 1, 4 }, MoveType::Jump);

	ASSERT_STREQ("1x2x5", ss.str().c_str());
}