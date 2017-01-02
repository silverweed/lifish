#include "shoot_utils.hpp"
#include "FreeSighted.hpp"
#include "Player.hpp"
#include "AutoShooting.hpp"

void Game::shootToNearestPlayer(Game::Entity& shooter) {
	const auto player = shooter.get<Game::FreeSighted>()->nearest<Game::Player>();
	if (player.expired() || player.lock() == nullptr) return;

	const auto& ppos = player.lock()->getPosition();

	// calculate angle with ppos: a = pi - arctan(dy / dx)
	const double dx = shooter.getPosition().x - ppos.x,
		     dy = ppos.y - shooter.getPosition().y;
	const auto angle = Game::radians(Game::PI - std::atan2(dy, dx));

	shooter.get<Game::AutoShooting>()->shoot(angle);
}
