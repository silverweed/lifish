#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <memory>
#include <vector>
#include "ShadedText.hpp"
#include "Component.hpp"
#include "ScreenStyle.hpp"
#include "Interactable.hpp"
#include "WithOrigin.hpp"

namespace Game {

namespace UI {

class ScreenBuilder;

class Screen : public sf::Drawable, public Game::WithOrigin {
	friend class Game::UI::ScreenBuilder;

protected:
	/** The window this screen is rendered in */
	const sf::RenderWindow& window;
	/** The expected size of the rendering target, used when positioning elements in this screen */
	sf::Vector2u size;
	/** The name of this screen */
	std::string name;
	/** The parent screen, if any */
	std::string parent;
	/** The background tile */
	sf::Sprite bgSprite;

	/** Whether this Screen was already built or not */
	bool built = false;

	/** The styles */
	std::unordered_map<std::string, Game::UI::ScreenStyle> styles;
	/** The interactable texts/images */
	std::unordered_map<std::string, std::unique_ptr<Game::UI::Interactable>> interactables;
	/** The non-interactable texts/images */
	std::vector<std::unique_ptr<sf::Drawable>> nonInteractables;

	/** The currently selected element, if any */
	std::pair<std::string, Game::UI::Interactable*> selected;

public:
	explicit Screen(const std::string& layoutFileName, const sf::RenderWindow& window);

	bool wasBuilt() const { return built; }
	const std::string& getParent() const { return parent; }
	const std::string& getName() const { return name; }

	/** @return The name of the selected element, if any (else "") */
	std::string getSelected() const;

	void update();
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void setOrigin(const sf::Vector2f& pos) override;
};

}

}
