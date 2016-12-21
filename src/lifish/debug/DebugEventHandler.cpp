#include "DebugEventHandler.hpp"
#include "Enemy.hpp"
#include "Killable.hpp"
#include "game.hpp"
#include "Player.hpp"
#include "Options.hpp"
#include "Music.hpp"
#include "Boss.hpp"
#include "Enemy.hpp"
#include "Bonusable.hpp"
#include "MusicManager.hpp"
#include "GameContext.hpp"

using Game::Debug::DebugEventHandler;

DebugEventHandler::DebugEventHandler(Game::GameContext& game)
	: game(game)
{}

bool DebugEventHandler::handleEvent(sf::Window&, sf::Event event) {
	switch (event.type) {
	case sf::Event::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::Q:
			Game::terminated = true;
			return true;
		case sf::Keyboard::J:
			game.lm.getPlayer(1)->setRemainingLives(0);
			game.lm.getPlayer(1)->get<Game::Killable>()->kill();
			return true;
		case sf::Keyboard::Period:
			game.lm.getPlayer(1)->get<Game::Bonusable>()->giveBonus(Game::BonusType::SHIELD, sf::seconds(-1));
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
		case sf::Keyboard::Add:
			game._advanceLevel();
			return true;
		case sf::Keyboard::Subtract:
			{
				int lvnum = game.lm.getLevel()->getInfo().levelnum - 1;
				if (lvnum < 1) 
					lvnum = game.ls.getLevelsNum();
				game.lm.setLevel(game.ls, lvnum);
				Game::musicManager->set(game.lm.getLevel()->get<Game::Music>()->getMusic())
					.setVolume(Game::options.musicVolume).play();
				return true;
			}
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
		case sf::Keyboard::I:
			game.toggleDebug(Game::GameContext::DBG_PRINT_GAME_STATS);
			return true;
		default: 
			break;
		}
	default: 
		break;
	}
	return false;
}
