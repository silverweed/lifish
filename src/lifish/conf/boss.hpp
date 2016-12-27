#pragma once

#include <SFML/System/Time.hpp>

namespace Game {

namespace Conf {
	namespace Boss {
		const sf::Time DEATH_TIME = sf::seconds(4);
		const sf::Time HURT_TIME = sf::seconds(0.40);

		namespace AlienBoss {
			constexpr unsigned int VALUE = 5000;
			constexpr unsigned short LIFE = 20;
			const sf::Time SHOOT_INTERVAL = sf::seconds(2);
			const sf::Time SHOOT_SHORT_INTERVAL = sf::milliseconds(400);
			constexpr unsigned short N_SHOTS = 3;
			constexpr float SIGHT_RADIUS = 9;
		}

		namespace HauntingSpiritBoss {
			constexpr unsigned int VALUE = 10000;
		}

		namespace BigAlienBoss {
			constexpr unsigned int VALUE = 50000;
			constexpr unsigned short LIFE = 100;
		}
	}
}

}
