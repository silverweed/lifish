#include "Explosion.hpp"
#include "LevelManager.hpp"
#include "BreakableWall.hpp"
#include "Animated.hpp"
#include "Direction.hpp"
#include "Collider.hpp"
#include "Scored.hpp"
#include "Player.hpp"
#include "utils.hpp"
#include "game.hpp"
#include "Drawable.hpp"
#include "GameCache.hpp"
#include "Temporary.hpp"
#include "ZIndexed.hpp"
#include "Sounded.hpp"
#include "CompoundCollider.hpp"
#include <list>
#include <algorithm>

using Game::Explosion;
using Game::TILE_SIZE;
using Game::Direction;

Explosion::Explosion(const sf::Vector2f& pos, unsigned short _radius, 
		const Game::Entity *const source, unsigned short damage)
	: Game::Entity(pos)
	, radius(_radius)
	, damage(damage)
	, sourceEntity(source)
{
	addComponent(new Game::Sounded(*this, { Game::getAsset("sounds", "explosion.ogg") }));
	explosionC = addComponent(new Game::Animated(*this, Game::getAsset("graphics", "explosionC.png")));
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::EXPLOSIONS));
	explosionC->addAnimation("explode", {
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE)
	}, true);
	explosionV = addComponent(new Game::Animated(*this, Game::getAsset("graphics", "explosionV.png")));
	explosionV->getTexture()->setRepeated(true);
	explosionH = addComponent(new Game::Animated(*this, Game::getAsset("graphics", "explosionH.png")));
	explosionH->getTexture()->setRepeated(true);

	addComponent(new Game::Drawable(*this, *this));

	explosionC->getSprite().setFrameTime(sf::seconds(0.05));
	explosionH->getSprite().setFrameTime(sf::seconds(0.05));
	explosionV->getSprite().setFrameTime(sf::seconds(0.05));
	explosionC->getSprite().setLooped(false);
	explosionH->getSprite().setLooped(false);
	explosionV->getSprite().setLooped(false);

	addComponent(new Game::Temporary(*this, [this] () {
		return !explosionC->getSprite().isPlaying();
	}));

	propagation.fill(0);
}

Game::Explosion* Explosion::propagate(Game::LevelManager& lm) {
	const sf::Vector2i m_tile = Game::tile(position);
	bool propagating[] = { true, true, true, true };
	bool blocked[] = { false, false, false, false };
	auto& entities = lm.getEntities();
	
	for (unsigned short dir = 0; dir < 4; ++dir) {	
		for (unsigned short r = 1; r <= radius; ++r) {
			if (!propagating[dir]) continue;

			sf::Vector2i new_tile = m_tile;
			switch (dir) {
			case Direction::UP:
				new_tile.y -= r;
				break;
			case Direction::LEFT:
				new_tile.x -= r;
				break;
			case Direction::DOWN:
				new_tile.y += r;
				break;
			case Direction::RIGHT:
				new_tile.x += r;
				break;
			}
			
			if (new_tile.x < 1 || new_tile.x > Game::LEVEL_WIDTH 
					|| new_tile.y < 1 || new_tile.y > Game::LEVEL_HEIGHT) {
				propagating[dir] = false;
				continue;
			}

			++propagation[dir];

			// Check if a solid fixed entity blocks propagation in this direction
			auto fxdlist = entities.getFixedAt(new_tile.x, new_tile.y);
			for (const auto& fxd : fxdlist) {
				const auto fxdcld = fxd.get().get<Game::Collider>();
				if (fxdcld != nullptr && Game::Layers::solid[fxdcld->getLayer()][Game::Layers::EXPLOSIONS]) { 
					propagating[dir] = false;
					blocked[dir] = true;
					break;
				}
			}
		}
	}

	/* Here be some shanenigans. If the explosion is blocked in some direction due to solid entities,
	 * we want to both tell those entities they impacted with an Explosion and to tell Explosion
	 * not to propagate further in that direction. Therefore, we create a collider which only
	 * extends for 1px to the solid entity we encountered (enough for the collision to happen, but
	 * also prevents accidental impacts with other entities beyond the solid one), then we reduce
	 * the propagation by 1 tile so that the explosion sprites aren't drawn in the solid entity's tile,
	 * which would be ugly.
	 */
	explCollider = addComponent(new Game::CompoundCollider(*this, Game::Layers::EXPLOSIONS, {
		Game::Collider(*this, Game::Layers::EXPLOSIONS,
			// size
			sf::Vector2i(
				TILE_SIZE * (propagation[Direction::LEFT] + propagation[Direction::RIGHT] + 1)
					- (blocked[Direction::RIGHT] ? TILE_SIZE - 1 : 0),
				TILE_SIZE - 2),
			// offset
			sf::Vector2f(-TILE_SIZE * propagation[Direction::LEFT], 1)),
		Game::Collider(*this, Game::Layers::EXPLOSIONS,
			// size
			sf::Vector2i(
				TILE_SIZE - 2,
				TILE_SIZE * (propagation[Direction::UP] + propagation[Direction::DOWN] + 1)
					- (blocked[Direction::DOWN] ? TILE_SIZE - 1 : 0)),
			// offset
			sf::Vector2f(1, -TILE_SIZE * propagation[Direction::UP]))
	}));

	for (unsigned short i = 0; i < 4; ++i)
		if (blocked[i]) --propagation[i];
	_setPropagatedAnims();

	lm.getEntities().refresh(*this);

	return this;
}

void Explosion::_setPropagatedAnims() {
	const unsigned short hsize = TILE_SIZE * (propagation[Direction::RIGHT] + propagation[Direction::LEFT] + 1);
	explosionH->addAnimation("explode", {
		sf::IntRect(0, 0, hsize, TILE_SIZE),
		sf::IntRect(0, TILE_SIZE, hsize, TILE_SIZE),
		sf::IntRect(0, 2 * TILE_SIZE, hsize, TILE_SIZE),
		sf::IntRect(0, 3 * TILE_SIZE, hsize, TILE_SIZE),
		sf::IntRect(0, 2 * TILE_SIZE, hsize, TILE_SIZE),
		sf::IntRect(0, TILE_SIZE, hsize, TILE_SIZE),
		sf::IntRect(0, 0, hsize, TILE_SIZE)
	}, true);
	const unsigned short vsize = TILE_SIZE * (propagation[Direction::DOWN] + propagation[Direction::UP] + 1);
	explosionV->addAnimation("explode", {
		sf::IntRect(0, 0, TILE_SIZE, vsize),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, vsize),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, vsize),
		sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, vsize),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, vsize),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, vsize),
		sf::IntRect(0, 0, TILE_SIZE, vsize)
	}, true);
	explosionH->setPosition(position - sf::Vector2f(TILE_SIZE * propagation[Direction::LEFT], 0));
	explosionV->setPosition(position - sf::Vector2f(0, TILE_SIZE * propagation[Direction::UP]));
}

void Explosion::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	// Draw horizontal
	window.draw(*explosionH, states);
	// Draw vertical
	window.draw(*explosionV, states);
	// Draw center
	window.draw(*explosionC, states);
}

void Explosion::dealDamageTo(const Game::Entity* entity) {
	damagedEntities.insert(entity);
}

bool Explosion::hasDamaged(const Game::Entity* entity) const {
	return damagedEntities.find(entity) != damagedEntities.end();
}
