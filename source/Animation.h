#ifndef Animation_h
#define Animation_h

#include <vector>
#include <memory>

class AnimationTarget
{
public:
	virtual void setProperty(int propertyID, double value) = 0;
};

enum TimingFunction
{
	TMFUNC_LINEAR,
	TMFUNC_EASE,
	TMFUNC_EASE_IN,
	TMFUNC_EASE_OUT,
	TMFUNC_EASE_IN_OUT
};

class ValueTransition
{
private:
	struct PercentValuePair
	{
		int percent;
		double value;
	};

public:
	explicit ValueTransition(int propertyID) : m_propertyID(propertyID) { }

	void add(int percent, double value);
	double initialValue() { return m_pairs.front().value; }
	double calculate(int elapsed, int duration, TimingFunction timingFunc);
	int propertyID() { return m_propertyID; }

private:
	int m_propertyID;
	std::vector<PercentValuePair> m_pairs;
};

class Animation
{
	friend class AnimationExecutor;
public:
	Animation(int duration, TimingFunction timingFunc, int delay = 0)
		: m_duration(duration), m_timingFunc(timingFunc), m_delay(delay) { }
	~Animation();

	ValueTransition *createTransition(int propertyID);

private:
	int m_delay;
	int m_duration;
	TimingFunction m_timingFunc;
	std::vector<ValueTransition *> m_transitions;
};

class AnimationExecutor
{
public:
	AnimationExecutor(std::shared_ptr<Animation> animation, AnimationTarget *target)
		: m_animation(animation),
		  m_alive(true), m_elapsed(-animation->m_delay), m_duration(animation->m_duration),
		  m_timingFunc(animation->m_timingFunc),
		  m_transitions(animation->m_transitions),
		  m_target(target) { }
	~AnimationExecutor() { end(); }

	void init();
	void progress(int delta_ms);
	void end();

	bool alive() { return m_alive; }

private:
	std::shared_ptr<Animation> m_animation;
	AnimationTarget *m_target;
	bool m_alive;
	int m_elapsed;
	int m_duration;
	TimingFunction m_timingFunc;
	std::vector<ValueTransition *> &m_transitions;
};

#endif
