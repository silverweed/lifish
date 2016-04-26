#pragma once

#include <SFML/Graphics.hpp>

namespace Game {

/**
 * A sf::Text container which draws a string with a shadow behind
 */
class ShadedText : public sf::Drawable {
protected:
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

	virtual sf::FloatRect getGlobalBounds() const { return fgtext.getGlobalBounds(); }

	void setStyle(sf::Text::Style style);

	void setString(const std::string& str);

	void setOrigin(const sf::Vector2f& origin);

	void moveSprites(const sf::Vector2f& offset);

	void setCharacterSize(unsigned int size);

	sf::Vector2f getPosition() const { return fgtext.getPosition(); }
	void setPosition(const sf::Vector2f& pos);

	void setShadowSpacing(float spx, float spy);

	void setFGColor(const sf::Color& fg) { fgtext.setColor(fgcol = fg); }
	void setBGColor(const sf::Color& bg) { bgtext.setColor(bgcol = bg); }
	void setColor(const sf::Color& fg, const sf::Color& bg);

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
