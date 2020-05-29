#include "Button.h"
#include "Font.h"
#include "Render.h"
#include "SDLUtils.h"

static Font buttonFont;
static std::shared_ptr<Animation> buttonFadeInAnim;
static std::shared_ptr<Animation> buttonFadeOutAnim;
const SDL_Color buttonFgColor = { 0xF9, 0xF6, 0xF2, 0xFF };
const SDL_Color buttonBgColor = { 0x8F, 0x7A, 0x66, 0xFF };

void Button_loadMetaData()
{
	buttonFont.init(18);

	ValueTransition *t;

	buttonFadeInAnim = std::make_shared<Animation>(200, TMFUNC_EASE_IN_OUT);
	t = buttonFadeInAnim->createTransition(5);
	t->add(0, 0.0);
	t->add(100, 1.0);

	buttonFadeOutAnim = std::make_shared<Animation>(200, TMFUNC_EASE_IN_OUT);
	t = buttonFadeOutAnim->createTransition(5);
	t->add(0, 1.0);
	t->add(100, 0.0);
}

void Button_unloadMetaData()
{
	buttonFadeInAnim.reset();
	buttonFadeOutAnim.reset();

	buttonFont.free();
}

Button::Button(const char *text, std::function<void ()> onClick)
	: m_text(text),
	  m_onClick(onClick)
{
	m_x = 0;
	m_y = 0;
	m_width = 120;
	m_height = 40;
	m_down = false;
	m_inversedRatio = 0.0;
}

void Button::render()
{
	SDL_Color fgColor = Color_Mix(buttonBgColor, buttonFgColor, m_inversedRatio);
	Texture textTexture;
	textTexture.loadFromLatinText(&buttonFont, m_text, fgColor);

	Texture texture;
	texture.createBlank(m_width, m_height, SDL_TEXTUREACCESS_TARGET);
	
	g_render.setTarget(texture.sdl_texture());
	SDL_Color bgColor = Color_Mix(buttonFgColor, buttonBgColor, m_inversedRatio);
	g_render.setDrawColor(bgColor);
	g_render.clear();
	textTexture.render(
		(texture.width() - textTexture.width()) / 2,
		(texture.height() - textTexture.height()) / 2);
	g_render.setDrawColor(buttonBgColor);
	g_render.drawRect(0, 0, m_width, m_height);
	g_render.drawRect(1, 1, m_width - 2 , m_height - 2);
	g_render.setTarget(nullptr);

	texture.render(m_x, m_y);
}

void Button::update(int delta_ms)
{
	if(m_animationExecutor)
		m_animationExecutor->progress(delta_ms);
}

void Button::setProperty(int propertyID, double value)
{
	if(propertyID == 5) {
		m_inversedRatio = value;
	}
}


void Button::handleEvent(const SDL_Event &e)
{
	if(e.type == SDL_FINGERMOTION || e.type == SDL_FINGERDOWN || e.type == SDL_FINGERUP) {
		int x = e.tfinger.x * SCREEN_WIDTH;
		int y = e.tfinger.y * SCREEN_HEIGHT;

		bool inside = isInside(x, y);

		if(e.type == SDL_FINGERDOWN) {
			if(inside)
			{
				m_down = true;
				m_animationExecutor = std::make_shared<AnimationExecutor>(buttonFadeInAnim, this);
				m_animationExecutor->init();
			}
		}
		else if(m_down) {
			m_down = false;
			if(inside)
			{
				m_onClick();
				m_animationExecutor = std::make_shared<AnimationExecutor>(buttonFadeOutAnim, this);
				m_animationExecutor->init();
			}
		}
	}
}

bool Button::isInside(int x, int y)
{
	return x >= m_x && x < m_x + m_width
		&& y >= m_y && y < m_y + m_height;
}
