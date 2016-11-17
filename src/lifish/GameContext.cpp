#include "GameContext.hpp"
#include "Killable.hpp"
#include "Music.hpp"
#include "BaseEventHandler.hpp"
#include "core.hpp"
#include "MusicManager.hpp"
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
	, lm()
	, wlHandler(lm)
	, sidePanel(lm)
{
	handlers.push_back(std::unique_ptr<Game::EventHandler>(new Game::BaseEventHandler));
#ifndef RELEASE
	handlers.push_back(std::unique_ptr<Game::EventHandler>(new Game::Debug::DebugEventHandler(*this)));
#endif

	int lvnum = startLv;
	ls.loadFromFile(levelsetName);
	if (lvnum > ls.getLevelsNum())
		lvnum %= ls.getLevelsNum();
	level = std::unique_ptr<Game::Level>(ls.getLevel(lvnum));

	// Create the players
	players = lm.createNewPlayers();
	for (auto p : players)
		p->get<Game::Controllable>()->setWindow(window);

	lm.setLevel(*level);

	// Setup the music
	//Game::options.musicVolume = 0; // FIXME
	//Game::options.soundsVolume = 0; // FIXME
	Game::musicManager->set(level->get<Game::Music>()->getMusic()); // TODO

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
	/*wlHandler.handleWinLose();
	if (wlHandler.getState() == WinLoseHandler::State::ADVANCING_LEVEL) {
		// Give bonus points/handle continues/etc
		wlHandler.advanceLevel(window, sidePanel);
		if (wlHandler.getState() == WinLoseHandler::State::GAME_WON) {
			// TODO
		}
		
		for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i)
			players[i] = lm.getPlayer(i + 1);
		level = ls.getLevel(level->getInfo().levelnum + 1);
		lm.setLevel(*level);
		Game::musicManager->set(level->get<Game::Music>()->getMusic())
			.setVolume(Game::options.musicVolume)
			.play();
		continue;
	}
*/
	// Update level
	if (!lm.isPaused())
		lm.update();

#	ifndef RELEASE
	if (cycle++ % 50 == 0 && (debug >> DBG_PRINT_CD_STATS) == 1)
		_printCDStats();
#	endif

	sidePanel.update();
}

bool GameContext::handleEvent(sf::Window& window, sf::Event event) {
	switch (event.type) {
		/* TODO
	case sf::Event::JoystickButtonPressed:
		{
			const auto btn = event.joystickButton;
			const short pb = JoystickUtils::getPauseButton(btn.joystickId);
			if (pb >= 0 && btn.button == static_cast<unsigned int>(pb))
				toggle_pause_game(ui, lm, was_ui_active);
			return true;
		}
		*/
	case sf::Event::KeyPressed:
		switch (event.key.code) {
			/*
		case sf::Keyboard::P:
			toggle_pause_game(ui, lm, was_ui_active);
			return true;
		*/
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
#endif

//void GameContext::_togglePauseGame(UI::UI& ui, Game::LevelManager& lm) {
	//if (ui.toggleActive()) {
		//lm.pause();
		//Game::musicManager->pause();
	//} else {
		//Game::musicManager->play();
	//}
//}
