#include "ShadedText.hpp"
#include "Game.hpp"
#include "GameCache.hpp"

using Game::ShadedText;

ShadedText::ShadedText(const std::string& fontname, const std::string& _str, 
		sf::Vector2f _pos, sf::Color fg, sf::Color bg)
	: Game::Entity(_pos)
	, str(_str)
	, shadowSpacing(3.5, 3)
	, fgcol(fg)
	, bgcol(bg)
{
	auto font = Game::cache.loadFont(fontname);
	fgtext.setFont(*font);
	bgtext.setFont(*font);
	fgtext.setString(str);
	bgtext.setString(str);
	fgtext.setPosition(_pos);
	bgtext.setPosition(_pos + shadowSpacing);
	fgtext.setColor(fgcol);
	bgtext.setColor(bgcol);
}

void ShadedText::setStyle(sf::Text::Style style) {
	bgtext.setStyle(style);
	fgtext.setStyle(style);
}

void ShadedText::setString(const std::string& str) {
	bgtext.setString(str);
	fgtext.setString(str);
}

void ShadedText::setOrigin(const sf::Vector2f& origin) {
	Game::Entity::setOrigin(origin);
	bgtext.setOrigin(origin);
	fgtext.setOrigin(origin);
}

void ShadedText::moveSprites(const sf::Vector2f& offset) {
	bgtext.move(offset);
	fgtext.move(offset);
}

void ShadedText::setCharacterSize(unsigned int size) {
	bgtext.setCharacterSize(size);
	fgtext.setCharacterSize(size);
}

void ShadedText::setPosition(const sf::Vector2f& _pos) {
	fgtext.setPosition(_pos);
	bgtext.setPosition(_pos + shadowSpacing);
	position = _pos;
}

void ShadedText::setShadowSpacing(float spx, float spy) {
	shadowSpacing.x = spx;
	shadowSpacing.y = spy;
	bgtext.setPosition(fgtext.getPosition() + shadowSpacing);
}

void ShadedText::setColor(const sf::Color& fg, const sf::Color& bg) {
	fgtext.setColor(fgcol = fg);
	bgtext.setColor(bgcol = bg);
}

void ShadedText::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	window.draw(bgtext, states);
	window.draw(fgtext, states);
}
