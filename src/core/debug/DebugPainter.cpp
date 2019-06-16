#include "DebugPainter.hpp"

using lif::debug::DebugPainter;

void DebugPainter::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for (auto& item : items)
		target.draw(*item, states);
}

void DebugPainter::addRectangleAt(const sf::Vector2f& pos, const sf::Vector2f& size, const sf::Color& color,
		int outlineThickness, const sf::Color& outlineColor,
		const lif::Angle& rot, const sf::Vector2f& origin )
{
	auto r = std::make_unique<sf::RectangleShape>(size - 2.f * sf::Vector2f(outlineThickness, outlineThickness));
	r->setPosition(pos + sf::Vector2f(outlineThickness, outlineThickness));
	r->setFillColor(color);
	r->setOutlineThickness(outlineThickness);
	r->setOutlineColor(outlineColor);
	r->setOrigin(origin);
	if (rot != lif::Angle::Zero)
		r->setRotation(rot.asDegrees());
	items.emplace_back(std::move(r));
}

void DebugPainter::addCircleAt(const sf::Vector2f& pos, float radius, const sf::Color& color,
		int outlineThickness, const sf::Color& outlineColor)
{
	auto c = std::make_unique<sf::CircleShape>(radius - 2.f * outlineThickness);
	c->setPosition(pos + sf::Vector2f(outlineThickness, outlineThickness));
	c->setFillColor(color);
	c->setOutlineThickness(outlineThickness);
	c->setOutlineColor(outlineColor);
	items.emplace_back(std::move(c));
}

void DebugPainter::addTextAt(const sf::Vector2f& pos, const std::string& text,
		unsigned size, const sf::Color& color,
		int outlineThickness, const sf::Color& outlineColor)
{
	auto t = std::make_unique<sf::Text>(text, *lif::cache.loadFont(
				lif::getAsset("fonts", lif::fonts::DEBUG_INFO)), size);
	t->setPosition(pos);
	t->setFillColor(color);
	t->setOutlineThickness(outlineThickness);
	t->setOutlineColor(outlineColor);
	items.emplace_back(std::move(t));
}

void DebugPainter::clear() { items.clear(); }
