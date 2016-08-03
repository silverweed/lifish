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
#include "Action.hpp"

namespace Game {

namespace UI {

class ScreenBuilder;

class Screen : public sf::Drawable, public Game::WithOrigin {
public:
	using Callback = std::function<Game::UI::Action()>;

private:
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

	/** The internal callbacks, if any. Internal callbacks have priority over external ones. */
	std::unordered_map<std::string, Callback> callbacks;

	/** The currently selected element, if any */
	std::pair<std::string, Game::UI::Interactable*> selected;


	Screen(const sf::RenderWindow& window);
	void _loadBGSprite(const std::string& bgSpritePath);

public:
	explicit Screen(const std::string& layoutFileName, const sf::RenderWindow& window);

	bool wasBuilt() const { return built; }
	void setParent(const std::string& name) { parent = name; }
	const std::string& getParent() const { return parent; }
	const std::string& getName() const { return name; }

	/** @return The name of the selected element, if any (else "") */
	std::string getSelected() const;

	virtual void update();
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void setOrigin(const sf::Vector2f& pos) override;

	bool hasCallback(const std::string& name) const;
	Game::UI::Action fireCallback(const std::string& name);
	/** This may be used by child classes to do specific logic;
	 *  @return true if signal should be ignored by UI's event loop.
	 */
	virtual bool receiveEvent(const sf::Event&) { return false; }
};

}

}
