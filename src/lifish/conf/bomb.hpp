#pragma once

namespace Game {

namespace Conf {
	namespace Bomb {
		const sf::Time DEFAULT_FUSE = sf::seconds(2/*5*/);
		const sf::Time QUICK_FUSE = DEFAULT_FUSE / 2.f;
		constexpr unsigned short DEFAULT_RADIUS = 2;
		constexpr unsigned short MAX_RADIUS = 4;
	}
}

}
