#pragma once

#include <array>
#include <vector>
#include <SFML/Graphics/Color.hpp>
#include "Component.hpp"

namespace lif {

/**
 * An Entity with a LightSource emits circular light in a Dark level.
 * It can optionally flicker.
 */
class LightSource : public lif::Component {
	std::array<float, 256> randomPool;
	std::vector<float> smoothing;

	float origRadius;
	float radius;
	sf::Color color;
	unsigned short flickerLen = 0;
	
	void _fillRandomPool(float flickerAmount);
	float _flickerStep();
public:
	explicit LightSource(lif::Entity& owner, float radius,
			sf::Color color = sf::Color::White,
			float flickerIntensity = 0,
			unsigned short flickerLen = 0);

	float getRadius() const { return radius; }
	void setRadius(float r) { radius = r; }

	sf::Color getColor() const { return color; }
	void setColor(const sf::Color& c) { color = c; }

	void update() override;
};

}
