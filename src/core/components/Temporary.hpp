#pragma once

#include "Killable.hpp"

namespace Game {

/**
 * A Temporary is a special Killable component which automatically checks
 * if it's "expired" and kills itself if so.
 */
class Temporary : public Game::Killable {
	std::function<bool()> expireCondition;

public:
	explicit Temporary(Game::Entity& owner, std::function<bool()> expireCondition)
		: Game::Killable(owner)
		, expireCondition(expireCondition)
	{}
	explicit Temporary(Game::Entity& owner,
			std::function<bool()> expireCondition, 
			OnKillCallback onKill)
		: Game::Killable(owner, onKill)
		, expireCondition(expireCondition)
	{}
	explicit Temporary(Game::Entity& owner, 
			std::function<bool()> expireCondition, 
			OnKillCallback onKill, 
			CheckKillCallback checkKill)
		: Game::Killable(owner, onKill, checkKill)
		, expireCondition(expireCondition)
	{}

	void update() override {
		Game::Component::update();
		if (!killed && expireCondition())
			kill();
	}
};

}
