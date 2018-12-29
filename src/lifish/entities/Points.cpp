#include "Points.hpp"
#include "AxisMoving.hpp"
#include "Direction.hpp"
#include "Drawable.hpp"
#include "Temporary.hpp"
#include "ZIndexed.hpp"
#include "conf/zindex.hpp"
#include "game.hpp"
#include "utils.hpp"

using lif::Points;

constexpr static float SPEED = 30.f;

Points::Points(const sf::Vector2f& pos, const std::string& str, sf::Color color, unsigned short charSize)
	: lif::Entity(pos)
	, initialPos(pos)
	, text(lif::getAsset("fonts", lif::fonts::POINTS), str, pos, color, sf::Color::Black)
{
	text.setCharacterSize(charSize);
	text.setShadowSpacing(1.5, 1);

	addComponent<lif::AxisMoving>(*this, SPEED, lif::Direction::UP);
	addComponent<lif::Drawable>(*this, text);
	addComponent<lif::ZIndexed>(*this, lif::conf::zindex::POINTS);
	addComponent<lif::Temporary>(*this, [this] () {
		return (initialPos - position).y >= 20;
	});
}

Points::Points(const sf::Vector2f& pos, int pts, sf::Color color, unsigned short charSize)
	: lif::Points(pos, lif::to_string(pts), color, charSize)
{}

void Points::update() {
	lif::Entity::update();
	text.setPosition(position);
}

void Points::setOrigin(const sf::Vector2f& pos) {
	lif::Entity::setOrigin(pos);
	text.setOrigin(pos);
}

sf::FloatRect Points::getGlobalBounds() const {
	return text.getGlobalBounds();
}
