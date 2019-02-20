#ifndef ScoreBoard_h
#define ScoreBoard_h

#include "Texture.h"
#include "Font.h"

class ScoreBoard
{
public:
	explicit ScoreBoard(const char *title, int &score);
	void render();

	int x() { return m_x; }
	int y() { return m_y; }
	void setXY(int newX, int newY) { m_x = newX; m_y = newY; }
	int width() { return 130; }
	int height() { return 70; }

private:
	int m_x;
	int m_y;
	const char *m_title;
	int &m_score;
	Font m_numberFont;
	Font m_titleFont;
};

#endif
