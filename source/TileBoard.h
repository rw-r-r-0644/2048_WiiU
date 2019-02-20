#ifndef TileBoard_h
#define TileBoard_h

#include "Tile.h"

extern void TileBoard_loadMetaData();
extern void TileBoard_unloadMetaData();

class Game;
class TileBoard
{
public:
	explicit TileBoard(Game *pGame);
	void render();

	void setXY(int x, int y) { m_x = x; m_y = y; }
	int width();
	int height() { return width(); }

private:
	Game *m_pGame;
	int m_x;
	int m_y;
};

#endif
