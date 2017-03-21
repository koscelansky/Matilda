#pragma once

#include "../board/board.h"

namespace sc
{
	class Player
	{
	public:
		virtual ~Player() { }

		virtual void PerformMove(SlovakCheckersBoard& board) = 0;

	protected:
		static const BoardState& GetBoardState(const SlovakCheckersBoard& board)
		{
			return board.m_board;
		}
	};
}