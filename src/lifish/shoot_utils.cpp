#include "shoot_utils.hpp"
#include "FreeSighted.hpp"
#include "Player.hpp"
#include "AutoShooting.hpp"

void lif::shootToNearestPlayer(lif::Entity& shooter) {
	const auto player = shooter.get<lif::FreeSighted>()->nearest<lif::Player>();
	if (player.expired() || player.lock() == nullptr) return;

	const auto& ppos = player.lock()->getPosition();
	shooter.get<lif::AutoShooting>()->shoot(lif::angleBetween(shooter.getPosition(), ppos));
}
