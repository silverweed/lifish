#include "LevelLoader.hpp"
#include "game.hpp"
#include "LevelSet.hpp"
#include "FixedWall.hpp"
#include "Teleport.hpp"
#include "BreakableWall.hpp"
#include "AI.hpp"
#include "TransparentWall.hpp"
#include "Coin.hpp"
#include "Enemy.hpp"
#include "Lifed.hpp"
#include <iostream>

using Game::TILE_SIZE;

bool Game::LevelLoader::load(const Game::Level& level, Game::LevelManager& lm) {

	lm.level = &level;
	lm.levelTime.setTime(sf::seconds(level.getInfo().time));

	Game::Teleport *first_teleport = nullptr,
		       *latest_teleport = nullptr;

	auto& entities = lm.getEntities();
	// Destroy everything
	entities.clear();

	for (unsigned short top = 0; top < Game::LEVEL_HEIGHT; ++top) {
		for (unsigned short left = 0; left < Game::LEVEL_WIDTH; ++left) {

			const sf::Vector2f curPos((left+1) * TILE_SIZE, (top+1) * TILE_SIZE);
			unsigned short enemy_id = 0;
			const auto& ls = level.getLevelSet();

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
			case EntityType::TELEPORT:
				{
					auto teleport = new Game::Teleport(curPos);

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
					break;
				}
			/*
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
			case EntityType::EMPTY:
				break;
			default:
				std::cerr << "Invalid tile at (" << left << ", " << top << "): "
					<< level.getTile(left, top) << std::endl;
				break;
			}
			if (enemy_id > 0) {
				auto enemy = new Game::Enemy(curPos, enemy_id, ls.getEnemyInfo(enemy_id));
				enemy->get<Game::AI>()->setLevelManager(&lm);
				auto sighted = enemy->get<Game::Sighted>();
				sighted->setLevelManager(&lm);
				// FIXME: can we avoid to hardcode the opaque layers here?
				sighted->setOpaque({ Game::Layers::WALLS, Game::Layers::TRANSP_WALLS });
				entities.add(enemy);
			}
		}
	}

	lm.levelTime.resume();

	return true;
}
