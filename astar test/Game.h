#ifndef GAME_H
#define GAME_H

#include <Windows.h>
#include "Grid.h"

class Game
{
public:
	Game(Grid g);
	Grid grid;
	POINT mousePos;
};

#endif // GRID_H
