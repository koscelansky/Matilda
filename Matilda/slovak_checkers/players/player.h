#pragma once

#include "../board/board.h"

namespace SlovakCheckers
{
	class Player
	{
	public:
		virtual ~Player() { }

		virtual void PerformMove(Board& board) = 0;

	protected:
		static const BoardState& GetBoardState(const Board& board)
		{
			return board.m_board;
		}
	};
}