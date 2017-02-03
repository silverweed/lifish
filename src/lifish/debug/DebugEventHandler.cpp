#include "DebugEventHandler.hpp"
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
#include <iostream>

using lif::debug::DebugEventHandler;

DebugEventHandler::DebugEventHandler(lif::GameContext& game)
	: game(game)
{}

/*
 * B : kill all bosses
 * C : print all entities
 * F : print CD stats
 * G : draw colliders
 * H : draw SH cells
 * I : print game stats
 * J : kill player 1
 * K : pause game simulation
 * L : step game simulation (pauses if needed)
 * M : morph all enemies
 * N : kill all enemies
 * . : give infinite shield to player
 * + : forward one level
 * - : back one level
 */
bool DebugEventHandler::handleEvent(sf::Window&, sf::Event event) {
	switch (event.type) {
	case sf::Event::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::B:
			game.lm.getEntities().apply([] (lif::Entity *e) {
				auto en = dynamic_cast<lif::Boss*>(e);
				if (en) en->get<lif::Killable>()->kill();
			});
			return true;
		case sf::Keyboard::C:
			{
				int i = 0;
				game.lm.getEntities().apply([&i] (const lif::Entity *e) {
					std::cerr << i++ << ": " << e->toString() << std::endl;
				});
			}
			return true;
		case sf::Keyboard::F:
			game.toggleDebug(lif::GameContext::DBG_PRINT_CD_STATS);
			return true;
		case sf::Keyboard::G:
			game.toggleDebug(lif::GameContext::DBG_DRAW_COLLIDERS);
			return true;
		case sf::Keyboard::H:
			game.toggleDebug(lif::GameContext::DBG_DRAW_SH_CELLS);
			return true;
		case sf::Keyboard::I:
			game.toggleDebug(lif::GameContext::DBG_PRINT_GAME_STATS);
			return true;
		case sf::Keyboard::J:
			game.lm.getPlayer(1)->setRemainingLives(0);
			game.lm.getPlayer(1)->get<lif::Killable>()->kill();
			return true;
		case sf::Keyboard::K:
			if (!game.lm.isPaused())
				game.lm.pause();
			else
				game.lm.resume();
			return true;
		case sf::Keyboard::L:
			if (game.lm.isPaused())
				game.lm.update();
			else
				game.lm.pause();
			return true;
		case sf::Keyboard::M:
			game.lm.getEntities().apply([] (lif::Entity *e) {
				auto en = dynamic_cast<lif::Enemy*>(e);
				if (en) en->setMorphed(!en->isMorphed());
			});
			return true;
		case sf::Keyboard::N:
			game.lm.getEntities().apply([] (lif::Entity *e) {
				auto en = dynamic_cast<lif::Enemy*>(e);
				//auto en = dynamic_cast<lif::BreakableWall*>(e);
				if (en) en->get<lif::Killable>()->kill();
			});
			return true;
		case sf::Keyboard::Period:
			{
				auto bns = game.lm.getPlayer(1)->get<lif::Bonusable>();
				if (bns->hasBonus(lif::BonusType::SHIELD))
					bns->giveBonus(lif::BonusType::SHIELD, sf::seconds(0));
				else
					bns->giveBonus(lif::BonusType::SHIELD, sf::seconds(-1));
			}
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
				lif::musicManager->set(game.lm.getLevel()->get<lif::Music>()->getMusic())
					.setVolume(lif::options.musicVolume).play();
				return true;
			}
		default: 
			break;
		}
	default: 
		break;
	}
	return false;
}
