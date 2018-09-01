#include "LevelLoader.hpp"
#include "AI.hpp"
#include "AcidPond.hpp"
#include "AlienBoss.hpp"
#include "Animated.hpp"
#include "AxisSighted.hpp"
#include "BreakableWall.hpp"
#include "Cage.hpp"
#include "Coin.hpp"
#include "DroppingBomb.hpp"
#include "EnemyFactory.hpp"
#include "FixedWall.hpp"
#include "Flare.hpp"
#include "Flash.hpp"
#include "GodEyeBoss.hpp"
#include "HauntedStatue.hpp"
#include "HauntingSpiritBoss.hpp"
#include "LeapingMovement.hpp"
#include "Level.hpp"
#include "LevelEffects.hpp"
#include "LevelManager.hpp"
#include "LevelSet.hpp"
#include "Lifed.hpp"
#include "MainframeBoss.hpp"
#include "Player.hpp"
#include "RexBoss.hpp"
#include "Spikes.hpp"
#include "Sprite.hpp"
#include "Teleport.hpp"
#include "TimedLaser.hpp"
#include "Torch.hpp"
#include "TransparentWall.hpp"
#include "game.hpp"
#include <iostream>

using lif::TILE_SIZE;
using lif::LevelLoader;
using lif::EntityType;

template<typename T, typename... Args>
static T* addBoss(lif::EntityGroup& entities, Args&&... args) {
	auto boss = new T(std::forward<Args>(args)...);
	for (auto s : boss->template getAllRecursive<lif::Sighted>())
		s->setEntityGroup(&entities);
	entities.add(boss);
	return boss;
}

bool LevelLoader::load(const lif::Level& level, lif::LevelManager& lm) {

	lm.levelTime->setTime(sf::seconds(level.getInfo().time));

	lif::Teleport *firstTeleport = nullptr,
	              *latestTeleport = nullptr;

	lm.reset();
	auto& entities = lm.getEntities();

	const auto lvinfo = level.getInfo();

	for (int top = 0; top < lvinfo.height; ++top) {
		for (int left = 0; left < lvinfo.width; ++left) {

			const sf::Vector2f curPos((left + 1) * TILE_SIZE, (top + 1) * TILE_SIZE);
			int enemy_id = 0;

			const auto is_game_over = [&lm] (auto id) -> bool {
				return lm.players[id] == nullptr || (
						lm.players[id]->getInfo().remainingLives <= 0
						&& lm.players[id]->template get<lif::Lifed>()->getLife() <= 0
						&& lm.getPlayerContinues(id + 1) <= 0
					);
			};

			auto add_player = [&lm, &entities, is_game_over] (int id, const sf::Vector2f& pos) {
				if (!is_game_over(id)) {
					lm.players[id]->setWinning(false);
					lm.players[id]->setPosition(pos);
					lm.players[id]->get<lif::Animated>()->setAnimation("idle_down");
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

			case EntityType::SPIKES:
				entities.add(new lif::Spikes(curPos));
				break;

			case EntityType::TORCH:
				{
					auto torch = new lif::Torch(curPos);
					torch->fixOrientation(level);
					entities.add(torch);
					break;
				}

			case EntityType::TELEPORT:
				{
					auto teleport = new lif::Teleport(curPos);

					// Save the first Teleport added
					if (firstTeleport == nullptr)
						firstTeleport = teleport;
					else
						teleport->linkTo(firstTeleport);

					// If we had already added a Teleport, link it to this one.
					if (latestTeleport != nullptr)
						latestTeleport->linkTo(teleport);
					latestTeleport = teleport;

					entities.add(teleport);
					break;
				}

			case EntityType::ALIEN_BOSS:
				addBoss<lif::AlienBoss>(entities, curPos);
				break;

			case EntityType::HAUNTING_SPIRIT_BOSS:
				addBoss<lif::HauntingSpiritBoss>(entities, curPos);
				break;

			case EntityType::REX_BOSS:
				addBoss<lif::RexBoss>(entities, curPos)->get<lif::AI>()->setLevelManager(&lm);
				break;

			case EntityType::GOD_EYE_BOSS:
				addBoss<lif::GodEyeBoss>(entities, curPos, lm);
				break;

			case EntityType::MAINFRAME_BOSS:
				addBoss<lif::MainframeBoss>(entities, curPos, lm);
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

			if (enemy_id > 0)
				entities.add(lif::EnemyFactory::create(lm, enemy_id, curPos).release());
		}
	}

	for (auto e : LevelEffects::getEffectEntities(level))
		entities.add(e);

	if (level.hasEffect("darkness")) {
		// In case of darkness, we need the Players to have an AxisSighted component
		for (size_t i = 0; i < lm.players.size(); ++i) {
			if (lm.players[i] == nullptr || lm.players[i]->get<lif::AxisSighted>() != nullptr)
				continue;
			auto sighted = lm.players[i]->addComponent<lif::AxisSighted>(*lm.players[i]);
			sighted->init();
			sighted->setEntityGroup(&lm.entities);
			sighted->setOpaque({
				lif::c_layers::BREAKABLES,
				lif::c_layers::UNBREAKABLES
			});
		}
		entities.add(new lif::Flare(sf::seconds(0.07f), sf::seconds(0.7f)));
	}

	lm.levelTime->resume();

	return true;
}
