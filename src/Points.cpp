#include "Points.hpp"

using Game::Points;

Points::Points(const sf::Vector2f& pos, const std::string& str, sf::Color color, unsigned short charSize) :
	Game::Temporary(pos),
	Game::ShadedText(
		Game::getAsset("fonts", Game::Fonts::POINTS),
		str, pos, color, sf::Color::Black),
	initialPos(pos)
{
	setCharacterSize(charSize);
	shadowSpacing = sf::Vector2f(1.5, 1);
}

void Points::draw(sf::RenderTarget& window) {
	sf::Time frameTime = frameClock.restart();
	ShadedText::pos.y -= speed * frameTime.asSeconds();
	ShadedText::setPosition(ShadedText::pos);
	Game::ShadedText::draw(window);
}
