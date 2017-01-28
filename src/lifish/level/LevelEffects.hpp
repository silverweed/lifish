#pragma once

#include <set>
#include <tuple>
#include <SFML/Graphics.hpp>
#include <SFML/System/NonCopyable.hpp>

namespace lif {

class Entity;
class Level;
class LevelManager;

class LevelEffects : private sf::NonCopyable {
	bool darknessOn = false;
	mutable sf::RenderTexture darknessRenderTex;


	/** Adds the "darkness" effect to level managed by `lm`, blending it over `window` starting at `origin` */
	void _blendDarkness(const lif::LevelManager& lm, sf::RenderTarget& window, const sf::Vector2f& origin) const;
	/** Given the Entity `e`, returns two rectangleshapes representing its vision.
	 *  `e` must have an `AxisSighted` component, else the method will throw.
	 */
	auto _getVisionRectangles(const lif::Entity& e) const
		-> std::pair<sf::RectangleShape, sf::RectangleShape>;
	/** Returns a 3-ple of rectangleshapes that form a "tiled circle" around the given point `center`.
	 *  NOTE: the third rectangle will be undefined if `radius` is 0; it is the caller's responsibility
	 *  to ensure it's not used improperly in that case.
	 */
	auto _getRadialRectangles(const sf::Vector2f& center, unsigned radius) const
		-> std::tuple<sf::RectangleShape, sf::RectangleShape, sf::RectangleShape>;
public:
	explicit LevelEffects();

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
