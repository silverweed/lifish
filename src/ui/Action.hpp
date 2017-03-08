#pragma once

namespace lif {

namespace ui {

enum class Action {
	START_GAME,
	LOAD_GAME,
	SAVE_GAME,
	SWITCH_SCREEN,
	SWITCH_SCREEN_OVERRIDE_PARENT,
	SWITCH_TO_PARENT,
	DEACTIVATE_UI,
	QUIT_TO_HOME, // terminate game, switch to main menu
	EXIT, // terminate game and exit
	DO_NOTHING
};

}

}
