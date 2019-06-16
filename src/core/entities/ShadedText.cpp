#include "ShadedText.hpp"
#include "Drawable.hpp"
#include "GameCache.hpp"
#include "game.hpp"

using lif::ShadedText;

ShadedText::ShadedText(const sf::Font& font, const std::string& str,
		sf::Vector2f pos, sf::Color fg, sf::Color bg)
	: lif::Entity(pos)
	, shadowSpacing(3.5f, 3.f)
{
	setFont(font);
	setString(str);
	fgtext.setPosition(pos);
	bgtext.setPosition(pos + shadowSpacing);
	fgtext.setFillColor(fg);
	bgtext.setFillColor(bg);

	addComponent<lif::Drawable>(*this, *this);
}

ShadedText::ShadedText(const std::string& fontname, const std::string& str,
		sf::Vector2f pos, sf::Color fg, sf::Color bg)
	: ShadedText(*lif::cache.loadFont(fontname), str, pos, fg, bg)
{}

void ShadedText::setFont(const sf::Font& font) {
	fgtext.setFont(font);
	bgtext.setFont(font);
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
	lif::Entity::setOrigin(origin);
	bgtext.setOrigin(origin);
	fgtext.setOrigin(origin);
}

void ShadedText::setCharacterSize(unsigned int size) {
	bgtext.setCharacterSize(size);
	fgtext.setCharacterSize(size);
}

void ShadedText::setPosition(const sf::Vector2f& _pos) {
	lif::Entity::setPosition(_pos);
	fgtext.setPosition(_pos);
	bgtext.setPosition(_pos + shadowSpacing);
}

void ShadedText::setShadowSpacing(float spx, float spy) {
	shadowSpacing.x = spx;
	shadowSpacing.y = spy;
	bgtext.setPosition(fgtext.getPosition() + shadowSpacing);
}

void ShadedText::setColor(const sf::Color& fg, const sf::Color& bg) {
	fgtext.setFillColor(fg);
	bgtext.setFillColor(bg);
}

void ShadedText::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	window.draw(bgtext, states);
	window.draw(fgtext, states);
}

sf::FloatRect ShadedText::getGlobalBounds() const {
	sf::FloatRect bounds(fgtext.getGlobalBounds());
	bounds.width += shadowSpacing.x;
	bounds.height += shadowSpacing.y;
	return bounds;
}

std::string ShadedText::getString() const {
	return fgtext.getString();
}

void ShadedText::setScale(float x, float y) {
	fgtext.setScale(x, y);
	bgtext.setScale(x, y);
}
