#pragma once
/**
 * A sf::Text container which draws a string with a shadow behind
 */
#include <SFML/Graphics.hpp>
#include "Drawable.hpp"

namespace Game {

class ShadedText : public Game::Drawable {
protected:
	sf::Font font;
	sf::Text fgtext, bgtext;
	std::string str;
	sf::Vector2f pos;
	sf::Vector2f shadowSpacing;
	sf::Color fgcol, bgcol;
public:
	ShadedText() {}
	ShadedText(
		const std::string& fontname,
		const std::string& str, 
		sf::Vector2f pos = sf::Vector2f(0, 0),
		sf::Color fgcol = sf::Color::White,
		sf::Color bgcol = sf::Color::Black);

	virtual ~ShadedText() {}

	void setStyle(sf::Text::Style style);

	void setString(const std::string& str);

	void setOrigin(const sf::Vector2f& origin);

	void setCharacterSize(unsigned int size);

	void setPosition(const sf::Vector2f& pos);

	void setShadowSpacing(const sf::Vector2f& sp);

	void draw(sf::RenderTarget& window) override;
};

}
