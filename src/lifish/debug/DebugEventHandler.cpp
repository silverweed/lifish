#include "DebugEventHandler.hpp"
#include "Enemy.hpp"
#include "Killable.hpp"
#include "game.hpp"
#include "Player.hpp"
#include "Options.hpp"
#include "Music.hpp"
#include "Boss.hpp"
#include "Enemy.hpp"
#include "MusicManager.hpp"
#include "GameContext.hpp"

using Game::Debug::DebugEventHandler;

DebugEventHandler::DebugEventHandler(Game::GameContext& game)
	: game(game)
{}

bool DebugEventHandler::handleEvent(sf::Window& window, sf::Event event) {
	switch (event.type) {
	case sf::Event::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::Q:
			Game::terminated = true;
			return true;
		case sf::Keyboard::J:
			game.players[0]->setRemainingLives(0);
			game.players[0]->get<Game::Killable>()->kill();
			return true;
		case sf::Keyboard::M:
			game.lm.getEntities().apply([] (Game::Entity *e) {
				auto en = dynamic_cast<Game::Enemy*>(e);
				if (en) en->setMorphed(!en->isMorphed());
			});
			return true;
		case sf::Keyboard::N:
			game.lm.getEntities().apply([] (Game::Entity *e) {
				auto en = dynamic_cast<Game::Enemy*>(e);
				//auto en = dynamic_cast<Game::BreakableWall*>(e);
				if (en) en->get<Game::Killable>()->kill();
			});
			return true;
		case sf::Keyboard::B:
			game.lm.getEntities().apply([] (Game::Entity *e) {
				auto en = dynamic_cast<Game::Boss*>(e);
				if (en) en->get<Game::Killable>()->kill();
			});
			return true;
			/* TODO
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
			*/
		case sf::Keyboard::L:
			if (game.lm.isPaused())
				game.lm.update();
			else
				game.lm.pause();
			return true;
		case sf::Keyboard::K:
			if (!game.lm.isPaused())
				game.lm.pause();
			else
				game.lm.resume();
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
