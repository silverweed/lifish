#pragma once

namespace Game {

namespace UI {

enum class Action {
	START_GAME,
	LOAD_GAME,
	SAVE_GAME,
	SWITCH_SCREEN,
	SWITCH_SCREEN_OVERRIDE_PARENT,
	SWITCH_TO_PARENT,
	DEACTIVATE_UI,
	EXIT,
	DO_NOTHING
};

}

}
