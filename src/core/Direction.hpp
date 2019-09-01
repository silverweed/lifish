#pragma once

#include <SFML/System/Vector2.hpp>
#include <ostream>
#include <string>

namespace lif {

// Don't change values of these, as they may be used to index arrays
enum Direction {
	UP,
	LEFT,
	DOWN,
	RIGHT,
	NONE
};

inline std::string directionToString(Direction dir) {
	switch (dir) {
	case Direction::UP:    return "up";
	case Direction::LEFT:  return "left";
	case Direction::DOWN:  return "down";
	case Direction::RIGHT: return "right";
	default:               return "none";
	}
}

inline std::string directionToStringOr(Direction dir, const std::string& dflt) {
	if (dir == Direction::NONE)
		return dflt;
	return directionToString(dir);
}

inline std::ostream& operator<<(std::ostream& stream, Direction dir) {
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

inline Direction oppositeDirection(Direction dir) {
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
Direction turnRight(Direction dir, short times);

/** Returns the direction of the vector pointing from tile `from` to `to`.
 *  Returns NONE if the given tiles don't have at least 1 coordinate in common or if
 *  they are the same tile.
 */
Direction getDirection(const sf::Vector2i& from, const sf::Vector2i& to);

sf::Vector2f directionToVersor(Direction dir);

/** Inverse function of `getDirection`: given `start` position and a direction, returns
 *  the position `scale` tiles towards that direction from it.
 */
template<typename T>
inline sf::Vector2<T> towards(const sf::Vector2<T>& start, Direction dir, float scale = 1) {
	return start + scale * sf::Vector2<T>(directionToVersor(dir));
}

} // end namespace lif
