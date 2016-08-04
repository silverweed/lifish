#pragma once

#include <string>
#include <vector>
#include <utility>
#include <SFML/System/NonCopyable.hpp>
#include "Screen.hpp"
#include "json.hpp"

namespace Game {

namespace UI {

class ScreenBuilder final : private sf::NonCopyable {
	constexpr static float H_PADDING = 20;
	constexpr static float V_PADDING = 10;

	sf::Vector2f pos;
	sf::FloatRect prevElemBounds;
	// pair (element, row)
	std::vector<std::pair<sf::Drawable*, unsigned short>> toBeAligned;
	std::vector<std::string> rowAligns;
	std::vector<float> rowWidths; 
	std::string vAlign;
	float totHeight = 0;

	void _parseStyles(Game::UI::Screen& screen, const nlohmann::json& screenJSON);
	void _addElement(Game::UI::Screen& screen, const nlohmann::json& screenJSON);
	void _addText(Game::UI::Screen& screen, const nlohmann::json& screenJSON);
	void _addImage(Game::UI::Screen& screen, const nlohmann::json& screenJSON);
	void _fixAlign(Game::UI::Screen& screen);

public:
	explicit ScreenBuilder() {}

	/** Builds `screen` from layout file `layoutFileName`. 
	 *  Throws is `screen` was already built.
	 */
	void build(Game::UI::Screen& screen, const std::string& layoutFileName);
};

}

}
