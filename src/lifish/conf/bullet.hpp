#pragma once

#include <map>
#include "Bullet.hpp"

namespace Game {

struct BulletData;

namespace Conf {
	namespace Bullet {
		constexpr float BASE_SPEED = 200.;

		extern std::map<unsigned short, const Game::BulletData> data;
	}
}

}
