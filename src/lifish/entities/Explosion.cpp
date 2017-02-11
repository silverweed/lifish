#include "Explosion.hpp"
#include "LevelManager.hpp"
#include "BreakableWall.hpp"
#include "Fire.hpp"
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
#include "conf/zindex.hpp"
#include "conf/bonus.hpp"
#include "Sounded.hpp"
#include "BufferedSpawner.hpp"
#include "LightSource.hpp"
#include <list>
#include <algorithm>

using lif::Explosion;
using lif::TILE_SIZE;
using lif::Direction;

Explosion::Explosion(const sf::Vector2f& pos, unsigned short _radius,
		const lif::Entity *const source, bool isIncendiary, int damage)
	: lif::Entity(pos)
	, radius(_radius)
	, damage(damage)
	, sourceEntity(source)
{
	explosionC = addComponent(std::make_shared<lif::Animated>(*this, lif::getAsset("graphics", "explosionC.png")));
	lightSource = addComponent(std::make_shared<lif::LightSource>(*this, (radius + 0.5) * TILE_SIZE));
	addComponent(std::make_shared<lif::ZIndexed>(*this, lif::conf::zindex::EXPLOSIONS));
	explosionC->addAnimation("explode", {
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE)
	}, true);
	explosionV = addComponent(std::make_shared<lif::Animated>(*this, lif::getAsset("graphics", "explosionV.png")));
	explosionV->getTexture()->setRepeated(true);
	explosionH = addComponent(std::make_shared<lif::Animated>(*this, lif::getAsset("graphics", "explosionH.png")));
	explosionH->getTexture()->setRepeated(true);

	addComponent(std::make_shared<lif::Drawable>(*this, *this));

	explosionC->getSprite().setFrameTime(sf::seconds(0.05));
	explosionH->getSprite().setFrameTime(sf::seconds(0.05));
	explosionV->getSprite().setFrameTime(sf::seconds(0.05));
	explosionC->getSprite().setLooped(false);
	explosionH->getSprite().setLooped(false);
	explosionV->getSprite().setLooped(false);

	addComponent(std::make_shared<lif::Temporary>(*this, [this] () {
		// expire condition
		return !explosionC->getSprite().isPlaying();
	}, [this] () {
		// on kill: spawn fire if incendiary
		if (spawner == nullptr) return;
		spawner->addSpawned(new lif::Fire(explColliderH->getPosition(),
					explColliderH->getSize(), lif::conf::bonus::FIRE_DURATION));
		spawner->addSpawned(new lif::Fire(explColliderV->getPosition(), 
					explColliderV->getSize(), lif::conf::bonus::FIRE_DURATION));
	}));

	if (isIncendiary)
		spawner = addComponent(std::make_shared<lif::BufferedSpawner>(*this));

	propagation.fill(0);
}

void Explosion::update() {
	lif::Entity::update();
	lightSource->setColor(sf::Color(255, 255, 255,
		(4 - lif::abs(static_cast<signed>(explosionC->getSprite().getCurrentFrame()) - 3)) * 255 / 4.0));
}

Explosion* Explosion::propagate(lif::LevelManager& lm) {
	const sf::Vector2i m_tile = lif::tile(position);
	bool propagating[] = { true, true, true, true };
	bool blocked[] = { false, false, false, false };
	auto& entities = lm.getEntities();
	
	for (unsigned dir = 0; dir < 4; ++dir) {	
		for (unsigned r = 1; r <= radius; ++r) {
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
			
			if (new_tile.x < 1 || new_tile.x > lif::LEVEL_WIDTH 
					|| new_tile.y < 1 || new_tile.y > lif::LEVEL_HEIGHT) {
				propagating[dir] = false;
				continue;
			}

			++propagation[dir];

			// Check if a solid fixed entity blocks propagation in this direction
			for (auto ent : entities.getEntitiesAtTile(new_tile)) {
				const auto entcld = ent.lock()->get<lif::Collider>();
				if (entcld != nullptr && lif::c_layers::solid[entcld->getLayer()][
						lif::c_layers::EXPLOSIONS])
				{
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
	// Note: no cast required, as `true` is promoted to integral value "1" by C++ standard (§4.7 conv.integral)
	short reduction = blocked[Direction::RIGHT] + blocked[Direction::LEFT];
	explColliderH = addComponent(std::make_shared<lif::Collider>(*this, lif::c_layers::EXPLOSIONS,
			// size
			sf::Vector2f(
				TILE_SIZE * (propagation[Direction::LEFT] + propagation[Direction::RIGHT]
					+ 1 - reduction) + reduction,
				TILE_SIZE - 2),
			// offset
			sf::Vector2f(-TILE_SIZE * propagation[Direction::LEFT]
				+ (TILE_SIZE - 1) * blocked[Direction::LEFT], 1)));

	reduction = blocked[Direction::UP]  + blocked[Direction::DOWN];
	explColliderV = addComponent(std::make_shared<lif::Collider>(*this, lif::c_layers::EXPLOSIONS,
			// size
			sf::Vector2f(
				TILE_SIZE - 2,
				TILE_SIZE * (propagation[Direction::UP] + propagation[Direction::DOWN]
					+ 1 - reduction) + reduction),
			// offset
			sf::Vector2f(1, -TILE_SIZE * propagation[Direction::UP]
				+ (TILE_SIZE - 1) * blocked[Direction::UP])));

	for (unsigned i = 0; i < 4; ++i)
		if (blocked[i]) --propagation[i];
	_setPropagatedAnims();

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

void Explosion::dealDamageTo(const lif::Entity& entity) {
	damagedEntities.insert(&entity);
}

bool Explosion::hasDamaged(const lif::Entity& entity) const {
	return damagedEntities.count(&entity) > 0;
}
