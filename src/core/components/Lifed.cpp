#include "Lifed.hpp"
#include <exception>
#include <sstream>

using lif::Lifed;

Lifed::Lifed(lif::Entity& owner, const int life)
	: lif::Component(owner)
	, life(life) 
	, maxLife(life) 
{
	if (life < 0)
		throw std::invalid_argument("Lifed: maxLife cannot be smaller than 0!");
}

Lifed::Lifed(lif::Entity& owner, const int life, OnHurtCallback _onHurt)
	: lif::Lifed(owner, life)
{
	onHurt = _onHurt;
}

std::string Lifed::toString() const {
	std::stringstream ss;
	ss << "Life: " << life << " / " << maxLife;
	return ss.str();
}

int Lifed::decLife(int dam, bool triggerCallback) {
	life -= dam > life ? life : dam;

	if (triggerCallback && onHurt)
		onHurt(life);

	return life;
}
