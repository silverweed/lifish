#include "Grenade.hpp"
#include "GuidedMoving.hpp"
#include "EnemyExplosion.hpp"
#include "Killable.hpp"
#include "Collider.hpp"
#include "Spawning.hpp"
#include "AxisMoving.hpp"
#include "utils.hpp"
#include <cmath>

using lif::Grenade;
using lif::TILE_SIZE;

Grenade::Grenade(const sf::Vector2f& pos, lif::Direction dir, const lif::BulletInfo& info,
		const lif::Entity *const source, const lif::Entity *const target)
	: lif::GuidedBullet(pos,
		_calculateEnd(pos, info.range, dir, target),
		info,
		sf::seconds(1.0 / info.speed),
		source)
{
	collider->setActive(false); // collide with nothing, not even level bounds
	const auto bounce = [] (auto t) {
		return sf::Vector2f(0, -lif::TILE_SIZE * lif::abs(std::sin(t * 2 * lif::PI)) / std::pow(t + 1, 3));
	};
	static_cast<lif::GuidedMoving*>(moving)->addModFunc({ bounce, false, false });
	addComponent<lif::Spawning>(*this, [this] () {
		return new lif::EnemyExplosion(lif::aligned2(position), 1, this->info.damage, this->source);
	});
}

void Grenade::update() {
	lif::Entity::update();

	if (static_cast<const lif::GuidedMoving*>(moving)->getPerc() >= 1) {
		get<lif::Killable>()->kill();
	}
}

sf::Vector2f Grenade::_calculateEnd(const sf::Vector2f& pos, float range,
		lif::Direction dir, const lif::Entity *const target) const
{
	// Calculate motion params
	auto end = pos;
	if (target != nullptr && (range < 0 || lif::manhattanDistance(target->getPosition(), pos) <= range)) {
		end = target->getPosition();
	} else {
		// Ensure the range is positive and finite
		if (range < 0) range = 6 * lif::TILE_SIZE;

		switch (dir) {
		case lif::Direction::UP:
			end.y -= range;
			break;
		case lif::Direction::DOWN:
			end.y += range;
			break;
		case lif::Direction::RIGHT:
			end.x += range;
			break;
		case lif::Direction::LEFT:
			end.x -= range;
			break;
		default:
			break;
		}
	}
	return lif::aligned2(end);
}
