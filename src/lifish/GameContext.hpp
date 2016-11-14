#pragma once

#include <string>
#include <vector>
#include <memory>
#include "WindowContext.hpp"
#include "LevelSet.hpp"
#include "LevelManager.hpp"
#include "WinLoseHandler.hpp"
#include "SidePanel.hpp"

namespace Game {

class Level;
class Player;

class GameContext : public Game::WindowContext {
public:
	enum : unsigned int {
		DBG_DRAW_COLLIDERS = 1,
		DBG_DRAW_SH_CELLS  = 1 << 1,
		DBG_PRINT_CD_STATS = 1 << 2
	};
private:
	unsigned int debug = 0;

	Game::LevelSet ls;
	Game::LevelManager lm;
	Game::WinLoseHandler wlHandler;
	Game::SidePanel sidePanel;
	std::unique_ptr<Game::Level> level;
	std::vector<std::shared_ptr<Game::Player>> players;
	int lvnum;

public:
	GameContext(sf::Window& window, const std::string& levelsetName, unsigned short startLv);

	void update() override;
	bool handleEvent(sf::Window& window, sf::Event evt) override;

	void toggleDebug(unsigned int dbg);
};

}
