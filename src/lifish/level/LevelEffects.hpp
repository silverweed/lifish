#pragma once

#include <set>
#include <tuple>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/System/NonCopyable.hpp>

namespace lif {

class Entity;
class Level;
class LevelManager;

class LevelEffects : private sf::NonCopyable {
	/** Height of the render texture */
	const float dy;

	bool darknessOn = false;
	mutable sf::RenderTexture darknessRenderTex;


	/** Converts an y coordinate from world to screen. */
	constexpr float _yw2s(float y) const { return dy - y; }
	/** Adds the "darkness" effect to level managed by `lm`, blending it over `window` starting at `origin` */
	void _blendDarkness(const lif::LevelManager& lm, sf::RenderTarget& window, const sf::Vector2f& origin) const;
	/** Given the Entity `e`, returns two rectangleshapes representing its vision.
	 *  `e` must have an `AxisSighted` component, else the method will throw.
	 */
	auto _getVisionRectangles(const lif::Entity& e) const
		-> std::pair<sf::RectangleShape, sf::RectangleShape>;
	/** Returns a vector of FloatRect that form a "tiled circle" around the given point `center`.
	 *  The size of such vector is `radius + 1`.
	 */
	std::vector<sf::FloatRect> _getRadialRectangles(const sf::Vector2f& center, unsigned radius) const;
public:
	explicit LevelEffects(const sf::Vector2u& windowSize);

	/** Returns the set of entities to be added to level according to `lv`'s effects */
	static std::set<lif::Entity*> getEffectEntities(const lif::Level& lv);

	/** Sets the effects for the `blendEffects` functions */
	template<class T>
	void setEffects(const T& effects) {
		darknessOn = effects.find("darkness") != effects.end();
	}

	void blendEffects(const lif::LevelManager& lm, sf::RenderTarget& target,
			const sf::Vector2f& origin = sf::Vector2f(0, 0)) const;
};

}
