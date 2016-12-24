#include "AlienPredator.hpp"
#include "Clock.hpp"
#include "Shooting.hpp"
#include "LevelManager.hpp"
#include "Animated.hpp"
#include "Player.hpp"
#include "Spawning.hpp"
#include "AxisMoving.hpp"
#include "AI.hpp"
#include "MovingAnimator.hpp"
#include "Pond.hpp"
#include "Temporary.hpp"
#include "core.hpp"
#include <list>
#include <limits>

using Game::AlienPredator;
using Game::TILE_SIZE;

const sf::Time AlienPredator::POND_LIFETIME = sf::seconds(5);
const sf::Time AlienPredator::TUNNEL_PERIOD = sf::seconds(20);
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
	// Add an initial random time
	std::uniform_real_distribution<float> dist(0, TUNNEL_PERIOD.asSeconds());
	tunnelClock->add(sf::seconds(dist(Game::rng)));
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
	} else if (shooting->isShooting()) {
		tunnelClock->restart();
	} else if (tunnelClock->getElapsedTime() > TUNNEL_PERIOD) {
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

	/* Find the list of viable positions, excluding:
	 * 1. tiles containing a solid entity
	 * 2. tiles which are nearby a moving solid entity
	 * 3. tiles which are too close to a player
	 */
	// First, construct the list of possible tiles.
	std::list<sf::Vector2i> tiles;
	for (unsigned short i = 1; i <= Game::LEVEL_WIDTH; ++i)
		for (unsigned short j = 1; j <= Game::LEVEL_HEIGHT; ++j)
			tiles.push_back(sf::Vector2i(i, j));

	// Remove all tiles which are too close to a player
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
		const auto player = lm.getPlayer(i + 1);
		if (player != nullptr) {
			const auto ptile = Game::tile(player->getPosition());
			for (auto it = tiles.begin(); it != tiles.end(); ) {
				if (Game::manhattanDistance(*it, ptile) < 5)
					it = tiles.erase(it);
				else
					++it;
			}
		}
	}

	// Then exclude all tiles occupied by solid entities or nearby moving solid entities
	lm.getEntities().apply([this, &tiles] (const Game::Entity *e) {
		if (tiles.size() == 0) return;

		const auto cld = e->get<Game::Collider>();
		if (cld == nullptr || !cld->isSolidFor(*collider))
			return;
	
		sf::Vector2i etile(Game::tile(e->getPosition()));
		tiles.remove(etile);
		const auto mv = e->get<Game::AxisMoving>();
		if (mv == nullptr)
			return;

		for (unsigned short i = 0; i < 3; ++i) {
			switch (mv->getDirection()) {
			case Game::Direction::UP:
				--etile.y;
				break;
			case Game::Direction::DOWN:
				++etile.y;
				break;
			case Game::Direction::LEFT:
				--etile.x;
				break;
			case Game::Direction::RIGHT:
				++etile.x;
				break;
			default:
				return;
			}
			tiles.remove(etile);
		}
	});

	if (tiles.size() == 0)
		return newPos;

	std::uniform_int_distribution<short> dist(0, tiles.size());
	auto it = tiles.begin();
	std::advance(it, dist(Game::rng));

	// FIXME: this should never happen, but it does in some cases.
	if (it->x < 1 || it->y < 1 || it->x > Game::LEVEL_WIDTH || it->y > Game::LEVEL_HEIGHT)
		return newPos;

	return sf::Vector2f(int(TILE_SIZE) * *it);
}
