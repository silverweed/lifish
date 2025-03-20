#include "Explosion.hpp"
#include "Animated.hpp"
#include "BreakableWall.hpp"
#include "BufferedSpawner.hpp"
#include "Collider.hpp"
#include "Direction.hpp"
#include "Drawable.hpp"
#include "GameCache.hpp"
#include "Level.hpp"
#include "LevelManager.hpp"
#include "Player.hpp"
#include "Scored.hpp"
#include "Sounded.hpp"
#include "Temporary.hpp"
#include "ZIndexed.hpp"
#include "conf/bonus.hpp"
#include "conf/zindex.hpp"
#include "game.hpp"
#include "utils.hpp"
#include <algorithm>
#include <list>

using lif::Explosion;
using lif::TILE_SIZE;
using lif::Direction;

Explosion::Explosion(const sf::Vector2f& pos, unsigned short _radius,
		const lif::Entity *const source, int damage, lif::c_layers::Layer cLayer)
	: lif::Entity(pos)
	, cLayer(cLayer)
	, radius(_radius)
	, damage(damage)
	, sourceEntity(source)
{
	explosionC = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "explosionC.png"));
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::EXPLOSIONS);
	explosionC->addAnimation("explode", {
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(3 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(2 * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(TILE_SIZE, 0, TILE_SIZE, TILE_SIZE),
		sf::IntRect(0, 0, TILE_SIZE, TILE_SIZE)
	}, true);
	explosionV = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "explosionV.png"));
	explosionV->getTexture()->setRepeated(true);
	explosionH = addComponent<lif::Animated>(*this, lif::getAsset("graphics", "explosionH.png"));
	explosionH->getTexture()->setRepeated(true);

	addComponent<lif::Drawable>(*this, *this);

	explosionC->getSprite().setFrameTime(sf::seconds(0.05));
	explosionH->getSprite().setFrameTime(sf::seconds(0.05));
	explosionV->getSprite().setFrameTime(sf::seconds(0.05));
	explosionC->getSprite().setLooped(false);
	explosionH->getSprite().setLooped(false);
	explosionV->getSprite().setLooped(false);

	addComponent<lif::Temporary>(*this, [this] () {
		// expire condition
		return !explosionC->getSprite().isPlaying();
	}, [this] () {
		// on kill: spawn fire if incendiary
		if (spawner == nullptr) return;
		/*
		spawner->addSpawned(new lif::Fire(explColliderH->getPosition(),
					explColliderH->getSize(), lif::conf::bonus::FIRE_DURATION));
		spawner->addSpawned(new lif::Fire(explColliderV->getPosition(),
					explColliderV->getSize(), lif::conf::bonus::FIRE_DURATION));
					*/
	});

	propagation.fill(0);
}

void Explosion::update() {
	lif::Entity::update();

	if (!explosionH->isActive())
		return;

	// Deactivate the colliders slightly earlier than actual animation end
	const auto sprite = explosionC->getSprite();
	if (sprite.getAnimation()->getSize() - sprite.getCurrentFrame() < 2) {
		explColliderH->setActive(false);
		explColliderV->setActive(false);
	}
}

Explosion* Explosion::propagate(lif::LevelManager& lm) {
	const sf::Vector2i m_tile = lif::tile(position);
	const auto lvinfo = lm.getLevel()->getInfo();
	auto& entities = lm.getEntities();
	std::array<bool, 4> propagating,
	                    blocked;

	propagating.fill(true);
	blocked.fill(false);

	for (unsigned dir = 0; dir < propagating.size(); ++dir) {
		for (unsigned r = 1; r <= radius; ++r) {
			if (!propagating[dir]) continue;

			auto new_tile = m_tile;
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

			if (new_tile.x < 1 || new_tile.x > lvinfo.width
				|| new_tile.y < 1 || new_tile.y > lvinfo.height)
			{
				propagating[dir] = false;
				continue;
			}

			++propagation[dir];

			// Check if a solid fixed entity blocks propagation in this direction
			const auto rect = sf::FloatRect(new_tile.x * TILE_SIZE, new_tile.y * TILE_SIZE,
					TILE_SIZE, TILE_SIZE);
			for (const auto entcld : entities.getCollidersIntersecting(rect)) {
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
	int reduction = blocked[Direction::RIGHT] + blocked[Direction::LEFT];
	explColliderH = addComponent<lif::Collider>(*this, cLayer,
			// size
			sf::Vector2f(
				TILE_SIZE * (propagation[Direction::LEFT] + propagation[Direction::RIGHT]
					+ 1 - reduction) + reduction,
				TILE_SIZE - 2),
			// offset
			sf::Vector2f(static_cast<signed>(-TILE_SIZE * propagation[Direction::LEFT]
				+ (TILE_SIZE - 1) * blocked[Direction::LEFT]), 1));

	reduction = blocked[Direction::UP]  + blocked[Direction::DOWN];
	explColliderV = addComponent<lif::Collider>(*this, cLayer,
			// size
			sf::Vector2f(
				TILE_SIZE - 2,
				TILE_SIZE * (propagation[Direction::UP] + propagation[Direction::DOWN]
					+ 1 - reduction) + reduction),
			// offset
			sf::Vector2f(1, static_cast<signed>(-TILE_SIZE * propagation[Direction::UP]
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
