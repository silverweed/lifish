#include "Boss.hpp"
#include "ZIndexed.hpp"
#include "game.hpp"
#include "Collider.hpp"
#include "Killable.hpp"
#include "GameCache.hpp"
#include "Sounded.hpp"
#include "Explosion.hpp"
#include "Lifed.hpp"
#include "game_values.hpp"
#include <cassert>

#include <iostream>

using Game::Boss;

Boss::Boss(const sf::Vector2f& pos)
	: Game::Entity(pos)
{
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::BOSSES));
}

void Boss::_hurt() {
	isHurt = true;
	wasHurt = true;
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

	std::cerr << "brect.left + brect.width = " << brect.left/Game::TILE_SIZE << " + " << brect.width /Game::TILE_SIZE<< " = "
		<< (brect.left + brect.width) / Game::TILE_SIZE << std::endl;
	std::cerr << "crect.left + crect.width = " << crect.left/Game::TILE_SIZE << " + " << crect.width /Game::TILE_SIZE<< " = "
		<< (crect.left + crect.width) / Game::TILE_SIZE << std::endl;
	std::cerr << "brect.top + brect.width = " << brect.top/Game::TILE_SIZE << " + " << brect.width /Game::TILE_SIZE<< " = "
		<< (brect.top + brect.width) / Game::TILE_SIZE << std::endl;
	std::cerr << "crect.top + crect.width = " << crect.top/Game::TILE_SIZE << " + " << crect.width /Game::TILE_SIZE<< " = "
		<< (crect.top + crect.width) / Game::TILE_SIZE << std::endl;
	std::cerr << "x = " << x / Game::TILE_SIZE<< ", wx = " << wx /Game::TILE_SIZE<< std::endl;
	std::cerr << "y = " << y / Game::TILE_SIZE<< ", wy = " << wy /Game::TILE_SIZE<< std::endl;
	const unsigned int damage = (wx - x) / Game::TILE_SIZE * (wy - y) / Game::TILE_SIZE * expl.getDamage();
	
	std::cerr << "dealt " << damage << " damage\n";
	get<Game::Lifed>()->decLife(damage);
	_hurt();
	expl.dealDamageTo(this);
	Game::cache.playSound(get<Game::Sounded>()->getSoundFile(Game::Sounds::HURT));
}
