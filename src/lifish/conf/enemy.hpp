#pragma once

#include <SFML/System/Time.hpp>

namespace lif {

namespace conf {
	namespace enemy {
		constexpr float BASE_SPEED = 75.f;
		const sf::Time DEATH_TIME = sf::seconds(2);

		namespace wisp {
			/** Wisp's speed in walls is (1 - IN_WALL_SPEED_REDUCTION) * speed */
			constexpr float IN_WALL_SPEED_REDUCTION = 0.5;
		}

		namespace spikes {
			constexpr int DAMAGE = 2;
		}
	}
}

}
