#pragma once

#include <map>
#include "entities/Bullet.hpp"
#include "conf/player.hpp"

namespace lif {

namespace Conf {
	namespace Bullet {
		constexpr float BASE_SPEED = lif::Conf::Player::DEFAULT_SPEED;

		extern std::map<unsigned short, const lif::BulletData> data;
	}
}

}
