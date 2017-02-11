#include "Killable.hpp"

using lif::Killable;

Killable::Killable(lif::Entity& owner)
	: lif::Component(owner)
{
	keys.emplace_back(_getKey<Killable>());
}

Killable::Killable(lif::Entity& owner, OnKillCallback callback)
	: Killable(owner)
{
	keys.emplace_back(_getKey<Killable>());
	onKill = callback;
}

Killable::Killable(lif::Entity& owner, OnKillCallback callback, CheckKillCallback checkKill)
	: Killable(owner, callback)
{
	keys.emplace_back(_getKey<Killable>());
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
