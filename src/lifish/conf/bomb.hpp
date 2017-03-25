#pragma once

namespace lif {

namespace conf {
	namespace bomb {
		const sf::Time DEFAULT_FUSE = sf::seconds(5);
		const sf::Time QUICK_FUSE = DEFAULT_FUSE / 2.f;
		constexpr unsigned short DEFAULT_RADIUS = 2;
		constexpr unsigned short MAX_RADIUS = 4;
	}
}

}
