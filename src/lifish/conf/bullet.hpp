#pragma once

#include <map>
#include "entities/Bullet.hpp"
#include "conf/player.hpp"

namespace lif {

namespace conf {
	namespace bullet {
		constexpr float BASE_SPEED = lif::conf::player::DEFAULT_SPEED;

		extern std::map<unsigned short, const lif::BulletData> data;
	}
}

}
