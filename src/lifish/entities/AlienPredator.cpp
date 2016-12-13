#include "AlienPredator.hpp"
#include "Spawning.hpp"
#include "Pond.hpp"
#include "Temporary.hpp"

using Game::AlienPredator;

const sf::Time AlienPredator::POND_LIFETIME = sf::seconds(5);

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
}
