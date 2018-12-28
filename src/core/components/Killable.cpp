#include "Killable.hpp"
#include "Time.hpp"
#include "core.hpp"

using lif::Killable;

Killable::Killable(lif::Entity& owner)
	: lif::Component(owner)
{
	_declComponent<Killable>();
}

Killable::Killable(lif::Entity& owner, OnKillCallback callback)
	: Killable(owner)
{
	_declComponent<Killable>();
	onKill = callback;
}

Killable::Killable(lif::Entity& owner, OnKillCallback callback, CheckKillCallback checkKill)
	: Killable(owner, callback)
{
	_declComponent<Killable>();
	checkKillProgress = checkKill;
}

void Killable::kill() {
	if (!killed) {
		killed = true;
		deathTime = lif::time.getGameTime();
		if (onKill)
			onKill();
	}
}

bool Killable::isKillInProgress() const {
	if (!killed) return false;
	return checkKillProgress ? checkKillProgress() : false;
}

sf::Time Killable::timeSinceDeath() const {
	return lif::time.getGameTime() - deathTime;
}
