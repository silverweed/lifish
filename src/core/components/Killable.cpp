#include "Killable.hpp"

using lif::Killable;

Killable::Killable(lif::Entity& owner)
	: lif::Component(owner)
{}

Killable::Killable(lif::Entity& owner, OnKillCallback callback)
	: Killable(owner)
{
	onKill = callback;
}

Killable::Killable(lif::Entity& owner, OnKillCallback callback, CheckKillCallback checkKill)
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
