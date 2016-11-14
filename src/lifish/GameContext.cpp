#include "GameContext.hpp"
#ifndef RELEASE
#	include <iostream>
#endif

using Game::GameContext;

GameContext::GameContext(sf::Window& window, const std::string& levelsetName, unsigned short startLv)
	: Game::WindowContext()
	, lm()
	, wlHandler(lm)
	, sidePanel(lm)
{
	handlers.push(std::unique_ptr<Game::EventHandler>(new Game::BaseEventHandler));
#ifndef RELEASE
	handlers.push(std::unique_ptr<Game::EventHandler>(new Game::Debug::DebugEventHandler(*this)));
#endif

	int lvnum = startLv;
	ls.loadFromFile(levelset_name);
	if (lvnum > ls.getLevelsNum())
		lvnum %= ls.getLevelsNum();
	level = std::unique_ptr<Game::Level>(ls.getLevel(lvnum));

	// Create the players
	players = lm.createNewPlayers();
	for (auto p : players)
		p->get<Game::Controllable>()->setWindow(window);

	lm.setLevel(*level);
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

	// Update level
	if (!lm.isPaused())
		lm.update();

#	ifndef RELEASE
	if (cycle++ % 50 == 0 && (debug >> DBG_PRINT_CD_STATS) == 1)
		_printCDStats(lm);
#	endif

	sidePanel.update();
}

bool GameContext::handleEvent(sf::Window& window, sf::Event event) {
	switch (event.type) {
	case sf::Event::JoystickButtonPressed:
		{
			const auto btn = event.joystickButton;
			const short pb = JoystickUtils::getPauseButton(btn.joystickId);
			if (pb >= 0 && btn.button == static_cast<unsigned int>(pb))
				toggle_pause_game(ui, lm, was_ui_active);
			return true;
		}
	case sf::Event::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::P:
			toggle_pause_game(ui, lm, was_ui_active);
			return true;
		case sf::Keyboard::Escape:
			for (auto player : players) {
				player->setRemainingLives(0);
				player->get<Killable>()->kill();
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

void GameContext::toggleDebug(unsigned int flag) {
	debug ^= 1 << flag;
}

void GameContext::setOrigin(const sf::Vector2f& o) {
	Game::WindowContext::setOrigin(o);
	lm.setOrigin(o);

}

void GameContext::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	window.draw(lm, states);
	window.draw(sidePanel, states);
}

#ifndef RELEASE
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

void GameContex::_togglePauseGame(UI::UI& ui, LevelManager& lm, bool& was_ui_active) {
	if (ui.toggleActive()) {
		lm.pause();
		was_ui_active = true;
		Game::musicManager->pause();
	} else {
		Game::musicManager->play();
	}
}

