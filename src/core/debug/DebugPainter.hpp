#pragma once

#include "Angle.hpp"
#include "GameCache.hpp"
#include "game.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

namespace lif {

namespace debug {

/** This class provides utility functions to draw simple geometry atop the game render target.
 *  Useful for visual debugging. The `clear()` method should be called after every frame.
 */
class DebugPainter : public sf::Drawable {

	std::vector<std::unique_ptr<sf::Drawable>> items;

public:
	explicit DebugPainter() {}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void addRectangleAt(const sf::Vector2f& pos, const sf::Vector2f& size, const sf::Color& color,
			int outlineThickness = 0,
			const sf::Color& outlineColor = sf::Color::Black,
			const lif::Angle& rot = lif::Angle::Zero,
			const sf::Vector2f& origin = sf::Vector2f(0, 0));

	void addCircleAt(const sf::Vector2f& pos, float radius, const sf::Color& color,
			int outlineThickness = 0,
			const sf::Color& outlineColor = sf::Color::Black);

	void addTextAt(const sf::Vector2f& pos, const std::string& text,
			unsigned size = 15, const sf::Color& color = sf::Color::Magenta,
			int outlineThickness = 1,
			const sf::Color& outlineColor = sf::Color::Black);

	void clear();
};

}
}
