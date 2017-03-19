#include "LevelLoader.hpp"
#include "game.hpp"
#include "LevelSet.hpp"
#include "Player.hpp"
#include "Animated.hpp"
#include "Level.hpp"
#include "LevelManager.hpp"
#include "FixedWall.hpp"
#include "AxisSighted.hpp"
#include "Teleport.hpp"
#include "BreakableWall.hpp"
#include "AlienBoss.hpp"
#include "AI.hpp"
#include "Flash.hpp"
#include "TransparentWall.hpp"
#include "Coin.hpp"
#include "Enemy.hpp"
#include "Lifed.hpp"
#include "Wisp.hpp"
#include "AlienPredator.hpp"
#include "AcidPond.hpp"
#include "LevelEffects.hpp"
#include "HauntedStatue.hpp"
#include "HauntingSpiritBoss.hpp"
#include "Torch.hpp"
#include "Sprite.hpp"
#include "Cage.hpp"
#include <iostream>

using lif::TILE_SIZE;

bool lif::LevelLoader::load(const lif::Level& level, lif::LevelManager& lm) {

	lm.levelTime->setTime(sf::seconds(level.getInfo().time));

	lif::Teleport *first_teleport = nullptr,
		       *latest_teleport = nullptr;

	lm.reset();
	auto& entities = lm.getEntities();

	for (unsigned top = 0; top < lif::LEVEL_HEIGHT; ++top) {
		for (unsigned left = 0; left < lif::LEVEL_WIDTH; ++left) {

			const sf::Vector2f curPos((left + 1) * TILE_SIZE, (top + 1) * TILE_SIZE);
			unsigned short enemy_id = 0;
			const auto& ls = level.getLevelSet();

			auto is_game_over = [&lm] (unsigned short id) -> bool {
				return lm.players[id] == nullptr || (
						lm.players[id]->getInfo().remainingLives <= 0
						&& lm.players[id]->get<lif::Lifed>()->getLife() <= 0
						&& lm.getPlayerContinues(id + 1) <= 0
					);
			};

			auto add_player = [&lm, &entities, is_game_over] (unsigned short id, const sf::Vector2f& pos) {
				if (!is_game_over(id)) {
					lm.players[id]->setWinning(false);
					lm.players[id]->setPosition(pos);
					lm.players[id]->get<lif::Animated>()->setAnimation("idle");
					lm.players[id]->get<lif::Moving>()->stop();
					entities.add(lm.players[id]);
					entities.add(new lif::Flash(pos));
				}
			};

			switch (level.getTile(left, top)) {
			case EntityType::FIXED:
				entities.add(new lif::FixedWall(curPos, level.getInfo().tileIDs.fixed));
				break;

			case EntityType::BREAKABLE:
				entities.add(new lif::BreakableWall(curPos, level.getInfo().tileIDs.breakable));
				break;

			case EntityType::TRANSPARENT_WALL:
				entities.add(new lif::TransparentWall(curPos));
				break;

			case EntityType::ACID_POND:
				entities.add(new lif::AcidPond(curPos, sf::Vector2f(TILE_SIZE, TILE_SIZE)));
				break;

			case EntityType::COIN:
				entities.add(new lif::Coin(curPos));
				break;

			case EntityType::HAUNTED_STATUE:
				entities.add(new lif::HauntedStatue(curPos));
				break;

			case EntityType::PLAYER1:
				add_player(0, curPos);
				break;

			case EntityType::PLAYER2:
				add_player(1, curPos);
				break;

			case EntityType::TELEPORT:
				{
					auto teleport = new lif::Teleport(curPos);

					// Save the first Teleport added
					if (first_teleport == nullptr)
						first_teleport = teleport;
					else
						teleport->linkTo(first_teleport);

					// If we had already added a Teleport, link it to this one.
					if (latest_teleport != nullptr)
						latest_teleport->linkTo(teleport);
					latest_teleport = teleport;

					entities.add(teleport);
				}
				break;

			case EntityType::ALIEN_BOSS:
				{
					auto boss = new lif::AlienBoss(curPos);
					for (auto s : boss->getAllRecursive<lif::Sighted>())
						s->setEntityGroup(&lm.entities);
					entities.add(boss);
				}
				break;

			case EntityType::HAUNTING_SPIRIT_BOSS:
				{
					auto boss = new lif::HauntingSpiritBoss(curPos);
					for (auto s : boss->getAllRecursive<lif::Sighted>())
						s->setEntityGroup(&lm.entities);
					entities.add(boss);
				}
				break;

			case EntityType::ENEMY1:
				enemy_id = 1;
				break;

			case EntityType::ENEMY2:
				enemy_id = 2;
				break;

			case EntityType::ENEMY3:
				enemy_id = 3;
				break;

			case EntityType::ENEMY4:
				enemy_id = 4;
				break;

			case EntityType::ENEMY5:
				enemy_id = 5;
				break;

			case EntityType::ENEMY6:
				enemy_id = 6;
				break;

			case EntityType::ENEMY7:
				enemy_id = 7;
				break;

			case EntityType::ENEMY8:
				enemy_id = 8;
				break;

			case EntityType::ENEMY9:
				enemy_id = 9;
				break;

			case EntityType::ENEMY10:
				enemy_id = 10;
				break;

			case EntityType::EMPTY:
				break;

			default:
				std::cerr << "Invalid tile at (" << left << ", " << top << "): "
					<< level.getTile(left, top) << std::endl;
				break;
			}

			if (enemy_id > 0) {
				lif::Enemy *enemy = nullptr;
				const auto& info = ls.getEnemyInfo(enemy_id);
				// Some enemies have their own classes, others are just 'Enemy'
				switch (enemy_id) {
				case 2:
					enemy = new lif::Wisp(curPos, info);
					break;
				case 10:
					enemy = new lif::AlienPredator(curPos, info);
					break;
				default:
					enemy = new lif::Enemy(curPos, enemy_id, info);
					break;
				}
				enemy->get<lif::AI>()->setLevelManager(&lm);
				auto sighted = enemy->get<lif::Sighted>();
				sighted->setEntityGroup(&lm.entities);
				sighted->setOpaque({ lif::c_layers::BREAKABLES, lif::c_layers::UNBREAKABLES });
				entities.add(enemy);
			}
		}
	}

	for (auto e : LevelEffects::getEffectEntities(level))
		entities.add(e);

	if (level.getInfo().effects.find("darkness") != level.getInfo().effects.end()) {
		// In case of darkness, we need the Players to have an AxisSighted component
		for (unsigned short i = 0; i < lm.players.size(); ++i) {
			if (lm.players[i] == nullptr) continue;
			auto sighted = lm.players[i]->addComponent<lif::AxisSighted>(*lm.players[i]);
			sighted->init();
			sighted->setEntityGroup(&lm.entities);
			sighted->setOpaque({
				lif::c_layers::BREAKABLES,
				lif::c_layers::UNBREAKABLES
			});
		}
	}

	lm.levelTime->resume();

	return true;
}
