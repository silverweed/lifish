#pragma once

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Angle.hpp"

namespace lif {

class DebugPainter : public sf::Drawable {

	std::vector<std::unique_ptr<sf::Shape>> shapes;

public:
	explicit DebugPainter() {}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		for (auto& shape : shapes)
			target.draw(*shape, states);
	}

	void addRectangleAt(const sf::Vector2f& pos, const sf::Vector2f& size,
			const sf::Color& color, const lif::Angle& rot = lif::Angle::Zero)
	{
		auto r = std::make_unique<sf::RectangleShape>(size);
		r->setPosition(pos);
		r->setFillColor(color);
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
