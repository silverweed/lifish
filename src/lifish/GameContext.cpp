#include "GameContext.hpp"
#include "Killable.hpp"
#include "Music.hpp"
#include "BaseEventHandler.hpp"
#include "Options.hpp"
#include "core.hpp"
#include "contexts.hpp"
#include "input_utils.hpp"
#include "MusicManager.hpp"
#include "Bonusable.hpp"
#include "bonus_type.hpp"
#include "Player.hpp"
#include "Controllable.hpp"
#ifndef RELEASE
#	include <iostream>
#	include <iomanip>
#	include "DebugRenderer.hpp"
#	include "DebugEventHandler.hpp"
#	include "game_logic.hpp"
#endif

using lif::GameContext;

GameContext::GameContext(sf::Window& window, const std::string& levelsetName, short startLv)
	: lif::WindowContext()
	, window(window)
	, lm()
	, sidePanel(lm)
	, wlHandler(lm, sidePanel)
{
	_addHandler<lif::BaseEventHandler>();
#ifndef RELEASE
	_addHandler<lif::debug::DebugEventHandler>(std::ref(*this));
#endif
	gameRenderTex.create((lif::LEVEL_WIDTH + 2) * lif::TILE_SIZE, (lif::LEVEL_HEIGHT + 2) * lif::TILE_SIZE);
	sidePanelRenderTex.create(lif::SIDE_PANEL_WIDTH, (lif::LEVEL_HEIGHT + 2) * lif::TILE_SIZE);

	ls.loadFromFile(levelsetName);
	_initLM(window, startLv);
}

void GameContext::_initLM(sf::Window& window, short lvnum) {
	if (lvnum > ls.getLevelsNum())
		lvnum %= ls.getLevelsNum();
	else while (lvnum <= 0)
		lvnum += ls.getLevelsNum();

	// Create the players
	lm.createNewPlayers();
	for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
		auto p = lm.getPlayer(i + 1);
		if (p != nullptr)
			p->get<lif::Controllable>()->setWindow(window);
	}

	lm.setLevel(ls, lvnum);

	const auto level = lm.getLevel();
	if (level == nullptr)
		throw std::invalid_argument("Level " + lif::to_string(lvnum) + " not found in levelset!");

	// Setup the music
	lif::musicManager->set(level->get<lif::Music>()->getMusic())
			.setVolume(lif::options.musicVolume).play();

	// Ensure lm is not paused
	lm.resume();
}

void GameContext::setActive(bool b) {
	lif::Activable::setActive(b);
	if (active)
		lm.resume();
}

void GameContext::update() {
	// Handle win / loss cases
	wlHandler.handleWinLose();
	switch (wlHandler.getState()) {
		using S = lif::WinLoseHandler::State;
	case S::ADVANCING_LEVEL:
		newContext = lif::CTX_INTERLEVEL;
		return;
	case S::ADVANCED_LEVEL:
		_advanceLevel();
		break;
	case S::EXIT_GAME:
		newContext = lif::CTX_UI;
		break;
	default: 
		break;
	}

	// Update level
	if (!lm.isPaused()) {
		lm.update();
		sidePanel.update();
	}

#ifndef RELEASE
	if (cycle % 50 == 0 && (debug >> DBG_PRINT_CD_STATS) & 1)
		_printCDStats();
	if (cycle % 50 == 0 && (debug >> DBG_PRINT_GAME_STATS) & 1)
		_printGameStats();
	++cycle;
#endif
}

bool GameContext::handleEvent(sf::Window&, sf::Event event) {
	auto pause_game = [this] () {
		lm.pause();
		newContext = lif::CTX_UI;
	};
	switch (event.type) {
	case sf::Event::JoystickButtonPressed:
		{
			const auto btn = event.joystickButton;
			const short pb = lif::joystick::getButton(lif::joystick::ButtonType::START, btn.joystickId);
			if (pb >= 0 && btn.button == static_cast<unsigned int>(pb))
				pause_game();
			return true;
		}
	case sf::Event::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::P:
			pause_game();
			return true;
		case sf::Keyboard::Escape:
			for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
				auto player = lm.getPlayer(i + 1);
				if (player != nullptr) {
					player->setRemainingLives(0);
					player->get<lif::Killable>()->kill();
				}
			}
			return true;
		default: 
			break;
		}
	default: 
		break;
	}
	return false;
}

void GameContext::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	// Draw the LevelManager in its render texture
	gameRenderTex.clear();
	gameRenderTex.draw(lm, states);
#ifndef RELEASE
	if ((debug >> DBG_DRAW_COLLIDERS) & 1)
		debug::DebugRenderer::drawColliders(gameRenderTex, lm.getEntities());
	if ((debug >> DBG_DRAW_SH_CELLS) & 1)
		debug::DebugRenderer::drawSHCells(gameRenderTex,
				static_cast<const lif::SHCollisionDetector&>(
					lm.getCollisionDetector()));
#endif
	gameRenderTex.display();

	// Draw the SidePanel in its render texture
	sidePanelRenderTex.clear();
	sidePanelRenderTex.draw(sidePanel, states);
	sidePanelRenderTex.display();
	
	// Draw both textures to window
	sf::Sprite gameSprite(gameRenderTex.getTexture());
	gameSprite.setOrigin(origin);
	window.draw(gameSprite, states);

	lm.getLevelEffects().blendEffects(lm, window, origin);

	sf::Sprite sidePanelSprite(sidePanelRenderTex.getTexture());
	window.draw(sidePanelSprite, states);
}

#ifndef RELEASE
void GameContext::toggleDebug(unsigned int flag) {
	debug ^= 1 << flag;
}

void GameContext::_printCDStats() const {
	const auto& dbgStats = lm.getCollisionDetector().getStats();
	std::ios::fmtflags flags(std::cerr.flags());
	std::cerr << std::setfill(' ') << std::scientific << std::setprecision(4)
		<< "#checked: " << std::setw(5) << dbgStats.counter.safeGet("checked")
		<< " | tot: " << std::setw(8) << dbgStats.timer.safeGet("tot")
		<< " | tot_narrow: " << std::setw(8) << dbgStats.timer.safeGet("tot_narrow")
		<< " | setup: " << std::setw(8) << dbgStats.timer.safeGet("setup") 
		<< " | average: " << std::setw(8) 
			<< dbgStats.timer.safeGet("tot_narrow")/dbgStats.counter.safeGet("checked")
		<< std::endl;
	std::cerr.flags(flags);
}

static float get_percentage(const lif::debug::Stats& stats, const char *totn, const char *name, char *percentage) {
	const auto tot = stats.timer.safeGet(totn);
	if (tot < 0) return -1;
	const float ratio = stats.timer.safeGet(name) / tot;
	if (ratio < 0) return -1;
	for (unsigned i = 0; i < 20; ++i) {
		if (ratio < i/20.) break;
		percentage[i] = '|';
	}
	return ratio;
}

void GameContext::_printGameStats() const {
	const auto& dbgStats = lm.getStats();
	const auto timers = { "tot", "reset_align", "validate", "cd", "logic", "ent_update", "checks" };
	std::ios::fmtflags flags(std::cerr.flags());
	std::cerr << "-------------";
	std::cerr << std::setfill(' ') << std::scientific << std::setprecision(3);
	for (const auto& t : timers) {
		char percentage[21] = {0};
		const float ratio = get_percentage(dbgStats, "tot", t, percentage);
		std::cerr << "\r\n | " << std::left << std::setw(12) << t << ": " 
			<< std::setw(7) << dbgStats.timer.safeGet(t) << " " << percentage
			<< (ratio >= 0 ? " " + lif::to_string(static_cast<int>(ratio*100)) + "%" : "");
	}
	std::cerr << "\r\n -- logic: --";
	for (unsigned i = 0; i < lif::game_logic::functions.size(); ++i) {
		std::stringstream t;
		t << "logic_" << i;
		char percentage[21] = {0};
		const float ratio = get_percentage(dbgStats, "logic", t.str().c_str(), percentage);
		std::cerr << "\r\n | " << i << ": " << std::setw(7) << dbgStats.timer.safeGet(t.str())
			<< " " << percentage
			<< (ratio >= 0 ? " " + lif::to_string(static_cast<int>(ratio*100)) + "%" : "");
	}
	std::cerr << std::endl;
	std::cerr.flags(flags);
}
#endif

void GameContext::_advanceLevel() {
	const auto level = lm.getLevel();
	if (level == nullptr)
		throw std::logic_error("Called _advanceLevel on null level!");
	short lvnum = level->getInfo().levelnum;
	const auto& ls = level->getLevelSet();

	if (lvnum == ls.getLevelsNum()) {
		// TODO game won
		//return;
	}

	// Resurrect any dead player which has a 'continue' left and
	// remove temporary effects
	for (unsigned i = 0; i < lif::MAX_PLAYERS; ++i) {
		auto player = lm.getPlayer(i + 1);
		if ((player == nullptr || player->get<lif::Killable>()->isKilled())) {
			if (lif::playerContinues[i] > 0) {
				--lif::playerContinues[i];
				auto player = std::make_shared<Player>(sf::Vector2f(0, 0), i + 1);
				player->get<lif::Controllable>()->setWindow(window); 
				lm.setPlayer(i + 1, player);
			} else {
				lm.removePlayer(i + 1);
			}
		} else if (player != nullptr) {
			auto bns = player->get<lif::Bonusable>();
			bns->expireTemporaryBonuses();
		}
	}

	lm.setNextLevel();
	{
		const auto level = lm.getLevel();
		if (level == nullptr)
			throw std::logic_error("No levels found after " + lif::to_string(lvnum) 
					+ " but end game not triggered!");
		lif::musicManager->set(level->get<lif::Music>()->getMusic())
			.setVolume(lif::options.musicVolume)
			.play();
	}
}
