#pragma once

#include <map>
#include "entities/Bullet.hpp"
#include "conf/player.hpp"

namespace lif {

namespace conf {
	namespace bullet {
		constexpr float BASE_SPEED = lif::conf::player::DEFAULT_SPEED;

		/** Contains mappings dataId => bullet data */
		extern std::map<unsigned, const lif::BulletData> data;
	}
}

}
