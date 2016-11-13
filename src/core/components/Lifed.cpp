#include "Lifed.hpp"
#include <exception>
#include <sstream>

using Game::Lifed;

Lifed::Lifed(Game::Entity& owner, const short life)
	: Game::Component(owner)
	, life(life) 
	, maxLife(life) 
{
	if (life < 0)
		throw std::invalid_argument("Lifed: maxLife cannot be smaller than 0!");
}

std::string Lifed::toString() const {
	std::stringstream ss;
	ss << "Life: " << life << " / " << maxLife;
	return ss.str();
}
