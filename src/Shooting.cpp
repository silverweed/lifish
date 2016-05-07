#include "Shooting.hpp"

using Game::Shooting;

bool Game::stringToAttackType(const std::string& str, AttackType& type) {
	if (str == "simple") type = AttackType::SIMPLE;
	else if (str == "contact") type = AttackType::CONTACT;
	else if (str == "ranged") type = AttackType::RANGED;
	else if (str == "blocking") type = AttackType::BLOCKING;
	else return false;

	return true;
}

Game::Bullet* Shooting::shoot() {
	shooting = true;
	return new Game::Bullet(
			owner,
			attack);
}

bool Shooting::isRecharging() const {
	return attack.fireRate > 0 && rechargeClock->getElapsedTime().asSeconds() < 1./attack.fireRate;
}
