#include "Grenade.hpp"
#include "GuidedMoving.hpp"
#include "Explosion.hpp"
#include "Collider.hpp"
#include "Spawning.hpp"
#include "utils.hpp"
#include <cmath>

using lif::Grenade;
using lif::TILE_SIZE;

Grenade::Grenade(const sf::Vector2f& pos, lif::Direction dir,
		const lif::BulletInfo& info, const lif::Entity *const source)
	: lif::AxisBullet(pos, dir, info, source)
{
	moving->setActive(false);
	collider->setActive(false);

	// Calculate motion params
	auto end = pos;
	switch (dir) {
	case lif::Direction::UP:
		end.y -= info.range;
		break;
	case lif::Direction::DOWN:
		end.y += info.range;
		break;
	case lif::Direction::RIGHT:
		end.x += info.range;
		break;
	case lif::Direction::LEFT:
		end.x -= info.range;
		break;
	default:
		break;
	}
	auto timeTaken = sf::seconds(lif::manhattanDistance(end, pos) / info.speed);
	const auto bounce = [] (auto t) {
		return sf::Vector2f(0, -static_cast<signed>(lif::TILE_SIZE) *
				lif::abs(std::sin(t * 2 * lif::PI)) / std::pow(t + 1, 3));
	};
	moving = addComponent<lif::GuidedMoving>(*this, pos, end, timeTaken,
			// so much for "perfect" forwarding...
			std::make_tuple(bounce, false, false));

	addComponent<lif::Spawning>(*this, [this] () {
		return new lif::Explosion(position, 1, this->source);
	});
}

void Grenade::update() {
	lif::Entity::update();
}
