#pragma once

#include "Component.hpp"

namespace Game {

/**
 * An object with a life
 */
class Lifed : public Game::Component {
protected:
	short life;
	const short maxLife;
public:
	explicit Lifed(Game::Entity& owner, const short life)
		: Game::Component(owner)
		, life(life) 
		, maxLife(life) 
	{}

	bool isAlive() const { return life > 0; }
	short getLife() const { return life; }
	short getMaxLife() const { return maxLife; }
	short decLife(short dam) { life -= dam; return life; }
	void setLife(short lf) { life = lf; }
};

}
