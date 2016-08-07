#pragma once

/** This file basically contains numeric values for the gameplay-related (balanceable) constants */
namespace Game {

namespace Conf {

	const sf::Time DAMAGE_SHIELD_TIME = sf::milliseconds(1000);
	const sf::Time RESURRECT_SHIELD_TIME = sf::milliseconds(5000);

	namespace Player {
		constexpr unsigned short INITIAL_LIVES = 3;
		constexpr unsigned short MAX_LIFE = 16;
		constexpr unsigned short DEFAULT_MAX_BOMBS = 5;
		constexpr unsigned short MAX_MAX_BOMBS = 8;
		constexpr unsigned short INITIAL_CONTINUES = 3;
		constexpr float DEFAULT_SPEED = 150.f;
		/** The number of letters needed to get an extra life */
		constexpr unsigned short N_EXTRA_LETTERS = 5;
	}
	
	namespace Enemy {
		const sf::Time DEATH_TIME = sf::milliseconds(2000);
	}

	namespace Bomb {
		const sf::Time DEFAULT_FUSE = sf::milliseconds(500);
		constexpr unsigned short DEFAULT_RADIUS = 2;
		constexpr unsigned short MAX_RADIUS = 4;
	}

	namespace ZIndex {
		constexpr int EXPLOSIONS = 1;
		constexpr int WALLS      = 2;   
		constexpr int PLAYERS    = 3;
		constexpr int ENEMIES    = 4;
		constexpr int BOSSES     = 5;
		constexpr int POINTS     = 6;
	}
}

}
