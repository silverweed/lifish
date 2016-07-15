#pragma once

#include <string>
#include <vector>
#include <utility>
#include "Screen.hpp"
#include "json.hpp"

namespace Game {

namespace UI {

class ScreenBuilder {
	static sf::Vector2f pos;
	static sf::FloatRect prevElemBounds;
	// pair (element, row)
	static std::vector<std::pair<sf::Drawable*, unsigned short>> toBeAligned;
	static std::vector<float> rowWidths; 
	static float totHeight;

	static void _parseStyles(Game::UI::Screen& screen, const nlohmann::json& screenJSON);
	static void _addElement(Game::UI::Screen& screen, const nlohmann::json& screenJSON);
	static void _addText(Game::UI::Screen& screen, const nlohmann::json& screenJSON);
	static void _addImage(Game::UI::Screen& screen, const nlohmann::json& screenJSON);
	static void _fixAlign(Game::UI::Screen& screen);

public:
	ScreenBuilder() = delete;
	ScreenBuilder(const ScreenBuilder&) = delete;

	/** Builds `screen` from layout file `layoutFileName`. 
	 *  Throws is `screen` was already built.
	 */
	static void build(Game::UI::Screen& screen, const std::string& layoutFileName);
};

}

}
