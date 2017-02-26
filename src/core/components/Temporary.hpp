#pragma once

#include "Killable.hpp"

namespace lif {

/**
 * A Temporary is a special Killable component which automatically checks
 * if it's "expired" and kills itself if so.
 */
class Temporary : public lif::Killable {
	std::function<bool()> expireCondition;

public:
	explicit Temporary(lif::Entity& owner, std::function<bool()> expireCondition)
		: lif::Killable(owner)
		, expireCondition(expireCondition)
	{
		_declComponent<Temporary>();
	}

	explicit Temporary(lif::Entity& owner,
			std::function<bool()> expireCondition, 
			OnKillCallback onKill)
		: lif::Killable(owner, onKill)
		, expireCondition(expireCondition)
	{
		_declComponent<Temporary>();
	}

	explicit Temporary(lif::Entity& owner, 
			std::function<bool()> expireCondition, 
			OnKillCallback onKill, 
			CheckKillCallback checkKill)
		: lif::Killable(owner, onKill, checkKill)
		, expireCondition(expireCondition)
	{
		_declComponent<Temporary>();
	}

	void update() override {
		lif::Component::update();
		if (!killed && expireCondition())
			kill();
	}
};

}
