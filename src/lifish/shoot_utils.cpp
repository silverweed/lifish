#include "shoot_utils.hpp"
#include "FreeSighted.hpp"
#include "Player.hpp"
#include "AutoShooting.hpp"

void lif::shootToNearestPlayer(lif::Entity& shooter) {
	const auto player = shooter.get<lif::FreeSighted>()->nearest<lif::Player>();
	if (player.expired() || player.lock() == nullptr) return;

	const auto& ppos = player.lock()->getPosition();

	// calculate angle with ppos: a = pi - arctan(dy / dx)
	const double dx = shooter.getPosition().x - ppos.x,
		     dy = ppos.y - shooter.getPosition().y;
	const auto angle = lif::radians(lif::PI - std::atan2(dy, dx));

	shooter.get<lif::AutoShooting>()->shoot(angle);
}
