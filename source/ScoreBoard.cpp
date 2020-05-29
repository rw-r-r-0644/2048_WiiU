#include "ScoreBoard.h"
#include "Render.h"
#include <string>
#include <sstream>

ScoreBoard::ScoreBoard(const char *title, int &score)
	: m_title(title),
	  m_score(score)
{
	m_titleFont.init(14);
	m_numberFont.init(36);
}

void ScoreBoard::render()
{
	Texture texture;
	texture.createBlank(width(), height(), SDL_TEXTUREACCESS_TARGET);
	Texture titleText;
	SDL_Color titleColor = { 0xEE, 0xE4, 0xDA, 0xFF };
	titleText.loadFromLatinText(&m_titleFont, m_title, titleColor);
	Texture numberText;
	std::stringstream buf;
	buf << m_score;
	numberText.loadFromLatinText(&m_numberFont, buf.str().c_str(), WHITE);

	g_render.setTarget(texture.sdl_texture());
	
	g_render.setDrawColor(0xBB, 0xAD, 0xA0);
	g_render.clear();
	titleText.render((texture.width() - titleText.width()) / 2, 3);
	numberText.render(
		(texture.width() - numberText.width()) / 2,
		texture.height() - numberText.height() - 2);

	g_render.setTarget(nullptr);

	texture.render(m_x, m_y);
}
