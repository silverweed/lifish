#include "Points.hpp"
#include "Direction.hpp"
#include "game.hpp"
#include "Temporary.hpp"
#include "utils.hpp"
#include "game_values.hpp"
#include "AxisMoving.hpp"
#include "Drawable.hpp"
#include "ZIndexed.hpp"

using Game::Points;

Points::Points(const sf::Vector2f& pos, const std::string& str, sf::Color color, unsigned short charSize) 
	: Game::Entity(pos)
	, initialPos(pos)
	, text(Game::getAsset("fonts", Game::Fonts::POINTS), str, pos, color, sf::Color::Black)
{
	text.setCharacterSize(charSize);
	text.setShadowSpacing(1.5, 1);

	addComponent(new Game::AxisMoving(*this, SPEED, Game::Direction::UP));
	addComponent(new Game::Drawable(*this, text));
	addComponent(new Game::ZIndexed(*this, Game::Conf::ZIndex::POINTS));
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

void Points::setOrigin(const sf::Vector2f& pos) {
	Game::Entity::setOrigin(pos);
	text.setOrigin(pos);
}

sf::FloatRect Points::getGlobalBounds() const {
	return text.getGlobalBounds();
}
