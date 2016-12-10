#pragma once

#include <array>
#include <ostream>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>
#include "game.hpp"

namespace Game {

template<class T, size_t ROWS, size_t COLS>
using Matrix = std::array<std::array<T, COLS>, ROWS>;

/// Polyfill ///
template<typename T> 
constexpr T abs(T num) {
#ifdef IS_APPLE
	// Apple Clang is a "good" compiler...
	if (num < 0) return -num;
	return num;
#else
	return std::abs(num);
#endif
}

template<typename T>
inline std::string to_string(T val) {
#ifdef __MINGW32__
	std::stringstream ss;
	ss << val;
	return ss.str();
#else
	return std::to_string(val);
#endif
}
//////////////

/** Returns the aligned tile corresponding to position `pos` by
 *  flooring its components and dividing by TILE_SIZE.
 */
inline sf::Vector2i tile(const sf::Vector2f& pos) {
	return sf::Vector2i(pos.x / Game::TILE_SIZE, pos.y / Game::TILE_SIZE);
}

/** Returns the pixel position `pos` aligned to the nearest tile. */
inline sf::Vector2f aligned(const sf::Vector2f& pos) {
	return sf::Vector2f(
		(int(pos.x)/TILE_SIZE) * TILE_SIZE,
		(int(pos.y)/TILE_SIZE) * TILE_SIZE);
}

template<typename T>
constexpr std::ostream& operator<<(std::ostream& stream, const sf::Vector2<T>& vec) {
	return stream << "(" << vec.x << ", " << vec.y << ")";
}

template<typename T>
constexpr std::ostream& operator<<(std::ostream& stream, const sf::Rect<T>& rect) {
	return stream << "(" << rect.left << ", " << rect.top 
		<< " x " << rect.width << ", " << rect.height << ")";
}

template<typename T, typename R>
constexpr double sqrDistance(const sf::Vector2<T>& a, const sf::Vector2<R>& b) {
	return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

template<typename T, typename R>
constexpr T manhattanDistance(const sf::Vector2<T>& tileA, const sf::Vector2<R>& tileB) {
	return Game::abs(tileA.x - tileB.x) + Game::abs(tileA.y - tileB.y);
}

/** @return A float indicating the X coordinate where `innerBounds` is to be positioned in 
 *  order to be horizontally centered relatively to `outerBounds`.
 */
template<typename T, typename R>
constexpr float centerX(const sf::Rect<T>& innerBounds, const sf::Rect<R>& outerBounds) {
	return outerBounds.left + (outerBounds.width - innerBounds.width) / 2.;
}

/** @return A float indicating the Y coordinate where `innerBounds` is to be positioned in 
 *  order to be vertically centered relatively to `outerBounds`.
 */
template<typename T, typename R>
constexpr float centerY(const sf::Rect<T>& innerBounds, const sf::Rect<R>& outerBounds) {
	return outerBounds.top + (outerBounds.height - innerBounds.height) / 2.;
}

template<typename T>
inline sf::Vector2f normalized(const sf::Vector2<T>& v) {
	const float norm = std::sqrt(v.x * v.x + v.y * v.y);
	return sf::Vector2f(v.x / norm, v.y / norm);
}

/** Given the bounding box of something, returns the coordinates
 *  which center that thing relatively to the given rectangle.
 *  bounds = object to center
 *  rect = container to center the object within (default: main game window)
 */
inline sf::Vector2f center(const sf::FloatRect& innerBounds, const sf::FloatRect& outerBounds) {
	return sf::Vector2f(Game::centerX(innerBounds, outerBounds), Game::centerY(innerBounds, outerBounds));
}

inline bool startsWith(const std::string& haystack, const std::string& needle) {
	return haystack.substr(0, needle.size()) == needle;
}

/** Plays a brief sound */
void testMusic();

/** If Game::options.showFPS == true, then display FPS on screen */
void maybeShowFPS(sf::RenderWindow& window);

namespace KeyUtils {
	short keyToNumber(sf::Keyboard::Key key);

	std::string keyToString(sf::Keyboard::Key key);
}

namespace JoystickUtils {
	short getPauseButton(unsigned int joystickId);
}

} // end namespace Game
