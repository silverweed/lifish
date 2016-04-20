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
	explicit Lifed(Game::Entity *const owner, const short life);

	bool isAlive() const { return life > 0; }
	short getLife() const { return life; }
	short getMaxLife() const { return maxLife; }
	void decLife(const short dam);
	void setLife(const short lf); 
};

}
