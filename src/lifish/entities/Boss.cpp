#include "Boss.hpp"
#include "ZIndexed.hpp"
#include "game.hpp"
#include "Clock.hpp"
#include "BossExplosion.hpp"
#include "Spawning.hpp"
#include "Collider.hpp"
#include "Killable.hpp"
#include "GameCache.hpp"
#include "Sounded.hpp"
#include "Explosion.hpp"
#include "Lifed.hpp"
#include "game_values.hpp"
#include "Foe.hpp"
#include <cassert>

#include <iostream>

using Game::Boss;
using Game::TILE_SIZE;

Boss::Boss(const sf::Vector2f& pos)
	: Game::Entity(pos)
{
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::BOSSES));
	addComponent(new Game::Foe(*this));
	explClock = addComponent(new Game::Clock(*this));
	deathClock = addComponent(new Game::Clock(*this));
	killable = addComponent(new Game::Killable(*this, [this] () {
		// on kill
		_kill();
	}, [this] () {
		// kill in progress
		return _killInProgress();
	}));
	addComponent(new Game::Spawning(*this, [this] (const Game::Spawning&) {
		return killable && killable->isKilled() 
			&& explClock->getElapsedTime() >= sf::milliseconds(100);
	}, [this] () {
		explClock->restart();
		// Calculate a random location inside the boss
		const auto bpos = position;
		std::uniform_real_distribution<float> distX(-0.5 * TILE_SIZE,
		                                            TILE_SIZE * (collider->getSize().x/TILE_SIZE - 0.5)),
		                                      distY(-0.5 * TILE_SIZE,
		                                            TILE_SIZE * (collider->getSize().y/TILE_SIZE - 0.5));
		const float x = distX(rng),
		            y = distY(rng);
		auto expl = new Game::BossExplosion(sf::Vector2f(bpos.x + x, bpos.y + y));
		Game::cache.playSound(expl->get<Game::Sounded>()->getSoundFile(Game::Sounds::DEATH));
		return expl;
	}));
}

void Boss::_hurt() {
	isHurt = true;
	wasHurt = true;
}

void Boss::_kill() {
	deathClock->restart();
	collider->setLayer(Game::Layers::DEFAULT);
}

bool Boss::_killInProgress() const {
	return deathClock->getElapsedTime() < Game::Conf::Boss::DEATH_TIME;
}

void Boss::update() {
	Game::Entity::update();

	if (wasHurt)
		wasHurt = false;
	else 
		isHurt = false;
}

void Boss::_checkCollision(Game::Collider& coll) {
	if (coll.getLayer() != Game::Layers::EXPLOSIONS) return;

	auto& expl = static_cast<Game::Explosion&>(coll.getOwnerRW());
	if (expl.hasDamaged(this)) return;

	assert(collider != nullptr);
	
	// Calculate how many explosion tiles overlap with boss's ones
	const auto brect = collider->getRect();
	const auto crect = coll.getRect();
	const short x = std::max(brect.left, crect.left),
	            wx = std::min(brect.left + brect.width, crect.left + crect.width),
	            y = std::max(brect.top, crect.top),
	            wy = std::min(brect.top + brect.height, crect.top + crect.height);
	
	assert(x > 0 && wx > 0 && y > 0 && wy > 0);

	//std::cerr << "brect.left + brect.width = " << brect.left/Game::TILE_SIZE << " + " << brect.width /Game::TILE_SIZE<< " = "
		//<< (brect.left + brect.width) / Game::TILE_SIZE << std::endl;
	//std::cerr << "crect.left + crect.width = " << crect.left/Game::TILE_SIZE << " + " << crect.width /Game::TILE_SIZE<< " = "
		//<< (crect.left + crect.width) / Game::TILE_SIZE << std::endl;
	//std::cerr << "brect.top + brect.width = " << brect.top/Game::TILE_SIZE << " + " << brect.width /Game::TILE_SIZE<< " = "
		//<< (brect.top + brect.width) / Game::TILE_SIZE << std::endl;
	//std::cerr << "crect.top + crect.width = " << crect.top/Game::TILE_SIZE << " + " << crect.width /Game::TILE_SIZE<< " = "
		//<< (crect.top + crect.width) / Game::TILE_SIZE << std::endl;
	//std::cerr << "x = " << x / Game::TILE_SIZE<< ", wx = " << wx /Game::TILE_SIZE<< std::endl;
	//std::cerr << "y = " << y / Game::TILE_SIZE<< ", wy = " << wy/Game::TILE_SIZE << std::endl;
	const unsigned damage = std::round(float(wx - x) / Game::TILE_SIZE) 
					* std::round(float(wy - y) / Game::TILE_SIZE) * expl.getDamage();
	
	//std::cerr << "dealt " << damage << " damage\n";
	if (get<Game::Lifed>()->decLife(damage) > 0)
		_hurt();
	else
		killable->kill();
	expl.dealDamageTo(this);
	Game::cache.playSound(get<Game::Sounded>()->getSoundFile(Game::Sounds::HURT));
}
