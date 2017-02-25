#include "LoadScreen.hpp"
#include "ShadedText.hpp"
#include "Drawable.hpp"
#include "Interactable.hpp"
#include "game.hpp"
#include "utils.hpp"
#include <memory>
#include <SFML/Graphics.hpp>
#include <iostream>

using lif::ui::LoadScreen;

LoadScreen::LoadScreen(const sf::RenderWindow& window, const sf::Vector2u& sz)
	: lif::ui::Screen(window, sz)
{
	name = "load";
	parent = "home";
	_loadBGSprite(lif::getAsset("graphics", "screenbg1.png"));

	/* Layout:
	 *
	 * Load
	 * [save1]
	 * [save2]
	 * --
	 * 
	 * [save1]
	 * [new save]
	 */
	const auto font = lif::getAsset("fonts", lif::fonts::SCREEN);
	const auto win_bounds = sf::FloatRect(0, 0, sz.x, sz.y);
	const auto size = 24;
	const float ipadx = 25,
	            ipady = 15;

	auto text = new lif::ShadedText(font, "LOAD GAME", sf::Vector2f(ipadx, ipady));
	text->setShadowSpacing(2, 2);
	text->setCharacterSize(size);
	nonInteractables.push_back(std::unique_ptr<sf::Drawable>(text));


	// Back
	text = new lif::ShadedText(font, "Back", sf::Vector2f(ipadx, ipady));
	text->setCharacterSize(size);
	auto bounds = text->getGlobalBounds();
	text->setPosition(sf::Vector2f(lif::center(bounds, win_bounds).x, win_bounds.height - 3 * bounds.height));
	interactables["back"] = std::unique_ptr<lif::ui::Interactable>(new lif::ui::Interactable(text));
}

void LoadScreen::onLoad() {
	std::cout << "Entering onLoad...\n";
	// Wipe out old elements
	{ 
		auto begin = nonInteractables.begin(); 
		if (begin != nonInteractables.end()) { 
			std::cout << "first element is " << static_cast<lif::ShadedText*>(begin->get())->getString() << std::endl;
			++begin; 
			nonInteractables.erase(begin, nonInteractables.end()); 
		} 
	}
	for (auto it = interactables.begin(); it != interactables.end(); ) {
		if (it->first != "back")
			it = interactables.erase(it);
		else
			++it;
	}
	callbacks.clear();
	std::cout << "nonInt: " << nonInteractables.size() << ", int: " << interactables.size() << ", cb: " << callbacks.size() << std::endl;

	const auto size = 20;
	const auto font = lif::getAsset("fonts", lif::fonts::CUTSCENES);

	sf::Vector2f pos(25, 75);
	
	std::cout << "browsing save data...\n";
	auto saves = browseSaveData(lif::pwd);
	std::cout << "save files: " << saves.size() << std::endl;
	if (saves.size() == 0) {
		auto text = new lif::ShadedText(font, "No save data", pos);
		text->setCharacterSize(size);
		text->setShadowSpacing(2, 2);
		text->setStyle(sf::Text::Italic);
		nonInteractables.push_back(std::unique_ptr<sf::Drawable>(text));
		return;
	}

	unsigned col = 0,
	         row = 0;
	std::cout << "entering for...\n";
	for (const auto& save : saves) {
		// Setup delete file
		auto text = new lif::ShadedText(font, "[x]", pos);
		std::cout << "created delete text at " << text->getPosition() << std::endl;
		text->setCharacterSize(size);
		const auto dcbname = _newUniqueId();
		std::cout << "delete unique id: " << dcbname << std::endl;
		interactables[dcbname] = std::unique_ptr<lif::ui::Interactable>(new lif::ui::Interactable(text));
		callbacks[dcbname] = _createDeleteCallback(save.path);

		text = new lif::ShadedText(font, save.displayName.substr(0, 10), pos + sf::Vector2f(35, 0));
		std::cout << "created load text at " << text->getPosition() << std::endl;
		text->setCharacterSize(size);
		const auto lcbname = _newUniqueId();
		std::cout << "load unique id: " << lcbname << std::endl;
		interactables[lcbname] = std::unique_ptr<lif::ui::Interactable>(new lif::ui::Interactable(text));
		callbacks[lcbname] = _createLoadCallback(save.path);
		if (++col == 10) {
			col = 0;
			pos.x += 200;
			pos.y = 75;
			if (++row == 3)
				break;
		} else {
			pos.y += 30;
		}
	}
	std::cout << "onLoad: nonInteractables: " << nonInteractables.size() << std::endl;
	std::cout << "onLoad: interactables: " << interactables.size() << std::endl;
}
