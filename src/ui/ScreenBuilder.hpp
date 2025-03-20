#pragma once

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

namespace tinyjson {
class json;
}

namespace lif {

class ShadedText;

namespace ui {

class Screen;

class ScreenBuilder final : private sf::NonCopyable {
	constexpr static float H_PADDING = 20;
	constexpr static float V_PADDING = 10;

	sf::Vector2f pos;
	sf::FloatRect prevElemBounds;
	// pair (element, row)
	std::vector<std::pair<sf::Drawable*, unsigned>> toBeAligned;
	const std::unordered_map<std::string, std::string>& dynamicTexts;
	std::vector<std::string> rowAligns;
	std::vector<float> rowWidths;
	std::string vAlign;
	float totHeight = 0;

	void _parseStyles(lif::ui::Screen& screen, const tinyjson::json& screenJSON);
	void _addElement(lif::ui::Screen& screen, const tinyjson::json& screenJSON);
	void _addText(lif::ui::Screen& screen, const tinyjson::json& screenJSON);
	void _addImage(lif::ui::Screen& screen, const tinyjson::json& screenJSON);
	void _fixAlign(lif::ui::Screen& screen);
	void _calcTransitions(lif::ui::Screen& screen);
	sf::String _maybeInsertDynamicText(const std::string& s) const;

public:
	explicit ScreenBuilder(const std::unordered_map<std::string, std::string>& dynTexts) : dynamicTexts(dynTexts) {}

	/** Builds `screen` from layout file `layoutFileName`.
	 *  Throws is `screen` was already built.
	 */
	void build(Screen& screen, const std::string& layoutFileName);
	void rebuild(lif::ui::Screen& screen);
};

}

}
