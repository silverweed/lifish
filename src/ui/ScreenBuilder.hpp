#pragma once

#include <string>
#include "Screen.hpp"
#include "json.hpp"

namespace Game {

namespace UI {

class ScreenBuilder {
	static void _parseStyles(Game::UI::Screen& screen, const nlohmann::json& screenJSON);
	static void _addElement(Game::UI::Screen& screen, const nlohmann::json& screenJSON);
	static void _addText(Game::UI::Screen& screen, const nlohmann::json& screenJSON);
	static void _addImage(Game::UI::Screen& screen, const nlohmann::json& screenJSON);

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
