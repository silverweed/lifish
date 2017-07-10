#pragma once

#include <SFML/System/Time.hpp>

namespace lif {

namespace conf {
	namespace boss {
		const sf::Time DEATH_TIME = sf::seconds(4);

		namespace alien_boss {
			constexpr auto VALUE = 5000;
			constexpr auto LIFE = 20;
			const sf::Time SHOOT_INTERVAL = sf::seconds(2);
			const sf::Time SHOOT_SHORT_INTERVAL = sf::milliseconds(400);
			constexpr auto N_SHOTS = 3;
			constexpr float SIGHT_RADIUS = 9;
		}

		namespace haunting_spirit_boss {
			constexpr auto VALUE = 10000;
			constexpr auto HAUNTED_STATUE_LIFE = 4;
			const sf::Time CHANGE_STATUE_DELAY = sf::seconds(25);
			const sf::Time PATTERN_SHOOT_DELAY = sf::seconds(3.5);
		}

		namespace temple_boss {
			constexpr auto CAGE_LIFE = 10;
		}

		namespace big_alien_boss {
			constexpr auto VALUE = 50000;
			constexpr auto LIFE = 100;
		}

		namespace rex_boss {
			constexpr float SPEED = 3;
			constexpr auto LIFE = 40;
			constexpr auto VALUE = 20000;
		}
	}
}

}
