#pragma once

#include "Component.hpp"

namespace Game {

/**
 * An object with a life. Life cannot be < 0.
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
	{
		if (life < 0)
			throw std::invalid_argument("Lifed: maxLife cannot be smaller than 0!");
	}

	bool isAlive() const { return life > 0; }
	short getLife() const { return life; }
	short getMaxLife() const { return maxLife; }
	short decLife(short dam) { life -= dam > life ? life : dam; return life; }
	void setLife(short lf) { life = lf < 0 ? 0 : lf; }
};

}
