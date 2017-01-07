#include "LevelNumText.hpp"
#include "game.hpp"
#include "utils.hpp"

using lif::LevelNumText;

LevelNumText::LevelNumText(lif::Entity& owner, const unsigned short levelnum)
	: lif::Component(owner)
	, text(lif::getAsset("fonts", lif::Fonts::LEVELNUM),
		lif::to_string(levelnum),
		sf::Vector2f(lif::TILE_SIZE * (lif::LEVEL_WIDTH+1), 0))
{
	text.setStyle(sf::Text::Bold);
	text.setCharacterSize(20);
}

void LevelNumText::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(text, states);
}

void LevelNumText::setOrigin(const sf::Vector2f& pos) {
	lif::Entity::setOrigin(pos);
	text.setOrigin(pos);
}
