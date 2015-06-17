#pragma once

#include <ostream>

namespace Game {

enum class EntityType {
	UNKNOWN,
	EMPTY,
	FIXED,
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
	ENEMY10
};

EntityType entityFromLetter(const char c);

}

std::ostream& operator<<(std::ostream& stream, const Game::EntityType& et);
