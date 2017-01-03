#pragma once

namespace Game {

namespace Conf {
	namespace ZIndex {
		// Default layer is 0
		constexpr int EXPLOSIONS      = 1;
		constexpr int TRANSP_WALLS    = 2;
		constexpr int WALLS           = 4;
		constexpr int PLAYERS         = 5;
		constexpr int ENEMIES         = 6;
		constexpr int TALL_ENTITIES   = 7;
		constexpr int BULLETS         = 8;
		constexpr int BOSSES          = 8;
		constexpr int FOG             = 9;
		constexpr int BOSS_EXPLOSIONS = 10;
		constexpr int FLASHES         = 10;
		constexpr int POINTS          = 42;
		constexpr int DROPPING_TEXTS  = 42;
	}
}

}
