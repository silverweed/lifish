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
			const sf::Time STOMP_WINDUP_TIME = sf::seconds(1);
			const sf::Time STOMP_DAMAGE_TIME = sf::seconds(1);
			const sf::Time STOMP_RECOVER_TIME = sf::seconds(1.5);
			constexpr auto STOMP_TILE_RADIUS = 2;
			constexpr auto STOMP_DAMAGE = 4;
			const sf::Time FLAME_WINDUP_TIME = sf::seconds(1);
			const sf::Time FLAME_DAMAGE_TIME = sf::seconds(3);
			const sf::Time FLAME_RECOVER_TIME = sf::seconds(1);
			constexpr auto FLAME_TILE_HEIGHT = 4;
			constexpr auto FLAME_TILE_WIDTH = 5;
			constexpr auto FLAME_DAMAGE = 3;
			const sf::Time MISSILES_WINDUP_TIME = sf::seconds(1);
			const sf::Time MISSILES_DAMAGE_TIME = sf::seconds(1.5);
			const sf::Time MISSILES_RECOVER_TIME = sf::seconds(2);
			// Missile damage and speed are in BulletFactory
			constexpr auto N_MISSILES = 12;
		}

		namespace god_eye_boss {
			constexpr auto LIFE = 1000;
		}
	}
}

}
