#ifndef ScoreAddition_h
#define ScoreAddition_h

#include "Animation.h"
#include "Texture.h"

extern void ScoreAddition_loadMetaData();
extern void ScoreAddition_unloadMetaData();

class ScoreAddition : public AnimationTarget
{
public:
	explicit ScoreAddition(int delta);
	~ScoreAddition();
	virtual void setProperty(int propertyID, double value) override;
	void render(int x, int y);
	void update(int delta_ms);

	int width() { return m_texture.width(); }
	int height() { return m_texture.height(); }

	bool alive() { return m_animationExecutor->alive(); }

private:
	Texture m_texture;
	int m_y;
	double m_opacity;
	AnimationExecutor *m_animationExecutor;
};

#endif
