#include "GameContext.hpp"
#include "Killable.hpp"
#include "Music.hpp"
#include "BaseEventHandler.hpp"
#include "Options.hpp"
#include "core.hpp"
#include "contexts.hpp"
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
#endif

using Game::GameContext;

GameContext::GameContext(sf::Window& window, const std::string& levelsetName, unsigned short startLv)
	: Game::WindowContext()
	, window(window)
	, lm()
	, sidePanel(lm)
	, wlHandler(lm, sidePanel)
{
	handlers.push_back(std::unique_ptr<Game::EventHandler>(new Game::BaseEventHandler));
#ifndef RELEASE
	handlers.push_back(std::unique_ptr<Game::EventHandler>(new Game::Debug::DebugEventHandler(*this)));
#endif

	int lvnum = startLv;
	ls.loadFromFile(levelsetName);
	if (lvnum > ls.getLevelsNum())
		lvnum %= ls.getLevelsNum();

	// Create the players
	players = lm.createNewPlayers();
	for (auto p : players)
		p->get<Game::Controllable>()->setWindow(window);

	lm.setLevel(ls, lvnum);

	const auto level = lm.getLevel();
	if (level == nullptr)
		throw std::invalid_argument("Level " + Game::to_string(lvnum) + " not found in levelset!");

	// Setup the music
	Game::musicManager->set(level->get<Game::Music>()->getMusic()); 

	// Ensure lm is not paused
	lm.resume();
}

void GameContext::setActive(bool b) {
	Game::Activable::setActive(b);
	if (active)
		lm.resume();
}

void GameContext::update() {
	// Handle win / loss cases
	wlHandler.handleWinLose();
	switch (wlHandler.getState()) {
	case Game::WinLoseHandler::State::ADVANCING_LEVEL:
		newContext = Game::CTX_INTERLEVEL;
		return;
	case Game::WinLoseHandler::State::ADVANCED_LEVEL:
		_advanceLevel();	
		break;
	case Game::WinLoseHandler::State::EXIT_GAME:
		newContext = Game::CTX_UI;
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
		newContext = Game::CTX_UI;
	};
	switch (event.type) {
	case sf::Event::JoystickButtonPressed:
		{
			const auto btn = event.joystickButton;
			const short pb = JoystickUtils::getPauseButton(btn.joystickId);
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
			for (auto player : players) {
				player->setRemainingLives(0);
				player->get<Game::Killable>()->kill();
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

void GameContext::setOrigin(const sf::Vector2f& o) {
	Game::WindowContext::setOrigin(o);
	lm.setOrigin(o);

}

void GameContext::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	window.draw(lm, states);
	window.draw(sidePanel, states);
#ifndef RELEASE
	if ((debug >> DBG_DRAW_COLLIDERS) & 1)
		Debug::DebugRenderer::drawColliders(window, lm.getEntities());
	if ((debug >> DBG_DRAW_SH_CELLS) & 1)
		Debug::DebugRenderer::drawSHCells(window,
				static_cast<const Game::SHCollisionDetector&>(
					lm.getCollisionDetector()));
#endif
}

#ifndef RELEASE
void GameContext::toggleDebug(unsigned int flag) {
	debug ^= 1 << flag;
}

void GameContext::_printCDStats() const {
	const auto& dbgStats = lm.getCollisionDetector().getStats();
	std::cerr << std::setfill(' ') << std::scientific << std::setprecision(4)
		<< "#checked: " << std::setw(5) << dbgStats.counter.safeGet("checked")
		<< " | tot: " << std::setw(8) << dbgStats.timer.safeGet("tot")
		<< " | tot_narrow: " << std::setw(8) << dbgStats.timer.safeGet("tot_narrow")
		<< " | setup: " << std::setw(8) << dbgStats.timer.safeGet("setup") 
		<< " | average: " << std::setw(8) 
			<< dbgStats.timer.safeGet("tot_narrow")/dbgStats.counter.safeGet("checked")
		<< std::resetiosflags(std::ios::showbase) << std::endl;
}

void GameContext::_printGameStats() const {
	const auto& dbgStats = lm.getStats();
	const auto timers = {
		"tot", "reset_align", "validate", "cd", "logic",
		"logic_0",
		"logic_1",
		"logic_2",
		"logic_3",
		"logic_4",
		"logic_5",  
		"ent_update", "checks" };
	std::cerr << std::setfill(' ') << std::scientific << std::setprecision(3);
	for (const auto& t : timers) {
		std::cerr << " | " << t << ": " << std::setw(7) << dbgStats.timer.safeGet(t);
	}
	std::cerr << std::resetiosflags(std::ios::showbase) << std::endl;
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
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
		auto player = lm.getPlayer(i + 1);
		if ((player == nullptr || player->get<Game::Killable>()->isKilled())) {
			if (Game::playerContinues[i] > 0) {
				--Game::playerContinues[i];
				auto player = std::make_shared<Player>(sf::Vector2f(0, 0), i + 1);
				player->get<Game::Controllable>()->setWindow(window); 
				lm.setPlayer(i + 1, player);
			} else {
				lm.removePlayer(i + 1);
			}
		} else if (player != nullptr) {
			auto bns = player->get<Game::Bonusable>();
			bns->expireTemporaryBonuses();
		}
	}

	lm.setNextLevel();
	{
		const auto level = lm.getLevel();
		if (level == nullptr)
			throw std::logic_error("No levels found after " + Game::to_string(lvnum) 
					+ " but end game not triggered!");
		Game::musicManager->set(level->get<Game::Music>()->getMusic())
			.setVolume(Game::options.musicVolume)
			.play();
	}
}
