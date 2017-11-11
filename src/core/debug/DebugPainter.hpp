#pragma once

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "GameCache.hpp"
#include "Angle.hpp"
#include "game.hpp"

namespace lif {

/** This class provides utility functions to draw simple geometry atop the game render target.
 *  Useful for visual debugging. The `clear()` method should be called after every frame.
 */
class DebugPainter : public sf::Drawable {

	std::vector<std::unique_ptr<sf::Drawable>> items;

public:
	explicit DebugPainter() {}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
		for (auto& item : items)
			target.draw(*item, states);
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
		items.emplace_back(std::move(r));
	}

	void addCircleAt(const sf::Vector2f& pos, float radius, const sf::Color& color) {
		auto c = std::make_unique<sf::CircleShape>(radius);
		c->setPosition(pos);
		c->setFillColor(color);
		items.emplace_back(std::move(c));
	}
	
	void addTextAt(const sf::Vector2f& pos, const std::string& text,
			unsigned size = 15, const sf::Color& color = sf::Color::Magenta) {
		auto t = std::make_unique<sf::Text>(text, *lif::cache.loadFont(
					lif::getAsset("fonts", lif::fonts::DEBUG_INFO)), size);
		t->setPosition(pos);
		t->setFillColor(color);
		items.emplace_back(std::move(t));
	}

	void clear() { items.clear(); }
};

}
