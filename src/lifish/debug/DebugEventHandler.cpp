#include "DebugEventHandler.hpp"
#include "Enemy.hpp"
#include "Killable.hpp"
#include "game.hpp"

using Game::Debug::DebugEventHandler;

DebugEventHandler::DebugEventHandler(Game::GameContext& game)
	: game(game)
{}

void DebugEventHandler::update() {}

bool DebugEventHandler::handleEvent(sf::Window& window, sf::Event event) {
	switch (event.type) {
	case sf::Event::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::Q:
			Game::terminated = true;
			return true;
		case sf::Keyboard::J:
			players[0]->setRemainingLives(0);
			players[0]->get<Game::Killable>()->kill();
			return true;
		case sf::Keyboard::M:
			lm.getEntities().apply([] (Game::Entity *e) {
				auto en = dynamic_cast<Game::Enemy*>(e);
				if (en) en->setMorphed(!en->isMorphed());
			});
			return true;
		case sf::Keyboard::N:
			lm.getEntities().apply([] (Game::Entity *e) {
				auto en = dynamic_cast<Game::Enemy*>(e);
				//auto en = dynamic_cast<Game::BreakableWall*>(e);
				if (en) en->get<Game::Killable>()->kill();
			});
			return true;
		case sf::Keyboard::B:
			lm.getEntities().apply([] (Game::Entity *e) {
				auto en = dynamic_cast<Game::Boss*>(e);
				if (en) en->get<Game::Killable>()->kill();
			});
			return true;
		case sf::Keyboard::Add:
			lvnum = level->getInfo().levelnum + 1;
			if (lvnum > ls.getLevelsNum())
				lvnum = 1;
			level = ls.getLevel(lvnum);
			Game::musicManager->set(level->get<Game::Music>()->getMusic())
				.setVolume(Game::options.musicVolume).play();
			lm.setLevel(*level);
			return true;
		case sf::Keyboard::Subtract:
			lvnum = level->getInfo().levelnum - 1;
			if (lvnum < 1) 
				lvnum = ls.getLevelsNum();
			level = ls.getLevel(lvnum);
			Game::musicManager->set(level->get<Game::Music>()->getMusic())
				.setVolume(Game::options.musicVolume).play();
			lm.setLevel(*level);
			return true;
		case sf::Keyboard::L:
			if (lm.isPaused())
				lm.update();
			else
				lm.pause();
			return true;
		case sf::Keyboard::K:
			if (!lm.isPaused())
				lm.pause();
			else
				lm.resume();
			return true;
		case sf::Keyboard::G:
			game.toggleDebug(Game::GameContext::DBG_DRAW_COLLIDERS);
			return true;
		case sf::Keyboard::H:
			game.toggleDebug(Game::GameContext::DBG_DRAW_SH_CELLS);
			return true;
		case sf::Keyboard::F:
			game.toggleDebug(Game::GameContext::DBG_PRINT_CD_STATS);
			return true;
		default: 
			break;
		}
	default: 
		break;
	}
	return false;
}
