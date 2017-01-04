#include <iostream>

#include "slovak_checkers/board.h"

#include <stdlib.h>

#include <windows.h>

class PerformanceCounter
{
private:
    double m_PCFreq;
    long long m_CounterStart;
public:
    PerformanceCounter()
    {
        LARGE_INTEGER Freq;
        QueryPerformanceFrequency(&Freq);
        m_PCFreq = static_cast<double>(Freq.QuadPart) / 1000.0;
        m_CounterStart = 0;
    }

    void StartCounter()
    {
        LARGE_INTEGER Counter;
        QueryPerformanceCounter(&Counter);
        m_CounterStart = Counter.QuadPart;
    }
    double GetCounter()
    {
        LARGE_INTEGER Counter;
        QueryPerformanceCounter(&Counter);
        return static_cast<double>(Counter.QuadPart - m_CounterStart) / m_PCFreq;
    }
};

int main()
{
    sc::SlovakCheckersBoard scb("B:W5,6,7,8,9,10,11,12:B18,19,20,24,29,30,31,32");

    PerformanceCounter pc;
    pc.StartCounter();

    int move = 0;

	while (true)
	{
		std::cout << scb;
		auto v = scb.get_moves();

		if (scb.game_ended())
			break;

		std::cout << scb.get_fen() << std::endl;

		for (int i = 0; i < v.size(); ++i)
		{
			std::cout << i << ": " << v[i] << "   ";
		}

		std::cout << std::endl;

		if (scb.next_player() == sc::PieceColor::White)
		{
			scb.perform_move(sc::SlovakCheckersSolver(sc::PieceColor::White).get_best_move(scb));
			/*
			int i;
			std::cin >> i;
			scb.perform_move(v[i]);
			*/
            
		}
		else
		{
			scb.perform_move(sc::SlovakCheckersSolver(sc::PieceColor::Black).get_best_move(scb));
		}

        getchar();

        move++;
        if (move == 50)
            break;
	}

    //std::cout << std::fixed << pc.GetCounter() << std::endl;

    getchar();

	return 0; 
}
