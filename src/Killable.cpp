#include "Killable.hpp"

using Game::Killable;

Killable::Killable(Game::Entity *const owner)
	: Game::Component(owner)
{}

Killable::Killable(Game::Entity *const owner, OnKillCallback callback)
	: Killable(owner)
{
	onKill = callback;
}

Killable::Killable(Game::Entity *const owner, OnKillCallback callback, CheckKillCallback checkKill)
	: Killable(owner, callback)
{
	checkKillProgress = checkKill;
}

void Killable::kill() {
	if (!killed) {
		killed = true;
		if (onKill)
			onKill();
	}
}

bool Killable::isKillInProgress() const {
	if (!killed) return false;

	return checkKillProgress ? checkKillProgress() : false;
}
