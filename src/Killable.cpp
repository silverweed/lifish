#include "Killable.hpp"

using Game::Killable;

Killable::Killable(Game::Entity *const owner)
	: Game::Component(owner)
{}

Killable::Killable(Game::Entity *const owner, Callback callback)
	: Killable(owner)
{
	onKill = callback;
}

Killable::Killable(Game::Entity *const owner, Callback callback, CheckKillCallback checkKill)
	: Killable(owner, callback)
{
	checkKillProgress = checkKill;
}

void Killable::kill() {
	if (!killed) {
		killed = true;
		if (onKill)
			onKill(this);
	}
}

bool Killable::isKillInProgress() const {
	if (!isKilled) return false;

	return checkKillProgress ? checkKillProgress(this) : false;
}
