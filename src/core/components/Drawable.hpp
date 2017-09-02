#pragma once

#include <cmath>
#include <SFML/Graphics.hpp>
#include "Angle.hpp"
#include "Component.hpp"

namespace lif {

class Drawable : public lif::Component, public sf::Drawable {
	const sf::Drawable& delegate;
	sf::Vector2f rotOrigin;
	lif::Angle rotation;
	sf::Vector2f scaleOrigin;
	sf::Vector2f scale;

public:
	explicit Drawable(lif::Entity& owner, const sf::Drawable& delegate)
		: lif::Component(owner)
		, delegate(delegate)
		, scale(1, 1)
	{
		_declComponent<Drawable>();
	}

	void draw(sf::RenderTarget& window, sf::RenderStates states) const override {
		if (!active) return;

		if (rotation != lif::Angle::Zero) {
			// Apply rotation
			const float angle = rotation.asRadians(),
				    cosine = std::cos(angle),
				    sine = std::sin(angle);
			const sf::Transform trans(1.f, 0.f, owner.getPosition().x + rotOrigin.x,
						  0.f, 1.f, owner.getPosition().y + rotOrigin.y,
						  0.f, 0.f, 1.f),
					    rot(cosine, sine,   0.f,
						-sine,  cosine, 0.f,
						0.f,    0.f,    1.f),
					    itrans(1.f, 0.f, -owner.getPosition().x - rotOrigin.x,
						   0.f, 1.f, -owner.getPosition().y - rotOrigin.y,
						   0.f, 0.f, 1.f);
			states.transform = trans * rot * itrans;
		}
		if (scale.x != 1 || scale.y != 1) {
			// Apply scale
			const sf::Transform strans(1.f, 0.f, owner.getPosition().x + scaleOrigin.x,
						   0.f, 1.f, owner.getPosition().y + scaleOrigin.y,
						   0.f, 0.f, 1.f),
					    scl(scale.x, 0.f,     0.f,
						0.f,     scale.y, 0.f,
						0.f,     0.f,     1.f),
					    istrans(1.f, 0.f, -owner.getPosition().x - scaleOrigin.x,
						    0.f, 1.f, -owner.getPosition().y - scaleOrigin.y,
						    0.f, 0.f, 1.f);
			states.transform *= strans * scl * istrans;
		}

		window.draw(delegate, states);
	}

	void setRotOrigin(float x, float y) { rotOrigin.x = x; rotOrigin.y = y; }
	void setRotation(lif::Angle a) { rotation = a; }
	lif::Angle getRotation() const { return rotation; }
	
	void setScaleOrigin(float x, float y) { scaleOrigin.x = x; scaleOrigin.y = y; }
	void setScale(float x, float y) { scale.x = x; scale.y = y; }
	sf::Vector2f getScale() const { return scale; }
};

}
