#pragma once

#include "Component.hpp"
#include <string>

namespace Game {

/**
 * An object with a life. Life cannot be < 0.
 */
class Lifed : public Game::Component {
protected:
	short life;
	const short maxLife;
public:
	explicit Lifed(Game::Entity& owner, const short life);

	bool isAlive() const { return life > 0; }
	short getLife() const { return life; }
	short getMaxLife() const { return maxLife; }
	/** @return Life after subtracting `dam` */
	short decLife(short dam) { life -= dam > life ? life : dam; return life; }
	void setLife(short lf) { life = lf < 0 ? 0 : lf; }
	void refillLife() { life = maxLife; }

	std::string toString() const override;
};

}
