#include "LevelLoader.hpp"
#include "Game.hpp"
#include "LevelSet.hpp"
#include "FixedWall.hpp"
#include "BreakableWall.hpp"
#include "TransparentWall.hpp"
#include "Coin.hpp"
#include "Enemy.hpp"
#include "Lifed.hpp"

using Game::TILE_SIZE;

bool Game::LevelLoader::load(const Game::Level& level, Game::LevelManager& lm) {

	lm.level = &level;
	lm.levelTime->setTime(level.getInfo().time);

	auto& entities = lm.getEntities();
	if (entities.size() > 0) {
		// Release the players so they're not destroyed with other entities.
		for (Game::Player *p : lm.players)
			if (p != nullptr)
				entities.release(p);

		// Destroy everything else
		entities.clear();
	}

	for (unsigned short top = 0; top < Game::LEVEL_HEIGHT; ++top) {
		for (unsigned short left = 0; left < Game::LEVEL_WIDTH; ++left) {

			const sf::Vector2f curPos = sf::Vector2f((left+1) * TILE_SIZE, (top+1) * TILE_SIZE);
			unsigned short enemy_id = 0;
			const Game::LevelSet *ls = level.getLevelSet();

			auto is_game_over = [&lm] (unsigned short id) -> bool {
				return lm.players[id] == nullptr || (
						lm.players[id]->getInfo().remainingLives <= 0
						&& lm.players[id]->get<Game::Lifed>()->getLife() <= 0
						&& Game::playerContinues[id] <= 0
					);
			};

			switch (level.getTile(left, top)) {
			case EntityType::FIXED: 
				//fixedEntities[top * LEVEL_WIDTH + left] = 
				entities.add(new Game::FixedWall(curPos, level.getInfo().tileIDs.fixed));
				break;

			case EntityType::BREAKABLE:
				//fixedEntities[top * LEVEL_WIDTH + left] = 
				entities.add(new Game::BreakableWall(curPos, level.getInfo().tileIDs.breakable));
				break;

			case EntityType::TRANSPARENT_WALL:
				//fixedEntities[top * LEVEL_WIDTH + left] =
				entities.add(new Game::TransparentWall(curPos));
				break;

			case EntityType::COIN:
				entities.add(new Game::Coin(curPos));
				//fixedEntities[top * LEVEL_WIDTH + left] = new Game::Coin(curPos);
				break;

			case EntityType::PLAYER1: 
				if (!is_game_over(0)) {
					lm.players[0]->setPosition(curPos);
					entities.add(lm.players[0]);
				}
				break;
			case EntityType::PLAYER2: 
				if (!is_game_over(1)) {
					lm.players[1]->setPosition(curPos);
					entities.add(lm.players[1]);
				}
				break;
			/*
			case EntityType::TELEPORT:
				{
					auto teleport = new Game::Teleport(curPos);
					// Save the first Teleport added
					if (firstTeleport == nullptr)
						firstTeleport = teleport;
					else
						teleport->linkTo(firstTeleport);

					// If we had already added a Teleport, link it to this one.
					if (latest_teleport != nullptr)
						latest_teleport->linkTo(teleport);
					latest_teleport = teleport;

					fixedEntities[top * LEVEL_WIDTH + left] = teleport;
					break;
				}
			case EntityType::BOSS:
				if (_isFinalLevel()) {
					if (finalBoss == nullptr) {
						finalBoss = std::unique_ptr<Game::FinalBoss>(new Game::FinalBoss(curPos));
					} else {
						std::cerr << "[ WARNING ] Duplicate Final Boss! Not adding more..." << std::endl;
					}
				} else {
					bosses.push_back(new Game::Boss(curPos));
				}
				break;
			*/
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
			default:
				break;
			}
			if (enemy_id > 0) {
				auto info = ls->getEnemyInfo(enemy_id - 1);
				entities.add(new Game::Enemy(curPos, enemy_id, info.speed, info.attack));
				// TODO AI
			}
		}
	}

	return true;
}
