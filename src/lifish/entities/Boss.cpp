#include "Boss.hpp"
#include "Absorbable.hpp"
#include "Animated.hpp"
#include "Bonusable.hpp"
#include "BossExplosion.hpp"
#include "Collider.hpp"
#include "Drawable.hpp"
#include "Explosion.hpp"
#include "Foe.hpp"
#include "GameCache.hpp"
#include "HurtDrawProxy.hpp"
#include "Killable.hpp"
#include "Lifed.hpp"
#include "Player.hpp"
#include "Sounded.hpp"
#include "Spawning.hpp"
#include "Time.hpp"
#include "ZIndexed.hpp"
#include "conf/boss.hpp"
#include "conf/player.hpp"
#include "conf/zindex.hpp"
#include "game.hpp"
#include <cassert>

using lif::Boss;
using lif::TILE_SIZE;

Boss::Boss(const sf::Vector2f& pos)
	: lif::Entity(pos)
{
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::BOSSES);
	addComponent<lif::Foe>(*this);
	killable = addComponent<lif::Killable>(*this, [this] () {
		// on kill
		_kill();
	}, [this] () {
		// kill in progress
		return deathT < lif::conf::boss::DEATH_TIME;
	});
	addComponent<lif::Spawning>(*this, [this] (const lif::Spawning&) {
		return killable && killable->isKilled()
			&& explT >= sf::milliseconds(100);
	}, [this] () {
		explT = sf::Time::Zero;
		// Calculate a random location inside the boss
		const auto bpos = collider->getPosition();
		std::uniform_real_distribution<float> distX(-0.5 * TILE_SIZE,
		                                            TILE_SIZE * (collider->getSize().x/TILE_SIZE - 0.5)),
		                                      distY(-0.5 * TILE_SIZE,
		                                            TILE_SIZE * (collider->getSize().y/TILE_SIZE - 0.5));
		const float x = distX(lif::rng),
		            y = distY(lif::rng);
		auto expl = new lif::BossExplosion(sf::Vector2f(bpos.x + x, bpos.y + y));
		lif::cache.playSound(expl->get<lif::Sounded>()->getSoundFile("explode"));
		return expl;
	});
	addComponent<lif::Absorbable>(*this);
}

lif::Entity* Boss::init() {
	lif::Entity::init();
	if (collider == nullptr)
		throw std::logic_error("Collider is null for " + toString() + "!");
	return this;
}

void Boss::update() {
	lif::Entity::update();

	const auto delta = lif::time.getDelta();
	explT += delta;
	deathT += delta;
}

void Boss::_kill() {
	deathT = sf::Time::Zero;
	collider->setLayer(lif::c_layers::DEFAULT);
}

void Boss::_checkCollision(lif::Collider& coll) {
	// One-shot players
	if (coll.getLayer() == lif::c_layers::PLAYERS) {
		auto& player = static_cast<lif::Player&>(coll.getOwnerRW());
		if (!player.get<lif::Bonusable>()->hasBonus(lif::BonusType::SHIELD)) {
			player.dealDamage(lif::conf::player::MAX_LIFE, true);
		}
		return;
	}

	if (!vulnerable || coll.getLayer() != lif::c_layers::EXPLOSIONS) return;

	auto& expl = static_cast<lif::Explosion&>(coll.getOwnerRW());
	if (expl.hasDamaged(*this)) return;

	const auto& explColliders = expl.getAll<lif::Collider>();

	// Calculate how many explosion tiles overlap with boss's ones
	const auto brect = collider->getRect();
	int overlapped = std::accumulate(explColliders.begin(), explColliders.end(), 0,
		[&brect] (int acc, const auto& cld) { return acc + lif::nOverlappedTiles(brect, cld->getRect()); });
	overlapped = std::min(overlapped, lif::conf::boss::MAX_TILES_CONSIDERED_FOR_BOMB_DAMAGE);

	const unsigned damage = overlapped * expl.getDamage();
	if (get<lif::Lifed>()->decLife(damage) > 0)
		get<lif::HurtDrawProxy>()->hurt();
	else
		killable->kill();
	expl.dealDamageTo(*this);
	lif::cache.playSound(get<lif::Sounded>()->getSoundFile("hurt"));
}

void Boss::_addDefaultCollider(const sf::Vector2f& size) {
	collider = addComponent<lif::Collider>(*this, [this] (lif::Collider& coll) {
		// on collision
		if (!(killable && killable->isKilled()))
			_checkCollision(coll);
	}, lif::c_layers::BOSSES, size);
	if (get<lif::HurtDrawProxy>() == nullptr) {
		throw std::logic_error("You need to add a HurtDrawProxy to this Boss.");
	}
	if (get<lif::Lifed>() == nullptr) {
		throw std::logic_error("You need to add a Lifed to this Boss.");
	}
	if (get<lif::Sounded>() == nullptr) {
		throw std::logic_error("You need to add a Sounded to this Boss.");
	}
}
