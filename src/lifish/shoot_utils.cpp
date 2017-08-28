#include "shoot_utils.hpp"
#include "FreeSighted.hpp"
#include "Player.hpp"
#include "Shooting.hpp"

void lif::shootToNearestPlayer(lif::Entity& shooter) {
	const auto player = shooter.get<lif::FreeSighted>()->nearest<lif::Player>();
	if (player == nullptr) return;

	const auto& ppos = player->getPosition();
	shooter.get<lif::Shooting>()->shoot(lif::angleBetween(shooter.getPosition(), ppos));
}
