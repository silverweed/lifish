#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <memory>
#include <vector>
#include "ShadedText.hpp"
#include "Component.hpp"
#include "ScreenStyle.hpp"

namespace Game {

namespace UI {

class ScreenBuilder;

class Screen : public sf::Drawable {
	friend class Game::UI::ScreenBuilder;

protected:
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
	/** The interactable texts */
	std::unordered_map<std::string, std::unique_ptr<Game::ShadedText>> texts;
	/** The interactable images */
	std::unordered_map<std::string, std::unique_ptr<sf::Sprite>> images;
	/** The non-interactable texts/images */
	std::vector<std::unique_ptr<sf::Drawable>> nonInteractables;

public:
	explicit Screen() {}
	explicit Screen(const std::string& layoutFileName);

	bool wasBuilt() const { return built; }

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

}

}
