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

public:
	explicit Drawable(lif::Entity& owner, const sf::Drawable& delegate)
		: lif::Component(owner)
		, delegate(delegate)
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

		window.draw(delegate, states);
	}

	void setRotOrigin(const sf::Vector2f& o) { rotOrigin = o; }
	void setRotation(lif::Angle a) { rotation = a; }
	lif::Angle getRotation() const { return rotation; }
};

}
