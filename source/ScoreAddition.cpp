#include "ScoreAddition.h"
#include "Font.h"
#include <memory>
#include <sstream>

static std::shared_ptr<Animation> scoreRiseAnimation;

void ScoreAddition_loadMetaData()
{
	ValueTransition *t;

	scoreRiseAnimation = std::make_shared<Animation>(600, TMFUNC_EASE_IN);
	t = scoreRiseAnimation->createTransition(2);
	t->add(0, 0);
	t->add(100, -75);
	t = scoreRiseAnimation->createTransition(3);
	t->add(0, 1.0);
	t->add(100, 0.0);
}

void ScoreAddition_unloadMetaData()
{
	scoreRiseAnimation.reset();
}

ScoreAddition::ScoreAddition(int delta)
{
	Font font;
	font.init(25);
	std::stringstream ss;
	ss << '+' << delta;
	const SDL_Color color = { 119, 110, 101, static_cast<Uint8>(0.9 * 255) };
	m_texture.loadFromLatinText(&font, ss.str().c_str(), color);
	m_texture.enableAlpha();

	m_animationExecutor = new AnimationExecutor(scoreRiseAnimation, this);
	m_animationExecutor->init();
}

ScoreAddition::~ScoreAddition()
{
	delete m_animationExecutor;
}

void ScoreAddition::setProperty(int propertyID, double value)
{
	switch(propertyID) {
	case 2:
		m_y = static_cast<int>(value);
		break;
	case 3:
		m_opacity = value;
		break;
	}
}

void ScoreAddition::render(int x, int y)
{
	m_texture.setAlpha(static_cast<Uint8>(m_opacity * 255));
	m_texture.render(x, y + m_y);
}

void ScoreAddition::update(int delta_ms)
{
	m_animationExecutor->progress(delta_ms);
}
