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
	: lif::AxisBullet(pos, dir, info, source)
{
	moving->setActive(false);
	static_cast<lif::AxisMoving*>(moving)->setEnsureAlign(false);
	collider->setLayer(lif::c_layers::IGNORE_ALL);

	// Calculate motion params
	sf::Vector2f end;
	if (target != nullptr) {
		end = target->getPosition();
	} else {
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
	}
	end = lif::aligned2(end);
	auto timeTaken = sf::seconds(lif::manhattanDistance(end, pos) / info.speed);
	const auto bounce = [] (auto t) {
		return sf::Vector2f(0, 0);
		return sf::Vector2f(0, -static_cast<signed>(lif::TILE_SIZE) *
				lif::abs(std::sin(t * 2 * lif::PI)) / std::pow(t + 1, 3));
	};
	moving = addComponent<lif::GuidedMoving>(*this, pos, end, timeTaken,
			// so much for "perfect" forwarding...
			std::make_tuple(bounce, false, false));

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
