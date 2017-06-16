#pragma once

#include <array>
#include <ostream>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include "Angle.hpp"
#include "core.hpp"
#include "json.hpp"

// Enable automatic sf::Time / json conversion:
// https://github.com/nlohmann/json#basic-usage
namespace sf {

void to_json(nlohmann::json& json, const sf::Time& time);
void from_json(const nlohmann::json& json, sf::Time& time);

}

namespace lif {

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
	return sf::Vector2i(pos.x / lif::TILE_SIZE, pos.y / lif::TILE_SIZE);
}

/** Like `tile`, but instead of flooring, round to the nearest tile. */
inline sf::Vector2i tile2(const sf::Vector2f& pos) {
	return sf::Vector2i(static_cast<int>(pos.x / lif::TILE_SIZE + 0.5),
	                    static_cast<int>(pos.y / lif::TILE_SIZE + 0.5));
}

/** Returns the pixel position `pos` aligned to the floored tile. */
inline sf::Vector2f aligned(const sf::Vector2f& pos) {
	return sf::Vector2f(
		static_cast<int>(pos.x / TILE_SIZE) * TILE_SIZE,
		static_cast<int>(pos.y / TILE_SIZE) * TILE_SIZE);
}

/** Like `aligned`, but instead of flooring, round to the nearest tile. */
inline sf::Vector2f aligned2(const sf::Vector2f& pos) {
	return sf::Vector2f(
		static_cast<int>(pos.x / TILE_SIZE + 0.5) * TILE_SIZE,
		static_cast<int>(pos.y / TILE_SIZE + 0.5) * TILE_SIZE);
}

template<typename T>
constexpr std::ostream& operator<<(std::ostream& stream, sf::Vector2<T> vec) {
	return stream << "(" << vec.x << ", " << vec.y << ")";
}

template<typename T>
constexpr std::ostream& operator<<(std::ostream& stream, sf::Rect<T> rect) {
	return stream << "(" << rect.left << ", " << rect.top 
		<< " x " << rect.width << ", " << rect.height << ")";
}

inline std::ostream& operator<<(std::ostream& stream, const sf::RectangleShape& rect) {
	const auto p = rect.getPosition();
	const auto s = rect.getSize();
	return stream << "(" << p.x << ", " << p.y << " x " << s.x << ", " << s.y << ")";
}

template<typename T, typename R>
constexpr float sqrDistance(sf::Vector2<T> a, sf::Vector2<R> b) {
	return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

template<typename T>
constexpr float length(sf::Vector2<T> v) {
	return std::sqrt(v.x * v.x + v.y * v.y);
}

template<typename T, typename R>
constexpr T manhattanDistance(sf::Vector2<T> tileA, sf::Vector2<R> tileB) {
	return lif::abs(tileA.x - tileB.x) + lif::abs(tileA.y - tileB.y);
}

/** @return A float indicating the X coordinate where `innerBounds` is to be positioned in 
 *  order to be horizontally centered relatively to `outerBounds`.
 */
template<typename T, typename R>
constexpr float centerX(sf::Rect<T> innerBounds, sf::Rect<R> outerBounds) {
	return outerBounds.left + (outerBounds.width - innerBounds.width) / 2.;
}

/** @return A float indicating the Y coordinate where `innerBounds` is to be positioned in 
 *  order to be vertically centered relatively to `outerBounds`.
 */
template<typename T, typename R>
constexpr float centerY(sf::Rect<T> innerBounds, sf::Rect<R> outerBounds) {
	return outerBounds.top + (outerBounds.height - innerBounds.height) / 2.;
}

template<typename T, typename R>
constexpr Angle angleBetween(sf::Vector2<T> a, sf::Vector2<R> b) {
	// calculate angle with ppos: a = pi - arctan(dy / dx)
	const double dx = a.x - b.x,
		     dy = a.y - b.y;
	return lif::radians(lif::PI / 2. + std::atan2(dy, dx));
}
}

template<typename T>
inline sf::Vector2f normalized(const sf::Vector2<T>& v) {
	const float norm = std::sqrt(v.x * v.x + v.y * v.y);
	if (norm == 0)
		return v;
	return sf::Vector2f(v.x / norm, v.y / norm);
}

/** Given the bounding box of something, returns the coordinates
 *  which center that thing relatively to the given rectangle.
 *  bounds = object to center
 *  rect = container to center the object within (default: main game window)
 */
inline sf::Vector2f center(const sf::FloatRect& innerBounds, const sf::FloatRect& outerBounds) {
	return sf::Vector2f(lif::centerX(innerBounds, outerBounds), lif::centerY(innerBounds, outerBounds));
}

inline bool startsWith(const std::string& haystack, const std::string& needle) {
	return haystack.substr(0, needle.size()) == needle;
}

/** Plays a brief sound */
void testMusic();

} // end namespace lif
