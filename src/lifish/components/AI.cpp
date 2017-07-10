#include "AI.hpp"
#include "LevelManager.hpp"
#include "AxisMoving.hpp"

using lif::AI;

AI::AI(lif::Entity& owner, AIFunction ai)
	: lif::Component(owner)
	, ai(ai)
{
	_declComponent<AI>();
}

lif::Entity* AI::init() {
	lif::Component::init();
	moving = owner.get<lif::Moving>();
	setAI(ai);
	return this;
}

void AI::update() {
	if (lm == nullptr || (moving != nullptr && moving->isBlocked()))
		return;
	func(*lm);
}

void AI::setLevelManager(lif::LevelManager *_lm) {
	lm = _lm;
}

void AI::setAI(lif::AIFunction newAI) {
	func = newAI(owner);
}
