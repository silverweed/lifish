#pragma once

#include <map>
#include "entities/Bullet.hpp"
#include "conf/player.hpp"

namespace Game {

namespace Conf {
	namespace Bullet {
		constexpr float BASE_SPEED = Game::Conf::Player::DEFAULT_SPEED;

		extern std::map<unsigned short, const Game::BulletData> data;
	}
}

}
