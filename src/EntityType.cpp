#include "EntityType.hpp"

using Game::EntityType;

namespace Game {

EntityType entityFromLetter(const char c) {
	switch (c) {
	case '0': return EntityType::EMPTY;
	case '1': return EntityType::FIXED;
	case '2': return EntityType::BREAKABLE;
	case 'X': return EntityType::PLAYER1;
	case 'Y': return EntityType::PLAYER2;
	case '+': return EntityType::TELEPORT;
	case 'A': return EntityType::ENEMY1;
	case 'B': return EntityType::ENEMY2;
	case 'C': return EntityType::ENEMY3;
	case 'D': return EntityType::ENEMY4;
	case 'E': return EntityType::ENEMY5;
	case 'F': return EntityType::ENEMY6;
	case 'G': return EntityType::ENEMY7;
	case 'H': return EntityType::ENEMY8;
	case 'I': return EntityType::ENEMY9;
	case 'K': return EntityType::ENEMY10;
	}
	return EntityType::UNKNOWN;
}

}

std::ostream& operator<<(std::ostream& stream, const Game::EntityType& et) {
	switch (et) {
	case EntityType::EMPTY:     stream << '0'; break;
	case EntityType::FIXED:     stream << '1'; break;
	case EntityType::BREAKABLE: stream << '2'; break;
	case EntityType::PLAYER1:   stream << 'X'; break;
	case EntityType::PLAYER2:   stream << 'Y'; break;
	case EntityType::TELEPORT:  stream << '+'; break;
	case EntityType::ENEMY1:    stream << 'A'; break;
	case EntityType::ENEMY2:    stream << 'B'; break;
	case EntityType::ENEMY3:    stream << 'C'; break;
	case EntityType::ENEMY4:    stream << 'D'; break;
	case EntityType::ENEMY5:    stream << 'E'; break;
	case EntityType::ENEMY6:    stream << 'F'; break;
	case EntityType::ENEMY7:    stream << 'G'; break;
	case EntityType::ENEMY8:    stream << 'H'; break;
	case EntityType::ENEMY9:    stream << 'I'; break;
	case EntityType::ENEMY10:   stream << 'K'; break;
	case EntityType::UNKNOWN:   stream << "?"; break;
	}
	return stream;
}
