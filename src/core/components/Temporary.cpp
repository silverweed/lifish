#include "Temporary.hpp"

using lif::Temporary;

Temporary::Temporary(lif::Entity& owner, std::function<bool()> expireCondition)
	: lif::Killable(owner)
	, expireCondition(expireCondition)
{
	_declComponent<Temporary>();
}

Temporary::Temporary(lif::Entity& owner, std::function<bool()> expireCondition, OnKillCallback onKill)
	: lif::Killable(owner, onKill)
	, expireCondition(expireCondition)
{
	_declComponent<Temporary>();
}

Temporary::Temporary(lif::Entity& owner, std::function<bool()> expireCondition,
		OnKillCallback onKill, CheckKillCallback checkKill)
	: lif::Killable(owner, onKill, checkKill)
	, expireCondition(expireCondition)
{
	_declComponent<Temporary>();
}

void Temporary::update() {
	lif::Component::update();
	if (!killed && expireCondition())
		kill();
}
