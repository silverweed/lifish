#pragma once

#include <array>
#include <ostream>
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

void to_json(nlohmann::json& json, const sf::VideoMode& vm);
void from_json(const nlohmann::json& json, sf::VideoMode& vm);

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

inline std::ostream& operator<<(std::ostream& stream, const sf::Color& col) {
	return stream << "(" << int{col.r} << ", " << int{col.g} << ", " << int{col.b} << ", " << int{col.a} << ")";
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

template<typename T, typename R>
#ifdef _MSC_VER
inline Angle angleBetween(sf::Vector2<T> a, sf::Vector2<R> b) {
#else
constexpr Angle angleBetween(sf::Vector2<T> a, sf::Vector2<R> b) {
#endif
	// calculate angle with ppos: a = pi - arctan(dy / dx)
	const double dx = static_cast<double>(b.x) - static_cast<double>(a.x),
	             dy = static_cast<double>(b.y) - static_cast<double>(a.y);
	return lif::radians(lif::PI / 2. + std::atan2(dy, dx));
}

template<typename T>
inline sf::Vector2f normalized(const sf::Vector2<T>& v) {
	const float norm = std::sqrt(v.x * v.x + v.y * v.y);
	if (norm == 0)
		return v;
	return sf::Vector2f(v.x / norm, v.y / norm);
}

/** @return A float indicating the X coordinate where `innerBounds` is to be positioned in
 *  order to be horizontally centered relatively to `outerBounds`.
 */
template<typename T, typename R>
constexpr float centerX(sf::Rect<T> innerBounds, sf::Rect<R> outerBounds) {
	return outerBounds.left + (outerBounds.width - innerBounds.width) / 2.f;
}

/** @return A float indicating the Y coordinate where `innerBounds` is to be positioned in
 *  order to be vertically centered relatively to `outerBounds`.
 */
template<typename T, typename R>
constexpr float centerY(sf::Rect<T> innerBounds, sf::Rect<R> outerBounds) {
	return outerBounds.top + (outerBounds.height - innerBounds.height) / 2.f;
}

/** Given a starting point `start`, returns the point which is at `extent` distance from it,
 *  at angle `angle`. Angle is 0 in UP direction, CW.
 */
inline sf::Vector2f towards(const sf::Vector2f& start, const lif::Angle& angle, float extent) {
	return start + extent * sf::Vector2f(std::sin(angle.asRadians()), -std::cos(angle.asRadians()));
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

template<typename T>
constexpr T lerp(T a, T b, float alpha) {
	return a * (1 - alpha) + b * alpha;
}

template<typename T>
constexpr int sign(T x) {
	return (x > 0) - (x < 0);
}

template<typename T>
constexpr T clamp(T val, T min, T max) {
	return std::max(std::min(val, max), min);
}

/** Plays a brief sound (BLOCKING) */
void testMusic();

} // end namespace lif
