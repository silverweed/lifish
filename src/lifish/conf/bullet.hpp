#pragma once

#include <map>
#include "Bullet.hpp"
#include "player.hpp"

namespace Game {

struct BulletData;

namespace Conf {
	namespace Bullet {
		constexpr float BASE_SPEED = Game::Conf::Player::DEFAULT_SPEED;

		extern std::map<unsigned short, const Game::BulletData> data;
	}
}

}
