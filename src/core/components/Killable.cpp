#include "Killable.hpp"

using Game::Killable;

Killable::Killable(Game::Entity& owner)
	: Game::Component(owner)
{}

Killable::Killable(Game::Entity& owner, OnKillCallback callback)
	: Killable(owner)
{
	onKill = callback;
}

Killable::Killable(Game::Entity& owner, OnKillCallback callback, CheckKillCallback checkKill)
	: Killable(owner, callback)
{
	checkKillProgress = checkKill;
}

void Killable::kill() {
	if (!killed) {
		killed = true;
		deathClock.restart();
		if (onKill)
			onKill();
	}
}

bool Killable::isKillInProgress() const {
	if (!killed) return false;
	return checkKillProgress ? checkKillProgress() : false;
}
