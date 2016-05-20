#pragma once

#include <string>
#include <set>
#include <unordered_map>
#include <vector>
#include <SFML/System.hpp>
#include "ShadedText.hpp"
#include "Sprite.hpp"
#include "Clickable.hpp"
#include "Component.hpp"

namespace Game {

/**text
 * A non-in-game screen, with a background and possibly some
 * foreground sprites or text
 */
class Screen : public Game::Entity, public sf::Drawable, protected sf::NonCopyable {
	Game::Sprite *bgSprite = nullptr;

	//[>* The background texture <]
	//sf::Texture *bgTexture;

	//[>* The background tile <]
	//sf::Sprite bgSprite;

	/** The objects in this screen which respond to a mouse event along
	 *  with their identifier string (see triggerMouseClick).
	 */
	std::unordered_map<std::string, Game::Clickable> clickables;

	/** The static elements which don't interact */
	std::vector<std::unique_ptr<sf::Drawable>> elements;

protected:
	void _addClickable(const std::string& key, Game::ShadedText *elem);
	void _addClickable(const std::string& key, Game::Sprite *elem);
	void _addStatic(sf::Drawable *elem) {
		elements.push_back(std::unique_ptr<sf::Drawable>(elem));
	}
	
public:
	Screen();
	virtual ~Screen() {}

	/** Note that the Screens aren't drawn via their Drawable component, but directly. */
	virtual void draw(sf::RenderTarget& window, sf::RenderStates states) const override;

	/** Checks if mouse is over a text and, if so, changes
	 *  its color.
	 */
	virtual void triggerMouseOver(const sf::Vector2f& mousePos);

	/** If the mouse was clicked on a clickable element, return the
	 *  name of that element; else, return the empty string.
	 */
	virtual std::string triggerMouseClick(const sf::Vector2f& mousePos);

	virtual std::set<Game::Screen*> getParents() const {
		std::set<Game::Screen*> empty;
		return empty; 
	}

	void setOrigin(const sf::Vector2f& origin) override {
		Game::Entity::setOrigin(origin);
		for (auto& pair : clickables)
			pair.second.setOrigin(origin);
		for (auto& e : elements) {
			auto o = dynamic_cast<Game::WithOrigin*>(e.get());
			if (o != nullptr)
				o->setOrigin(origin);
			else {
				auto t = dynamic_cast<sf::Transformable*>(e.get());
				if (t != nullptr)
					t->setOrigin(origin);
			}
		}
	}
};

}
