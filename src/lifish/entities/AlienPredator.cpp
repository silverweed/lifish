#include "AlienPredator.hpp"
#include "Clock.hpp"
#include "LevelManager.hpp"
#include "Animated.hpp"
#include "Spawning.hpp"
#include "AxisMoving.hpp"
#include "AI.hpp"
#include "MovingAnimator.hpp"
#include "Pond.hpp"
#include "Temporary.hpp"
#include "core.hpp"

using Game::AlienPredator;
using Game::TILE_SIZE;

const sf::Time AlienPredator::POND_LIFETIME = sf::seconds(5);
const sf::Time AlienPredator::TUNNEL_PERIOD = sf::seconds(3);
const sf::Time AlienPredator::TUNNEL_TRANSITION_TIME = sf::seconds(1);

AlienPredator::AlienPredator(const sf::Vector2f& pos, const Game::EnemyInfo& info) : Game::Enemy(pos, 10, info) {
	addComponent(new Game::Spawning(*this, [this] () {
		// Spawn Acid Pond on death, which persists for N seconds.
		auto pond = new Game::Pond(position, "acid_pond.png",
					sf::Vector2f(TILE_SIZE, TILE_SIZE),
					1, // damage
					{ Game::Layers::PLAYERS });
		sf::Clock clock;
		pond->addComponent(new Game::Temporary(*pond, [clock] () {
			return clock.getElapsedTime() > POND_LIFETIME;
		}));
		return pond;
	}));
	tunnelClock = addComponent(new Game::Clock(*this));
	tunnelAnimClock = addComponent(new Game::Clock(*this));
	// Add the tunneling animation
	auto& a_tunnel = animated->addAnimation("tunnel");
	for (unsigned short i = 0; i < TUNNEL_N_FRAMES; ++i) {
		a_tunnel.addFrame(sf::IntRect(
					(6 + i) * TILE_SIZE,
					2 * TILE_SIZE,
					TILE_SIZE, TILE_SIZE));
	}
}

void AlienPredator::update() {
	Game::Enemy::update();

	if (tunneling) {
		const auto t = tunnelAnimClock->getElapsedTime();
		if (t > TUNNEL_TRANSITION_TIME) {
			tunneling = false;
			tunnelChangedPosition = false;
			animated->setAnimation("walk_down");
			tunnelClock->restart();
			movingAnimator->setActive(true);
			moving->setDirection(Game::Direction::DOWN);
		} else if (!tunnelChangedPosition && t > TUNNEL_TRANSITION_TIME / 2.f) {
			position = _findTunneledPosition(*get<Game::AI>()->getLevelManager());
			tunnelChangedPosition = true;
		}
	} else if (tunnelClock->getElapsedTime() > TUNNEL_PERIOD) {
		// TODO: reset tunnel clock when shooting
		tunnelAnimClock->restart();
		tunneling = true;
		moving->stop();
		movingAnimator->setActive(false);
		animated->setAnimation("tunnel");
	}
}

sf::Vector2f AlienPredator::_findTunneledPosition(const Game::LevelManager& lm) const {
	// If finding a new position fails, fallback to the current one.
	sf::Vector2f newPos(position);

	// TODO

	return newPos + sf::Vector2f(32, 32);
}
