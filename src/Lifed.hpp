#pragma once
/**
 * An object with a life
 */

namespace Game {

class Lifed {
protected:
	short life;
	const short maxLife;
public:
	Lifed(const short _life) : life(_life), maxLife(_life) {}

	short getLife() const { return life; }
	short getMaxLife() const { return maxLife; }
	void decLife(const short dam) { life -= dam; }
	void setLife(const short lf) { life = lf; }
};

}
