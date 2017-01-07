#pragma once

#include <ostream>

namespace lif {

enum class EntityType {
	UNKNOWN,
	EMPTY,
	FIXED,
	COIN,
	BREAKABLE,
	PLAYER1,
	PLAYER2,
	TELEPORT,
	ENEMY1,
	ENEMY2,
	ENEMY3,
	ENEMY4,
	ENEMY5,
	ENEMY6,
	ENEMY7,
	ENEMY8,
	ENEMY9,
	ENEMY10,
	BOSS,
	TRANSPARENT_WALL,
	ACID_POND,
	HAUNTED_STATUE
};

EntityType entityFromLetter(const char c);

std::ostream& operator<<(std::ostream& stream, const lif::EntityType& et);

}
