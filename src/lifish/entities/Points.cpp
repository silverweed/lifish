#include "Points.hpp"
#include "Direction.hpp"
#include "game.hpp"
#include "Temporary.hpp"
#include "utils.hpp"
#include "conf/zindex.hpp"
#include "AxisMoving.hpp"
#include "Drawable.hpp"
#include "ZIndexed.hpp"

using lif::Points;

// Note: putting this in the class declaration causes some compilers
// (notably: clang) to give 'undefined reference'
constexpr static float SPEED = 30.f;

Points::Points(const sf::Vector2f& pos, const std::string& str, sf::Color color, unsigned short charSize) 
	: lif::Entity(pos)
	, initialPos(pos)
	, text(lif::getAsset("fonts", lif::fonts::POINTS), str, pos, color, sf::Color::Black)
{
	text.setCharacterSize(charSize);
	text.setShadowSpacing(1.5, 1);

	addComponent(std::make_shared<lif::AxisMoving>(*this, SPEED, lif::Direction::UP));
	addComponent(std::make_shared<lif::Drawable>(*this, text));
	addComponent(std::make_shared<lif::ZIndexed>(*this, lif::conf::zindex::POINTS));
	addComponent(std::make_shared<lif::Temporary>(*this, [this] () {
		return (initialPos - position).y >= 20;
	}));
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
