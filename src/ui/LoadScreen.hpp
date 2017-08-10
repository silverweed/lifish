#pragma once

#include "Screen.hpp"
#include "SaveDataBrowser.hpp"

namespace lif {

namespace ui {

class LoadScreen : public lif::ui::Screen, public lif::SaveDataBrowser {
public:
	static constexpr const char *SCREEN_NAME = "load";

	explicit LoadScreen(const sf::RenderWindow& window, const sf::Vector2u& size);

	/** Reads the save directory and presents all the save files found in the screen */
	void onLoad() override;
};

}

}
