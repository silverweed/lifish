#pragma once

#include "LevelManager.hpp"
#include "LevelSet.hpp"
#include "SidePanel.hpp"
#include "WinLoseHandler.hpp"
#include "WindowContext.hpp"
#include <memory>
#include <string>
#include <vector>

namespace lif {

class Player;
struct SaveData;

#ifndef RELEASE
namespace debug {

class DebugEventHandler;

}
#endif

class GameContext : public lif::WindowContext {
public:
	enum : unsigned {
		DBG_DRAW_COLLIDERS   = 1,
		DBG_DRAW_SH_CELLS    = 1 << 1,
		DBG_PRINT_CD_STATS   = 1 << 2,
		DBG_PRINT_GAME_STATS = 1 << 3,
		DBG_NO_PAINT_CLEAR      = 1 << 4
	};
private:
#ifndef RELEASE
	friend class lif::debug::DebugEventHandler;

	unsigned debug = 0;
	unsigned cycle = 0;

#endif
	mutable sf::RenderTexture gameRenderTex;
	mutable sf::RenderTexture sidePanelRenderTex;

	const sf::Window& window;
	lif::LevelSet ls;
	lif::LevelManager lm;
	lif::SidePanel sidePanel;
	lif::WinLoseHandler wlHandler;

	void _initLM(const sf::Window& window, short lvnum);
	void _advanceLevel();
	void _resurrectDeadPlayers();
#ifndef RELEASE
	void _printCDStats() const;
	void _printGameStats() const;
#endif

public:
	explicit GameContext(sf::Window& window, const std::string& levelsetName, short startLv);

	lif::WinLoseHandler& getWLHandler() { return wlHandler; }
	const lif::WinLoseHandler& getWLHandler() const { return wlHandler; }
	lif::LevelManager& getLM() { return lm; }
	const lif::LevelManager& getLM() const { return lm; }
	lif::SidePanel& getSidePanel() { return sidePanel; }
	const lif::SidePanel& getSidePanel() const { return sidePanel; }

	void loadGame(const lif::SaveData& saveData);
	/** Called by `main` after `GET READY` screen */
	void onLevelStart();

	void update() override;
	bool handleEvent(sf::Window& window, sf::Event evt) override;
	void draw(sf::RenderTarget& window, sf::RenderStates states) const override;
	void setActive(bool b) override;

#ifndef RELEASE
	void toggleDebug(unsigned dbg);
#endif
};

}
