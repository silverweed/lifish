#include "ShadedText.hpp"
#include <iostream>

using Game::ShadedText;

ShadedText::ShadedText(const std::string& fontname, const std::string& _str, 
		sf::Vector2f _pos, sf::Color fg, sf::Color bg)
	: str(_str), pos(_pos), fgcol(fg), bgcol(bg)
{
	if (!font.loadFromFile(fontname)) {
		std::cerr << "[ShadedFont.cpp] Error loading font " << fontname << std::endl;
	} else {
		fgtext.setFont(font);
		bgtext.setFont(font);
		fgtext.setString(str);
		bgtext.setString(str);
		fgtext.setPosition(pos);
		bgtext.setPosition(pos + sf::Vector2f(3.5, 3));
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
	
void ShadedText::draw(sf::RenderTarget& window) {
	window.draw(bgtext);
	window.draw(fgtext);
}
