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

/**
 * This class manages the "special effects" of a level.
 * Special effects can be either represented by one or more entities to be added to the level
 * or by post-process effects to be blended over the rendertarget.
 */
class LevelEffects : private sf::NonCopyable {

	bool darknessOn = false;
	mutable sf::RenderTexture darknessRenderTex;


	/** Adds the "darkness" effect to level managed by `lm`, blending it over `window` */
	void _blendDarkness(const lif::LevelManager& lm, sf::RenderTarget& window) const;
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
	/** Returns the set of entities to be added to level according to `lv`'s effects */
	static std::set<lif::Entity*> getEffectEntities(const lif::Level& lv);

	explicit LevelEffects(const sf::Vector2u& windowSize);

	/** Sets the effects for the `blendEffects` functions */
	template<class T>
	void setEffects(const T& effects) {
		darknessOn = effects.find("darkness") != effects.end();
	}

	void blendEffects(const lif::LevelManager& lm, sf::RenderTarget& target) const;
};

}
