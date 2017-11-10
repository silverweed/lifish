#pragma once

namespace lif {

namespace conf {
	namespace zindex {
		// Default layer is 0.
		// ZIndex < 0 will be drawn *above* the level border, with more negative Z being drawn
		// above less negative Z.
		constexpr int PONDS           = 1;
		constexpr int EXPLOSIONS      = 2;
		constexpr int TRANSP_WALLS    = 3;
		constexpr int WALLS           = 4;
		constexpr int BOMBS           = 5;
		constexpr int PLAYERS         = 6;
		constexpr int ENEMIES         = 7;
		constexpr int BULLETS         = 8;
		constexpr int BOSSES          = 8;
		constexpr int TALL_ENTITIES   = 9;
		constexpr int FOG             = 10;
		constexpr int BOSS_EXPLOSIONS = 20;
		constexpr int FLASHES         = 20;
		constexpr int POINTS          = 42;
		constexpr int DROPPING_TEXTS  = 42;
		constexpr int TORCHES         = -1;
	}
}

}
