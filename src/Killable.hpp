#pragma once

#include <functional>
#include "Component.hpp"

namespace Game {

class Killable : public Game::Component {
private:
	using Callback = std::function<void(Game::Entity*)>;
	using CheckKillCallback = std::function<bool(Game::Entity*)>;

	bool killed = false;

	/** Function to be called on kill() */
	Callback onKill;
	CheckKillCallback checkKillProgress;

public:
	explicit Killable(Game::Entity *const owner); 
	explicit Killable(Game::Entity *const owner, Callback callback); 
	explicit Killable(Game::Entity *const owner, Callback callback, CheckKillCallback checkKill); 

	void kill();
	bool isKilled() const { return killed; }
	/** 
	 * If checkKill callback is provided, this will return whether the kill is still
	 * happening or not. If not provided, will always return false.
	 * Note that isKillInProgress() implies isKilled()
	 */
	bool isKillInProgress() const;
};

}
