#include "Lifed.hpp"

using Game::Lifed;

Lifed::Lifed(Game::Entity *const owner, const short life)
	: Game::Component(owner)
	, life(life) 
	, maxLife(life) 
{}

void Lifed::decLife(const short dam) {
	life -= dam;
}

void Lifed::setLife(const short lf) {
	life = lf;
}
