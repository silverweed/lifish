#pragma once

namespace lif {

namespace conf {
	namespace bomb {
		const sf::Time DEFAULT_FUSE = sf::seconds(5);
		const sf::Time QUICK_FUSE = DEFAULT_FUSE / 2.f;
		constexpr auto DEFAULT_RADIUS = 2;
		constexpr auto MAX_RADIUS = 4;
		constexpr auto EXPL_DAMAGE = 1;
	}
}

}
