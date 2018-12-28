#pragma once

#include "Component.hpp"
#include <functional>

namespace lif {

class Killable : public lif::Component {
protected:
	using OnKillCallback = std::function<void()>;
	using CheckKillCallback = std::function<bool()>;

	sf::Time deathTime;
	bool killed = false;

	/** Function to be called on kill() */
	OnKillCallback onKill;
	CheckKillCallback checkKillProgress;

public:
	explicit Killable(lif::Entity& owner);
	explicit Killable(lif::Entity& owner, OnKillCallback callback);
	explicit Killable(lif::Entity& owner, OnKillCallback callback, CheckKillCallback checkKill);

	void kill();
	void resurrect() { killed = false; }
	bool isKilled() const { return killed; }
	/**
	 * If checkKill callback is provided, this will return true if the kill is still
	 * happening and therefore the owner should not be disposed of.
	 * If not provided, will always return false.
	 * Note that isKillInProgress() implies isKilled()
	 */
	bool isKillInProgress() const;

	/** @return The time passed since kill() was called last. Only valid if `isKilled()`. */
	sf::Time timeSinceDeath() const;
};

}
