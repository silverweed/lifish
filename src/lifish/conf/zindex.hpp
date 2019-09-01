#pragma once

#include <array>

namespace lif {

namespace conf {
	namespace zindex {
		// Default layer is 0.
		// ZIndex < 0 will be drawn *above* the level border, with more negative Z being drawn
		// above less negative Z.
		constexpr int PONDS                 = 1;
		constexpr int TELEPORTS             = 2;
		constexpr int EXPLOSIONS            = 3;
		constexpr int TRANSP_WALLS          = 4;
		constexpr int WALLS                 = 5;
		constexpr int COINS                 = 6;
		constexpr int BOMBS                 = 7;
		constexpr int PLAYERS               = 8;
		constexpr int ENEMIES               = 9;
		constexpr int BULLETS               = 10;
		constexpr int BOSSES                = 11;
		constexpr int TALL_ENTITIES         = 12;
		constexpr int HAUNTING_SPIRIT_BOSS  = 13;
		constexpr int BOSS_BULLETS          = 14;
		constexpr int FOG                   = 15;
		constexpr int BOSS_EXPLOSIONS       = 20;
		constexpr int FLASHES               = 21;
		constexpr int DROPPING_TEXTS        = 42;
		constexpr int TORCHES               = -1;
		constexpr int UI                    = -2;
		constexpr int POINTS                = -3;

		constexpr std::array<int, 21> ALL {{
			PONDS, EXPLOSIONS, TRANSP_WALLS, TELEPORTS, WALLS, COINS, BOMBS, PLAYERS, ENEMIES,
			BULLETS, BOSSES, TALL_ENTITIES, HAUNTING_SPIRIT_BOSS, BOSS_BULLETS, FOG, BOSS_EXPLOSIONS,
			FLASHES, DROPPING_TEXTS, TORCHES, UI, POINTS
		}};

#ifndef _WIN32
		constexpr const char* layerName(int layer) {
#else
		inline const char* layerName(int layer) {
#endif
			switch (layer) {
			case PONDS: return "Ponds";
			case EXPLOSIONS: return "Explosions";
			case TRANSP_WALLS: return "Transp_walls";
			case TELEPORTS: return "Teleports";
			case WALLS: return "Walls";
			case COINS: return "Coins";
			case BOMBS: return "Bombs";
			case PLAYERS: return "Players";
			case ENEMIES: return "Enemies";
			case BULLETS: return "Bullets";
			case BOSSES: return "Bosses";
			case TALL_ENTITIES: return "Tall_entities";
			case HAUNTING_SPIRIT_BOSS: return "Haunting_spirit_boss";
			case BOSS_BULLETS: return "Boss_bullets";
			case FOG: return "Fog";
			case BOSS_EXPLOSIONS: return "Boss_explosions";
			case FLASHES: return "Flashes";
			case DROPPING_TEXTS: return "Dropping_texts";
			case TORCHES: return "Torches";
			case UI: return "Ui";
			case POINTS: return "Points";
			}
			return "[none]";
		}
	}
}

}
