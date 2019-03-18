#ifndef Button_h
#define Button_h

#include <SDL.h>
#include <functional>
#include "Texture.h"
#include "Animation.h"

extern void Button_loadMetaData();
extern void Button_unloadMetaData();

class Button : public AnimationTarget
{
public:
	Button(const char *text, std::function<void ()> onClick);
	void setXY(int x, int y) { m_x = x; m_y = y; }
	void render();
	void handleEvent(const SDL_Event &e);
	void update(int delta_ms);
	virtual void setProperty(int propertyID, double value) override;

	int width() { return m_width; }
	int height() { return m_height; }

private:
	const char *m_text;
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	std::function<void ()> m_onClick;
	bool m_down;
	double m_inversedRatio;
	std::shared_ptr<AnimationExecutor> m_animationExecutor;

	bool isInside(int x, int y);
};

#endif
