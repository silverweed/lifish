#pragma once

#include <SFML/System/Time.hpp>

namespace lif {

namespace conf {
	namespace enemy {
		constexpr float BASE_SPEED = 60.f;
		constexpr auto BASE_LIFE = 1;
		const sf::Time DEATH_TIME = sf::seconds(2);
		const sf::Time DAMAGE_SHIELD_TIME = sf::seconds(1);
		const sf::Time YELL_INTERVAL_MIN = sf::seconds(7);
		const sf::Time YELL_INTERVAL_MAX = sf::seconds(40);

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
