#include "AI.hpp"
#include "LevelManager.hpp"
#include "AxisMoving.hpp"

using Game::AI;

AI::AI(Game::Entity& owner, AIFunction ai)
	: Game::Component(owner) 
	, ai(ai)
{}

Game::Entity* AI::init() {
	Game::Component::init();
	moving = owner.get<Game::Moving>();
	setAI(ai);
	return this;
}

void AI::update() {
	if (lm == nullptr || (moving != nullptr && moving->isBlocked()))
		return;
	func(*lm);
}

void AI::setLevelManager(Game::LevelManager *_lm) {
	lm = _lm;
}

void AI::setAI(Game::AIFunction newAI) {
	func = newAI(owner);
}
