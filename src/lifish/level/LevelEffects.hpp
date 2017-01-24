#pragma once

#include <set>
#include <SFML/Graphics.hpp>
#include <SFML/System/NonCopyable.hpp>

namespace lif {

class Entity;
class Level;
class LevelManager;

class LevelEffects : private sf::NonCopyable {
	bool darknessOn;
	mutable sf::RenderTexture darknessRenderTex;

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
