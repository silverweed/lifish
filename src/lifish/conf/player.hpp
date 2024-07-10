#pragma once

#include <SFML/System/Time.hpp>

namespace lif {

namespace conf {
	namespace player {
		constexpr unsigned short INITIAL_LIVES = 3;
		constexpr unsigned short MAX_LIFE = 16;
		constexpr unsigned short DEFAULT_MAX_BOMBS = 5;
		constexpr unsigned short MAX_MAX_BOMBS = 8;
		constexpr unsigned short INITIAL_CONTINUES = 3;
		constexpr float DEFAULT_SPEED = 120.f;
		/** The number of letters needed to get an extra life */
		constexpr unsigned short N_EXTRA_LETTERS = 5;
		const sf::Time DEATH_TIME = sf::seconds(5);
		const sf::Time DEATH_STOP_ANIM_TIME = sf::seconds(2);
		const sf::Time HURT_ANIM_DURATION = sf::milliseconds(140);
		const sf::Time RESURRECT_SHIELD_TIME = sf::seconds(5);
		const sf::Time DAMAGE_SHIELD_TIME = sf::seconds(1);
	}
}

}
