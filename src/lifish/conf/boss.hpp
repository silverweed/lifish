#pragma once

#include "Angle.hpp"
#include <SFML/System/Time.hpp>

namespace lif {

namespace conf {
	namespace boss {
		const sf::Time DEATH_TIME = sf::seconds(4);
		const auto MAX_TILES_CONSIDERED_FOR_BOMB_DAMAGE = 5;

		namespace alien_boss {
			constexpr auto VALUE = 5000;
			constexpr auto LIFE = 20;
			const sf::Time CAN_SHOOT_INTERVAL = sf::seconds(1.2);
			const sf::Time CANNOT_SHOOT_INTERVAL = sf::seconds(2.4);
			const sf::Time SHOOT_SHORT_INTERVAL = sf::milliseconds(400);
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
			constexpr auto TIMES_TO_HURT_BEFORE_DEATH = 4;
			constexpr auto N_ENEMIES_SPAWNED = 4;
			constexpr auto SPAWNED_ENEMY_ID = 7;
			const sf::Time SHAKE_DURATION = sf::seconds(3);
		}

		namespace mainframe_boss {
			constexpr auto LIFE = 40;
			constexpr auto VALUE = 30000;
			const sf::Time IDLE_TIME = sf::seconds(3);
			constexpr auto SURGE_DAMAGE = 6;
			constexpr auto SURGE_ROT_PER_SECOND = lif::degrees(8);
			constexpr auto SURGE_SPANNED_ANGLE = lif::degrees(60);
			const sf::Time SURGE_WINDUP_TIME = sf::seconds(3);
			const sf::Time SURGE_RECOVER_TIME = sf::seconds(SURGE_SPANNED_ANGLE / SURGE_ROT_PER_SECOND);
			const sf::Time LIGHTNING_WINDUP_TIME = sf::seconds(2);
			constexpr auto LIGHTNING_N_SHOTS = 20;
			const sf::Time LIGHTNING_SHOOT_DELAY = sf::milliseconds(133);
			const sf::Time LIGHTNING_RECOVERY_TIME = sf::seconds(3);
			const sf::Time SHIELD_WINDUP_TIME = sf::seconds(2.5);
			const sf::Time SHIELD_DAMAGE_TIME = sf::seconds(5);
			const sf::Time SHIELD_RECOVERY_TIME = sf::seconds(1);
			constexpr auto SHIELD_DAMAGE = 4;
			constexpr auto LASERS_MIN_N_SHOTS = 3;
			constexpr auto LASERS_MAX_N_SHOTS = 9;
			const sf::Time LASERS_SHOOT_MIN_DELAY = sf::seconds(0.3);
			const sf::Time LASERS_SHOOT_MAX_DELAY = sf::seconds(1.5);
			constexpr auto LASERS_DAMAGE = 3;
			const sf::Time LASERS_WARN_DURATION = sf::seconds(1);
			const sf::Time LASERS_RECOVERY_TIME = sf::seconds(4);
			constexpr auto SPAWNED_ENEMY_ID = 4;
			constexpr auto N_SPAWNED_ENEMIES = 4;
			const sf::Time ZAPS_SPAWN_DELAY = sf::seconds(1.5);
			const sf::Time ZAPS_RECOVERY_TIME = sf::seconds(4);
		}

		namespace big_alien_boss {
			constexpr auto SPEED = 1.0;
			constexpr auto LIFE = 40;
			constexpr auto VALUE = 100000;
			constexpr auto EGG_SPEED = 1.5;
			const sf::Time ATK_INTERVAL = sf::seconds(5);
		}
	}
}

}
