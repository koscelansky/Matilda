#pragma once

#include "board.h"

namespace sc
{
	class Player
	{
	public:
		virtual void PerformMove(SlovakCheckersBoard& board) = 0;
	};
}