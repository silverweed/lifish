#pragma once

#include "Entity.hpp"
#include <SFML/Graphics.hpp>
#include <string>

namespace lif {

/**
 * A sf::Text container which draws a string with a shadow behind
 */
class ShadedText final : public lif::Entity, public sf::Drawable {
	sf::Text fgtext, bgtext;
	sf::Vector2f shadowSpacing;

public:
	ShadedText() {}
	ShadedText(
		const std::string& fontname,
		const std::string& str,
		sf::Vector2f pos = sf::Vector2f(0, 0),
		sf::Color fgcol = sf::Color::White,
		sf::Color bgcol = sf::Color::Black);
	ShadedText(
		const sf::Font& font,
		const std::string& str,
		sf::Vector2f pos = sf::Vector2f(0, 0),
		sf::Color fgcol = sf::Color::White,
		sf::Color bgcol = sf::Color::Black);

	sf::FloatRect getGlobalBounds() const;
	std::string getString() const;

	sf::Color getFGColor() const { return fgtext.getFillColor(); }
	sf::Color getBGColor() const { return bgtext.getFillColor(); }
	void setStyle(sf::Text::Style style);
	void setString(const std::string& str);
	void setCharacterSize(unsigned int size);
	void setShadowSpacing(float spx, float spy);
	void setFGColor(const sf::Color& fg) { fgtext.setFillColor(fg); }
	void setBGColor(const sf::Color& bg) { bgtext.setFillColor(bg); }
	void setColor(const sf::Color& fg, const sf::Color& bg);
	void setFont(const sf::Font& font);
	void setScale(float x, float y);

	void setPosition(const sf::Vector2f& pos) override;
	void setOrigin(const sf::Vector2f& origin) override;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}
