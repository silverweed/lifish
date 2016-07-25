#include "Points.hpp"
#include "Direction.hpp"
#include "game.hpp"
#include "Temporary.hpp"
#include "AxisMoving.hpp"
#include "Drawable.hpp"

using Game::Points;

Points::Points(const sf::Vector2f& pos, const std::string& str, sf::Color color, unsigned short charSize) 
	: Game::Entity(pos)
	, initialPos(pos)
	, text(Game::getAsset("fonts", Game::Fonts::POINTS), str, pos, color, sf::Color::Black)
{
	text.setCharacterSize(charSize);
	text.setShadowSpacing(1.5, 1);

	addComponent(new Game::AxisMoving(*this, SPEED, Game::Direction::UP));
	addComponent(new Game::Drawable(*this, &text));
	addComponent(new Game::Temporary(*this, [this] () {
		return (initialPos - position).y >= 20;
	}));
}

Points::Points(const sf::Vector2f& pos, int pts, sf::Color color, unsigned short charSize) 
	: Game::Points(pos, Game::to_string(pts), color, charSize)
{}

void Points::update() {
	Game::Entity::update();
	text.setPosition(position);
}
