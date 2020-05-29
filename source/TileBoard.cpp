#include "TileBoard.h"
#include "Render.h"
#include "Font.h"
#include "Game.h"

// [TODO] refactor
const int tileSize = 100;
const int gridSpacing = 15;

static Texture gameOverText;

void TileBoard_loadMetaData()
{
	Font font;
	font.init(60);
	SDL_Color color = { 0x77, 0x6E, 0x65, 0xFF };
	gameOverText.loadFromLatinText(&font, "Game Over", color);
	font.free();
}

void TileBoard_unloadMetaData()
{
	gameOverText.free();
}

TileBoard::TileBoard(Game *pGame)
	: m_pGame(pGame)
{
}

int TileBoard::width()
{
	// [TODO] refactor
	const int gridsSize = tileSize * m_pGame->m_size + gridSpacing * (m_pGame->m_size + 1);
	return gridsSize;
}

void TileBoard::render()
{
	const int gridsSize = tileSize * m_pGame->m_size + gridSpacing * (m_pGame->m_size + 1);

	g_render.setDrawColor(0xBB, 0xAD, 0xA0);
	g_render.fillRect(m_x, m_y, gridsSize, gridsSize);

	Texture holder;
	holder.createBlank(tileSize, tileSize, SDL_TEXTUREACCESS_TARGET);
	holder.enableAlpha();
	g_render.setTarget(holder.sdl_texture());
	g_render.setDrawColor(238, 228, 218, static_cast<int>(0.35 * 255));
	g_render.clear();
	g_render.setTarget(nullptr);

	for(int row = 0; row < m_pGame->m_size; row++) {
		for(int col = 0; col < m_pGame->m_size; col++) {
			holder.render(
				m_x + gridSpacing + col * (tileSize + gridSpacing),
				m_y + gridSpacing + row * (tileSize + gridSpacing));
		}
	}

	auto renderTile = [&] (Tile *t) { t->render(m_x + gridSpacing, m_y + gridSpacing); };
	m_pGame->forEachTile(renderTile);

	if(m_pGame->gameOver()) {
		Texture mask;
		mask.createBlank(gridsSize, gridsSize, SDL_TEXTUREACCESS_TARGET);
		mask.enableAlpha();
		g_render.setTarget(mask.sdl_texture());
		g_render.setDrawColor(238, 228, 218, static_cast<int>(0.5 * 255));
		g_render.clear();
		gameOverText.render(
			(gridsSize - gameOverText.width()) / 2,
			(gridsSize - gameOverText.height()) / 2);
		g_render.setTarget(nullptr);
		mask.render(m_x, m_y);
	}
}
