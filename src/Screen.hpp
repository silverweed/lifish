#pragma once

#include <string>
#include <set>
#include <unordered_map>
#include <SFML/System.hpp>
#include "ShadedText.hpp"
#include "Sprite.hpp"

namespace Game {

/**
 * A non-in-game screen, with a background and possibly some
 * foreground sprites or text
 */
class Screen : public Game::Entity, public sf::Drawable, protected sf::NonCopyable {
protected:
	/** The background tile */
	Game::Sprite *bgSprite;

	/** The texts in this screen which respond to a mouse event along
	 *  with their identifier string (see triggerMouseClick).
	 */
	std::unordered_map<std::string, Game::ShadedText*> texts;

	/** The clickable images */
	std::unordered_map<std::string, sf::Sprite*> images;

	/** The static elements which don't interact */
	std::vector<sf::Drawable*> elements;

public:
	Screen();
	virtual ~Screen();

	virtual void draw(sf::RenderTarget& window, sf::RenderStates states) const override;

	/** Checks if mouse is over a text and, if so, changes
	 *  its color.
	 */
	virtual void triggerMouseOver(const sf::Vector2f& mousePos);

	/** If the mouse was clicked on a clickable element, return the
	 *  name of that element; else, return the empty string.
	 */
	virtual std::string triggerMouseClick(const sf::Vector2f& mousePos);

	virtual std::set<Game::Screen*> getParents() {
		std::set<Game::Screen*> empty;
		return empty; 
	}

	void setOrigin(const sf::Vector2f& origin) override {
		Game::Entity::setOrigin(origin);
		for (auto& e : elements) {
			auto o = dynamic_cast<Game::WithOrigin*>(e);
			if (o != nullptr)
				o->setOrigin(origin);
			else {
				auto t = dynamic_cast<sf::Transformable*>(e);
				if (t != nullptr)
					t->setOrigin(origin);
			}
		}
		for (auto& pair : texts)
			pair.second->setOrigin(origin);
		for (auto& pair : images)
			pair.second->setOrigin(origin);
	}
};

}
