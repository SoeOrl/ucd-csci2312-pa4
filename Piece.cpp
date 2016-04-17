#include "Piece.h"

namespace Gaming
{

	unsigned int Piece::__idGen = 0;
	Piece::Piece(const Game &g, const Position &p): __game(g), __position(p)
	{
		__finished = false;
		__turned = false;
		__id = __idGen++;
	}

	Piece::~Piece()
	{
	}



	std::ostream & operator<<(std::ostream & os, const Piece & piece)
	{
		if (!piece.isFinished())
		{
			piece.print(os);
			return os;
		}
	}

}