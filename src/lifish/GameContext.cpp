#include "GameContext.hpp"

using Game::GameContext;

GameContext::GameContext(sf::Window& window, const std::string& levelsetName, unsigned short startLv)
	: Game::WindowContext()
	, lm()
	, wlHandler(lm)
	, sidePanel(lm)
{
	handlers.push(Game::BaseEventHandler());
#ifndef RELEASE
	handlers.push(Game::Debug::DebugEventHandler(*this));
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
}

void GameContext::update() {

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
