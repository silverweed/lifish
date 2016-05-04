#pragma once

#include "Component.hpp"

namespace Game {

/**
 * A Temporary defines an entity which gets deleted by an EntityGroup
 * as soon as isExpired() returns true.
 */
class Temporary : public Game::Component {
	bool expired = false;

	std::function<bool()> expireCondition;

public:
	explicit Temporary(Game::Entity *const owner)
		: Game::Component(owner) {}

	explicit Temporary(Game::Entity *const owner, std::function<bool()> expireCondition)
		: Game::Component(owner)
		, expireCondition(expireCondition)
	{}

	bool isExpired() const { return expired; }
	void expire() { expired = true; }

	void update() override {
		if (!expired && expireCondition && expireCondition())
			expired = true;
	}
};

}
