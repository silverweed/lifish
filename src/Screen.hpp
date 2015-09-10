#pragma once

#include <string>
#include <unordered_map>
#include <SFML/System.hpp>
#include "ShadedText.hpp"
#include "Drawable.hpp"

namespace Game {

/**
 * A non-in-game screen, with a background and possibly some
 * foreground sprites or text
 */
class Screen : public Game::Drawable, protected sf::NonCopyable {
protected:
	/** The background texture */
	sf::Texture bgTexture;

	/** The background tile */
	sf::Sprite bgSprite;

	/** The texts in this screen which respond to a mouse event along
	 *  with their identifier string (see triggerMouseClick).
	 */
	std::unordered_map<std::string, Game::ShadedText*> texts;

	/** The static elements which don't interact */
	std::vector<sf::Drawable*> elements;

public:
	Screen();
	virtual ~Screen();

	virtual void draw(sf::RenderTarget& window) override;

	/** Checks if mouse is over a text and, if so, changes
	 *  its color.
	 */
	virtual void triggerMouseOver(const sf::Vector2f& mousePos);

	/** If the mouse was clicked on a clickable element, return the
	 *  name of that element; else, return the empty string.
	 */
	virtual std::string triggerMouseClick(const sf::Vector2f& mousePos);

	virtual Game::Screen* getParent() { return nullptr; }
};

}
