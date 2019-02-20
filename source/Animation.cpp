#include "UnitBezier.h"
#include "Animation.h"

static UnitBezier bezier_ease(0.25, 0.1, 0.25, 1.0);
static UnitBezier bezier_easeIn(0.42, 0.0, 1.0, 1.0);
static UnitBezier bezier_easeOut(0.0, 0.0, 0.58, 1.0);
static UnitBezier bezier_easeInOut(0.42, 0.0, 0.58, 1.0);

inline double interploate(double from, double to, double ratio)
{
	return from + (to - from) * ratio;
}

inline double timing(TimingFunction timingFunc, int duration, double input)
{
	switch(timingFunc) {
	case TMFUNC_LINEAR:
		return input;
	case TMFUNC_EASE:
		return bezier_ease.solve(input, duration);
	case TMFUNC_EASE_IN:
		return bezier_easeIn.solve(input, duration);
	case TMFUNC_EASE_OUT:
		return bezier_easeOut.solve(input, duration);
	case TMFUNC_EASE_IN_OUT:
		return bezier_easeInOut.solve(input, duration);
	default:
		// avoid warning
		return 0.0;
	}
}

void ValueTransition::add(int percent, double value)
{
	PercentValuePair p;
	p.percent = percent;
	p.value = value;
	m_pairs.push_back(p);
}

double ValueTransition::calculate(int elapsed, int duration, TimingFunction timingFunc)
{
	auto iter = m_pairs.cbegin();

	while(elapsed * 100 / duration >= (iter + 1)->percent) {
		iter++;
		if(iter->percent == 100)
			return iter->value;
	}

	auto next = iter + 1;
	auto offset_ms = elapsed - iter->percent / 100.0 * duration;
	auto percent_diff = next->percent - iter->percent;
	auto interval_ms = percent_diff / 100.0 * duration;

	return interploate(iter->value, next->value, timing(timingFunc, duration, offset_ms / interval_ms));
}

////////////////////////////////////////////////////////////////////////////////

Animation::~Animation()
{
	for(auto iter = m_transitions.cbegin(); iter != m_transitions.cend(); iter++) {
		delete (*iter);
	}
}

ValueTransition *Animation::createTransition(int propertyID)
{
	ValueTransition *t = new ValueTransition(propertyID);
	m_transitions.push_back(t);
	return t;
}

////////////////////////////////////////////////////////////////////////////////

void AnimationExecutor::init()
{
	for(auto iter = m_transitions.cbegin(); iter != m_transitions.cend(); iter++) {
		m_target->setProperty((*iter)->propertyID(), (*iter)->initialValue());
	}
}

void AnimationExecutor::progress(int delta_ms)
{
	if(!m_alive)
		return;

	m_elapsed += delta_ms;
	if(m_elapsed < 0)
		return;

	if(m_elapsed >= m_duration)
		m_alive = false;

	for(auto iter = m_transitions.cbegin(); iter != m_transitions.cend(); iter++) {
		m_target->setProperty(
			(*iter)->propertyID(),
			(*iter)->calculate(m_elapsed, m_duration, m_timingFunc));
	}
}

void AnimationExecutor::end()
{
	m_elapsed = 0;
	progress(m_duration);
}
