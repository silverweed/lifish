#pragma once

#include <string>
#include <functional>
#include "Component.hpp"

namespace lif {

/**
 * An object with a life. Life cannot be < 0.
 */
class Lifed : public lif::Component {
	using OnHurtCallback = std::function<void(int damage, int newLife)>;

	OnHurtCallback onHurt;

protected:
	int life;
	const unsigned int maxLife;

public:
	explicit Lifed(lif::Entity& owner, const unsigned int life);
	explicit Lifed(lif::Entity& owner, const unsigned int life, OnHurtCallback onHurt);

	bool isAlive() const { return life > 0; }
	int getLife() const { return life; }
	int getMaxLife() const { return maxLife; }
	/** @return Life after subtracting `dam`. If `triggerCallback` is true, call `onHurt(newLife)` (if any). */
	int decLife(int dam, bool triggerCallback = true);
	void setLife(int lf) { life = lf < 0 ? 0 : lf; }
	void refillLife() { life = maxLife; }

	std::string toString() const override;
};

}
