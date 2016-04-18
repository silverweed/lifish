#include "LevelNumText.hpp"
#include "utils.hpp"

using Game::LevelNumText;

LevelNumText::LevelNumText(Game::Entity *const owner, const unsigned short levelnum)
	: Game::Component(owner)
{	
	// Load the levelnum text
	text = new Game::ShadedText(
		Game::getAsset("fonts", Game::Fonts::LEVELNUM),
		Game::to_string(levelnum),
		sf::Vector2f(TILE_SIZE * (LEVEL_WIDTH+1), 0));
	text->setStyle(sf::Text::Bold);
	text->setCharacterSize(20);
}

void LevelNumText::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	text.draw(target, states);
}
