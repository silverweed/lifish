#include "DebugEventHandler.hpp"
#include "Bonusable.hpp"
#include "Boss.hpp"
#include "BreakableWall.hpp"
#include "DebugPainter.hpp"
#include "Drawable.hpp"
#include "Enemy.hpp"
#include "FadeoutTextManager.hpp"
#include "GameContext.hpp"
#include "Killable.hpp"
#include "Lifed.hpp"
#include "Music.hpp"
#include "MusicManager.hpp"
#include "Options.hpp"
#include "Player.hpp"
#include "Time.hpp"
#include "collision_utils.hpp"
#include "game.hpp"
#include <iomanip>
#include <iostream>
#include <map>

using lif::debug::DebugEventHandler;

static std::string _printEntitiesDetails(const lif::EntityGroup& entities);
static void _printHelp();

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
 * J : kill player 2
 * K : pause game simulation
 * L : step game simulation (pauses if needed)
 * M : morph all enemies
 * N : kill all enemies
 * \ : print number of entities
 * . : give infinite shield to player
 * + : forward one level
 * - : back one level
 * ? : print help
 * Numpad1 : slow down time
 * Numpad3 : destroy all breakable walls
 * Numpad4 : speed up time
 * Numpad6 : flip all entities
 * Numpad7 : compute and show free tiles
 * Numpad8 : kill player 1
 * Numpad9 : rotate all entities of pi/4
 */
bool DebugEventHandler::handleEvent(sf::Window&, sf::Event event) {
	switch (event.type) {
	case sf::Event::KeyPressed:
		switch (event.key.code) {
		case sf::Keyboard::PageDown:
			{
				lif::time.setTimeScale(std::max(0.01, lif::time.getTimeScale() - 0.1));
				std::stringstream ss;
				ss << "Time Scale = " << std::setprecision(2) << lif::time.getTimeScale();
				lif::fadeoutTextMgr->add(ss.str());
				return true;
			}

		case sf::Keyboard::Numpad3:
			game.lm.getEntities().apply([] (lif::Entity *e) {
				auto w = dynamic_cast<lif::BreakableWall*>(e);
				if (w) w->get<lif::Killable>()->kill();
			});
			return true;

		case sf::Keyboard::PageUp:
			{
				lif::time.setTimeScale(std::max(0.01, lif::time.getTimeScale() + 0.1));
				std::stringstream ss;
				ss << "Time Scale = " << std::setprecision(2) << lif::time.getTimeScale();
				lif::fadeoutTextMgr->add(ss.str());
				return true;
			}

		case sf::Keyboard::Numpad6:
			game.lm.getEntities().apply([] (lif::Entity *e) {
				auto d = e->get<lif::Drawable>();
				if (d) {
					d->setScaleOrigin(lif::TILE_SIZE/2, lif::TILE_SIZE/2);
					d->setScale(-d->getScale().x, 1);
				}
			});
			return true;

		case sf::Keyboard::Numpad7:
			{
				const auto free = lif::collision_utils::findFreeTiles(game.lm);
				for (const auto& tile : free) {
					lif::debugPainter->addRectangleAt(
						sf::Vector2f(tile * lif::TILE_SIZE), sf::Vector2f(lif::TILE_SIZE, lif::TILE_SIZE),
						sf::Color(0, 160, 100, 128));
				}
				game.toggleDebug(lif::GameContext::DBG_NO_PAINT_CLEAR);
			}
			return true;

		case sf::Keyboard::Numpad8:
			game.lm.getPlayer(1)->setRemainingLives(0);
			game.lm.getPlayer(1)->get<lif::Killable>()->kill();
			return true;

		case sf::Keyboard::Numpad9:
			game.lm.getEntities().apply([] (lif::Entity *e) {
				auto d = e->get<lif::Drawable>();
				if (d) {
					d->setRotOrigin(lif::TILE_SIZE/2, lif::TILE_SIZE/2);
					d->setRotation(d->getRotation() + lif::degrees(90));
				}
			});
			return true;

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
					std::cout << i++ << ": " << e->toString() << std::endl;
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
			game.lm.getPlayer(2)->setRemainingLives(0);
			game.lm.getPlayer(2)->get<lif::Killable>()->kill();
			return true;

		case sf::Keyboard::K:
			if (!game.lm.isPaused())
				game.lm.pause();
			else
				game.lm.resume();
			return true;

		case sf::Keyboard::L:
			if (game.lm.isPaused()) {
				lif::time.addTime(sf::seconds(1.0 / lif::options.framerateLimit));
				game.lm.update();
			} else {
				game.lm.pause();
			}
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
				if (en) en->get<lif::Killable>()->kill();
			});
			return true;

		case sf::Keyboard::Slash:
			if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
				return false;
			_printHelp();
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
			game.onLevelStart();
			game.getWLHandler().state = lif::WinLoseHandler::State::DEFAULT;
			return true;

		case sf::Keyboard::Subtract:
			{
				int lvnum = game.lm.getLevel()->getInfo().levelnum - 1;
				if (lvnum < 1)
					lvnum = game.ls.getLevelsNum();
				game.lm.setLevel(game.ls, lvnum);
				lif::musicManager->set(game.lm.getLevel()->get<lif::Music>()->getMusic())
					.setVolume(lif::options.musicVolume).play();
				game.getWLHandler().state = lif::WinLoseHandler::State::DEFAULT;
				return true;
			}

		case sf::Keyboard::BackSlash:
			std::cout << _printEntitiesDetails(game.lm.getEntities());
			break;

		default:
			break;
		}
	default:
		break;
	}
	return false;
}

std::string _printEntitiesDetails(const lif::EntityGroup& entities) {
	std::map<std::string, unsigned> count;
	std::size_t largest = 1;
	entities.apply([&count, &largest] (const lif::Entity *e) {
		++count["all"];
		const auto tname = e->getTypeName();
		++count[tname];
		largest = std::max(largest, tname.length() + 1);
	});
	std::stringstream ss;
	ss << "# Entities: " << count["all"] << "\n";
	for (const auto& pair : count) {
		if (pair.first == "all") continue;
		ss << "    " << std::setw(largest) << std::left << pair.first << ": "
			<< std::right << pair.second << "\n";
	}
	ss << std::endl;
	return ss.str();
}

void _printHelp() {
	std::cout << "\n====== DEBUG COMMANDS (with US layout): ======\n"
		<< "B : kill all bosses\n"
		<< "C : print all entities\n"
		<< "F : print CD stats\n"
		<< "G : draw colliders\n"
		<< "H : draw SH cells\n"
		<< "I : print game stats\n"
		<< "J : kill player 1\n"
		<< "K : pause game simulation\n"
		<< "L : step game simulation (pauses if needed)\n"
		<< "M : morph all enemies\n"
		<< "N : kill all enemies\n"
		<< "Q : quit game\n"
		<< "\\ : print number of entities\n"
		<< ". : give infinite shield to player\n"
		<< "+ : forward one level\n"
		<< "- : back one level\n"
		<< "? : print help\n"
		<< "Num0 : toggle draw stats display\n"
		<< "PageDown : slow down time\n"
		<< "Numpad3  : destroy all breakable walls\n"
		<< "PageUp   : speed up time\n"
		<< "Numpad6  : flip all entities\n"
		<< "Numpad7  : compute and show free tiles\n"
		<< "Numpad9  : rotate all entities of pi/4\n"
		<< std::endl;
}
