#pragma once

#include <string>
#include <vector>
#include <utility>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "json.hpp"

namespace lif {

namespace ui {

class Screen;

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

	void _parseStyles(lif::ui::Screen& screen, const nlohmann::json& screenJSON);
	void _addElement(lif::ui::Screen& screen, const nlohmann::json& screenJSON);
	void _addText(lif::ui::Screen& screen, const nlohmann::json& screenJSON);
	void _addImage(lif::ui::Screen& screen, const nlohmann::json& screenJSON);
	void _fixAlign(lif::ui::Screen& screen);

public:
	explicit ScreenBuilder() {}

	/** Builds `screen` from layout file `layoutFileName`. 
	 *  Throws is `screen` was already built.
	 */
	void build(lif::ui::Screen& screen, const std::string& layoutFileName);
};

}

}
