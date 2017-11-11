#pragma once

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Angle.hpp"

namespace lif {

/** This class provides utility functions to draw simple geometry atop the game render target.
 *  Useful for visual debugging. The `clear()` method should be called after every frame.
 */
class DebugPainter : public sf::Drawable {

	std::vector<std::unique_ptr<sf::Shape>> shapes;

public:
	explicit DebugPainter() {}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		for (auto& shape : shapes)
			target.draw(*shape, states);
	}

	void addRectangleAt(const sf::Vector2f& pos, const sf::Vector2f& size, const sf::Color& color,
			const lif::Angle& rot = lif::Angle::Zero,
			const sf::Vector2f& origin = sf::Vector2f(0, 0))
	{
		auto r = std::make_unique<sf::RectangleShape>(size);
		r->setPosition(pos);
		r->setFillColor(color);
		r->setOrigin(origin);
		if (rot != lif::Angle::Zero)
			r->setRotation(rot.asDegrees());
		shapes.emplace_back(std::move(r));
	}

	void addCircleAt(const sf::Vector2f& pos, float radius, const sf::Color& color) {
		auto c = std::make_unique<sf::CircleShape>(radius);
		c->setPosition(pos);
		c->setFillColor(color);
		shapes.emplace_back(std::move(c));
	}

	void clear() { shapes.clear(); }
};

}
