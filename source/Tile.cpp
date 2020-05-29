#include "Tile.h"
#include <sstream>
#include <map>
#include "SDLUtils.h"
#include "Render.h"
#include "Font.h"
#include "Animation.h"

// [TODO] fix text sizes

struct TileStyle
{
	SDL_Color bgColor;
	SDL_Color fgColor;
	Font *font;
};

static std::map<int, TileStyle> tileStyleMap;
static TileStyle superTileStyle;

static SDL_Color tileColor = { 0xEE, 0xE4, 0xDA, 0xFF };
static SDL_Color tileGoldColor = { 0xED, 0xC2, 0x2E, 0xFF };

static SDL_Color specialColors[] =
{
	{ 0xF7, 0x8E, 0x48, 0xFF }, // 8
	{ 0xFC, 0x5E, 0x2E, 0xFF }, // 16
	{ 0xFF, 0x33, 0x33, 0xFF }, // 32
	{ 0xFF, 0x00, 0x00, 0xFF }, // 64
};

static SDL_Color textColor = { 0x77, 0x6E, 0x65, 0xFF };
static SDL_Color brightTextColor = { 0xF9, 0xF6, 0xF2, 0xFF };

static Font numFontLarge;
static Font numFontMedium;
static Font numFontSmall;
static Font numFontXSmall;

static void BuildTileStyles()
{
	int power = 2;
	int exponent = 1;
	int limit = 11;

	while(exponent <= limit) {
		TileStyle style;
		double goldRatio = (exponent - 1.0) / (limit - 1.0);
		style.bgColor = Color_Mix(tileGoldColor, tileColor, goldRatio);

		if(power >= 8 && power <= 64) {
			SDL_Color &specialColor = specialColors[exponent - 3];
			style.bgColor = Color_Mix(specialColor, style.bgColor, 0.55);
		}

		style.fgColor = (power >= 8) ? brightTextColor : textColor;

		style.font = &numFontLarge;
		if(power >= 100 && power < 1000)
			style.font = &numFontMedium;
		else if(power >= 1000)
			style.font = &numFontSmall;

		tileStyleMap[power] = style;

		exponent++;
		power *= 2;
	}

	SDL_Color c = { 0x33, 0x33, 0x33, 0xFF };
	superTileStyle.bgColor = Color_Mix(c, tileGoldColor, 0.95);
	superTileStyle.fgColor = brightTextColor;
	superTileStyle.font = &numFontXSmall;
}

std::shared_ptr<Animation> tileAppearAnimation;
std::shared_ptr<Animation> tilePopAnimation;

const int moveTimeInMillis = 100;

void BuildTileAnimations()
{
	ValueTransition *t;

	tileAppearAnimation = std::make_shared<Animation>(200, TMFUNC_EASE, moveTimeInMillis);
	t = tileAppearAnimation->createTransition(4);
	t->add(0, 0.0);
	t->add(100, 1.0);

	tilePopAnimation = std::make_shared<Animation>(200, TMFUNC_EASE_IN_OUT, moveTimeInMillis);
	t = tilePopAnimation->createTransition(3);
	t->add(0, 0.0);
	t->add(100, 1.0);
	t = tilePopAnimation->createTransition(4);
	t->add(0, 0.0);
	t->add(50, 1.2);
	t->add(100, 1.0);
}

int tileSize = 100;
int gridSpacing = 15;

void calculatePosition(int row, int col, int *pX, int *pY)
{
	*pX = col * (tileSize + gridSpacing);
	*pY = row * (tileSize + gridSpacing);
}

std::shared_ptr<Animation> Tile_makeMoveAnimation(int row1, int col1, int row2, int col2)
{
	int x1, y1, x2, y2;
	calculatePosition(row1, col1, &x1, &y1);
	calculatePosition(row2, col2, &x2, &y2);

	auto anim = std::make_shared<Animation>(moveTimeInMillis, TMFUNC_EASE_IN_OUT);
	if(col1 != col2) {
		// change x
		auto t = anim->createTransition(1);
		t->add(0, x1);
		t->add(100, x2);
	}
	else {
		// change y
		auto t = anim->createTransition(2);
		t->add(0, y1);
		t->add(100, y2);
	}

	return anim;
}

void Tile_LoadMetaData()
{
	numFontLarge.init(55);
	numFontMedium.init(45);
	numFontSmall.init(35);
	numFontXSmall.init(30);

	BuildTileStyles();
	BuildTileAnimations();
}

void Tile_UnloadMetaData()
{
	numFontLarge.free();
	numFontMedium.free();
	numFontSmall.free();
	numFontXSmall.free();

	tileAppearAnimation.reset();
	tilePopAnimation.reset();
}

//////////////////////////////////////////////////////////////////////

Tile::Tile(int row, int col, int value)
{
	std::stringstream buf;
	Texture numTexture;
	buf << value;
	numTexture.loadFromLatinText(&numFontLarge, buf.str().c_str(), tileStyleMap[value].fgColor);

	m_texture.createBlank(tileSize, tileSize, SDL_TEXTUREACCESS_TARGET);
	m_texture.enableAlpha();
	g_render.setTarget(m_texture.sdl_texture());
	g_render.setDrawColor(tileStyleMap[value].bgColor);
	g_render.clear();
	numTexture.render((tileSize - numTexture.width()) / 2, (tileSize - numTexture.height()) / 2);
	g_render.setTarget(NULL);

	m_row = row;
	m_col = col;
	calculatePosition(row, col, &m_x, &m_y);
	m_value = value;

	m_opacity = 1.0;
	m_scale = 1.0;

	m_mergedFrom1 = m_mergedFrom2 = nullptr;
}

void renderTile(Tile *tile, int x, int y)
{
	tile->m_texture.setAlpha(static_cast<Uint8>(255 * tile->m_opacity));
	tile->m_texture.renderScaled(x + tile->m_x, y + tile->m_y, tile->m_scale);
}

void Tile::render(int x, int y)
{
	if(hasMergedFrom()) {
		renderTile(m_mergedFrom1, x, y);
		renderTile(m_mergedFrom2, x, y);
	}
	renderTile(this, x, y);
}

void Tile::setProperty(int propertyID, double value)
{
	switch(propertyID) {
	case 1:
		m_x = static_cast<int>(value);
		break;
	case 2:
		m_y = static_cast<int>(value);
		break;
	case 3:
		m_opacity = value;
		break;
	case 4:
		// [TODO] consider using texture redraw to implement tile scaling
		m_scale = value;
		calculatePosition(m_row, m_col, &m_x, &m_y);
		m_x -= static_cast<int>(tileSize * (m_scale - 1) / 2);
		m_y -= static_cast<int>(tileSize * (m_scale - 1) / 2);
	}
}

void updateTile(Tile *tile, int delta_ms)
{
	if(tile->m_animationExecutor) {
		tile->m_animationExecutor->progress(delta_ms);
	}
}

void Tile::update(int delta_ms)
{
	updateTile(this, delta_ms);
	if(hasMergedFrom()) {
		updateTile(m_mergedFrom1, delta_ms);
		updateTile(m_mergedFrom2, delta_ms);
	}
}

void Tile::attachAnimation(std::shared_ptr<Animation> animation)
{
	m_animationExecutor = std::make_shared<AnimationExecutor>(animation, this);
	m_animationExecutor->init();
}

void Tile::planMovement(const Vector &v)
{
	attachAnimation(Tile_makeMoveAnimation(m_row, m_col, v.row, v.col));
	m_row = v.row;
	m_col = v.col;
}
