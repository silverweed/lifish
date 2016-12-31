#pragma once

#include <random>
#include <SFML/System/Time.hpp>

namespace Game {

namespace Conf {
	namespace Bonus {
		constexpr unsigned int VALUE = 100; // FIXME
		constexpr unsigned short N_BONUS_TYPES = 9;
		constexpr unsigned short N_PERMANENT_BONUS_TYPES = 6;
		const sf::Time EXPIRE_TIME = sf::seconds(10);
		const sf::Time SHIELD_DURATION = sf::seconds(20);
		const sf::Time SPEEDY_DURATION = sf::seconds(20);

		extern std::discrete_distribution<unsigned short> distribution;
	}
}

}
