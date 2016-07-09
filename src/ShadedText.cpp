#include "ShadedText.hpp"
#include "Game.hpp"
#include "GameCache.hpp"
#include "Drawable.hpp"

using Game::ShadedText;

ShadedText::ShadedText(const std::string& fontname, const std::string& str, 
		sf::Vector2f pos, sf::Color fg, sf::Color bg)
	: Game::Entity(pos)
	, shadowSpacing(3.5, 3)
{
	setFont(fontname);
	setString(str);
	fgtext.setPosition(pos);
	bgtext.setPosition(pos + shadowSpacing);
	fgtext.setColor(fg);
	bgtext.setColor(bg);

	addComponent(new Game::Drawable(this, this));
}

void ShadedText::setFont(const std::string& fontname) {
	auto font = Game::cache.loadFont(fontname);
	fgtext.setFont(*font);
	bgtext.setFont(*font);
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

void ShadedText::setCharacterSize(unsigned int size) {
	bgtext.setCharacterSize(size);
	fgtext.setCharacterSize(size);
}

void ShadedText::setPosition(const sf::Vector2f& _pos) {
	Game::Entity::setPosition(_pos);
	fgtext.setPosition(_pos);
	bgtext.setPosition(_pos + shadowSpacing);
}

void ShadedText::setShadowSpacing(float spx, float spy) {
	shadowSpacing.x = spx;
	shadowSpacing.y = spy;
	bgtext.setPosition(fgtext.getPosition() + shadowSpacing);
}

void ShadedText::setColor(const sf::Color& fg, const sf::Color& bg) {
	fgtext.setColor(fg);
	bgtext.setColor(bg);
}

void ShadedText::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	window.draw(bgtext, states);
	window.draw(fgtext, states);
}
