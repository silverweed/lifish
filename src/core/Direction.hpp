#pragma once

#include <ostream>

namespace lif {

// Don't change values of these, as they may be used to index arrays
enum Direction {
	UP,
	LEFT,
	DOWN,
	RIGHT,
	NONE
};

inline std::string directionToString(const Direction& dir) {
	switch (dir) {
	case Direction::UP:    return "up";
	case Direction::LEFT:  return "left";
	case Direction::DOWN:  return "down"; 
	case Direction::RIGHT: return "right";
	default:               return "none";
	}
}

inline std::ostream& operator<<(std::ostream& stream, const Direction& dir) {
	switch (dir) {
	case Direction::UP:    
		stream << "UP";
		break;
	case Direction::LEFT:
		stream << "LEFT"; 
		break;
	case Direction::DOWN:  
		stream << "DOWN";
		break;
	case Direction::RIGHT: 
		stream << "RIGHT"; 
		break;
	default: 
		stream << "NONE"; 
		break;
	}
	return stream;
}

inline Direction oppositeDirection(const Direction dir) {
	switch (dir) {
	case Direction::UP:    return Direction::DOWN;
	case Direction::DOWN:  return Direction::UP;
	case Direction::LEFT:  return Direction::RIGHT;
	case Direction::RIGHT: return Direction::LEFT;
	default:               return Direction::NONE;
	}
}

/** Turns right `times` times (negative is OK, means 'turn left') starting from `dir`
 *  and returns the corresponding direction.
 */
Direction turnRight(const Direction dir, short times);

}
