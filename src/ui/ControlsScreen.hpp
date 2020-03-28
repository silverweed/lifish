#pragma once

#include "DynamicScreen.hpp"

namespace lif {

class ShadedText;

namespace ui {

class ControlsScreen : public lif::ui::DynamicScreen {
	/** Whose player we're changing controls */
	int selectedPlayer = 1;
	/** The control we're changing right now, if any */
	lif::ShadedText *changingCtrlText = nullptr;

	void _highlightSelectedPlayer();
	void _resyncCommandStrings();

	// Callbacks
	/** Selects `id`-th player (starting from 1) */
	lif::ui::Action _selectPlayer(int id);
	/** Sets the `textKey` text to be in 'changing' state */
	lif::ui::Action _changeControl(const std::string& textKey);
	/** Toggles the lif::useJoystick variable for the selected player */
	lif::ui::Action _toggleJoystick();

	void _setupCallbacks();
	void _setupTransitions();

	void build() override;
public:
	static constexpr const char *SCREEN_NAME = "controls";

	explicit ControlsScreen(const sf::RenderWindow& window, const sf::Vector2u& size);

	void update() override;
	bool handleEvent(sf::Window& window, sf::Event event) override;

};

}
}
