#include "ShadedText.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include <iostream>

using Game::ShadedText;

ShadedText::ShadedText(const std::string& fontname, const std::string& _str, 
		sf::Vector2f _pos, sf::Color fg, sf::Color bg)
	: str(_str), pos(_pos), shadowSpacing(3.5, 3), fgcol(fg), bgcol(bg)
{
	auto font = Game::cache.loadFont(fontname);
	if (font == nullptr) {
		std::cerr << "[ShadedFont.cpp] Error loading font " << fontname << std::endl;
	} else {
		fgtext.setFont(*font);
		bgtext.setFont(*font);
		fgtext.setString(str);
		bgtext.setString(str);
		fgtext.setPosition(pos);
		bgtext.setPosition(pos + shadowSpacing);
		fgtext.setColor(fgcol);
		bgtext.setColor(bgcol);
	}
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
	bgtext.setOrigin(origin);
	fgtext.setOrigin(origin);
}

void ShadedText::setCharacterSize(unsigned int size) {
	bgtext.setCharacterSize(size);
	fgtext.setCharacterSize(size);
}

void ShadedText::setPosition(const sf::Vector2f& _pos) {
	fgtext.setPosition(_pos);
	bgtext.setPosition(_pos + shadowSpacing);
	pos = _pos;
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
	
void ShadedText::draw(sf::RenderTarget& window) {
	window.draw(bgtext);
	window.draw(fgtext);
}
