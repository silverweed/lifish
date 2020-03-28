#pragma once

#include "SaveDataBrowser.hpp"
#include "DynamicScreen.hpp"

namespace lif {

namespace ui {

class LoadScreen : public lif::ui::DynamicScreen, public lif::SaveDataBrowser {
	void build() override;

public:
	static constexpr const char *SCREEN_NAME = "load";

	explicit LoadScreen(const sf::RenderWindow& window, const sf::Vector2u& size);

	/** Reads the save directory and presents all the save files found in the screen */
	void onLoad() override;
};

} // end namespace ui

} // end namespace lif
