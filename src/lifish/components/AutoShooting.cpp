#include <exception>
#include "AutoShooting.hpp"
#include "Shooting.hpp"
#include "AxisBullet.hpp"
#include "FreeBullet.hpp"

using Game::AutoShooting;

AutoShooting::AutoShooting(Game::Entity& owner)
	: Game::Spawning(owner)
{}

Game::Entity* AutoShooting::init() {
	shooting = owner.get<Shooting>();
	if (shooting == nullptr)
		throw std::logic_error("owner of AutoShooting has no Shooting component!");
	return this;
}

void AutoShooting::shoot(Game::Direction dir) {
	latestShot.push(std::unique_ptr<Game::Bullet>(shooting->shoot(dir)));
}

void AutoShooting::shoot(double angle) {
	latestShot.push(std::unique_ptr<Game::Bullet>(shooting->shoot(angle)));
}

std::unique_ptr<Game::Entity> AutoShooting::spawn() {
	std::unique_ptr<Game::Entity> result;
	if (!latestShot.empty()) {
		result = std::unique_ptr<Game::Entity>(latestShot.front().release());
		latestShot.pop();
	}
	return result;
}

bool AutoShooting::shouldSpawn() const {
	return !latestShot.empty();
}
