#include "LevelLoader.hpp"
#include "Game.hpp"

using Game::TILE_SIZE;

bool Game::LevelLoader::load(const Game::Level& level, Game::EntityGroup& entities) {
	for (unsigned short top = 0; top < Game::LEVEL_HEIGHT; ++top) {
		for (unsigned short left = 0; left < Game::LEVEL_WIDTH; ++left) {

			const sf::Vector2f curPos = sf::Vector2f((left+1) * TILE_SIZE, (top+1) * TILE_SIZE);
			unsigned short enemy_id = 0;
			
			struct {
				unsigned short type;
				float fireRate;
				unsigned short id;
				unsigned short damage;
				unsigned short blockTime = 0;
				float speed;
				short range = -1;
			} enemy_attack;	

			/*auto is_game_over = [] (unsigned short id) {
				return _players[id] == nullptr || (_players[id]->getRemainingLives() <= 0
						&& _players[id]->getLife() <= 0 && Game::playerContinues[id] <= 0);
			};*/

			switch (level->getTile(left, top)) {

				using AT = Game::Enemy::AttackType;

			case EntityType::FIXED: 
				//fixedEntities[top * LEVEL_WIDTH + left] = 
				entities.add(new Game::FixedWall(curPos, level->getInfo().tileIDs.fixed));
				break;

			case EntityType::BREAKABLE:
				//fixedEntities[top * LEVEL_WIDTH + left] = 
				entities.add(new Game::BreakableWall(curPos, level->getInfo().tileIDs.breakable));
				break;

			case EntityType::TRANSPARENT_WALL:
				//fixedEntities[top * LEVEL_WIDTH + left] =
				entities.add(new Game::TransparentWall(curPos));
				break;

			/*case EntityType::COIN:
				entities.add(new Game::Coin(curPos));
				//fixedEntities[top * LEVEL_WIDTH + left] = new Game::Coin(curPos);
				++coinsNum;
				break;

			case EntityType::PLAYER1: 
				if (!is_game_over(0)) {
					_players[0]->setPosition(curPos);
					players[0] = _players[0].get();
					movingEntities.push_back(players[0]);
				}
				break;
			case EntityType::PLAYER2: 
				if (!is_game_over(1)) {
					_players[1]->setPosition(curPos);
					players[1] = _players[1].get();
					movingEntities.push_back(players[1]);
				}
				break;
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
			case EntityType::ENEMY1: 
				// TODO: make enemy_attack configurable from levels.json
				enemy_id = 1;
				enemy_attack.id = 1;
				enemy_attack.type = AT::SIMPLE;
				enemy_attack.damage = 1;
				enemy_attack.speed = 1;
				enemy_attack.fireRate = 1;
				break;
			case EntityType::ENEMY2: 
				enemy_id = 2;
				enemy_attack.id = 1;
				enemy_attack.type = AT::SIMPLE;
				enemy_attack.damage = 1;
				enemy_attack.speed = 1;
				enemy_attack.fireRate = 2;
				break;
			case EntityType::ENEMY3: 
				enemy_id = 3;
				enemy_attack.id = 2;
				enemy_attack.type = AT::SIMPLE | AT::BLOCKING;
				enemy_attack.damage = 2;
				enemy_attack.speed = 0.66;
				enemy_attack.fireRate = 1;
				enemy_attack.blockTime = 200;
				break;
			case EntityType::ENEMY4:
				enemy_id = 4;
				enemy_attack.type = AT::CONTACT;
				enemy_attack.damage = 2;
				enemy_attack.fireRate = 1;
				break;
			case EntityType::ENEMY5: 
				enemy_id = 5;
				enemy_attack.id = 3;
				enemy_attack.type = AT::BLOCKING;
				enemy_attack.damage = 2;
				enemy_attack.speed = 1;
				enemy_attack.fireRate = 10;
				break;
			case EntityType::ENEMY6: 
				enemy_id = 6;
				enemy_attack.id = 4;
				enemy_attack.type = AT::SIMPLE | AT::BLOCKING;
				enemy_attack.damage = 3;
				enemy_attack.fireRate = 1;
				enemy_attack.speed = 0.66;
				enemy_attack.blockTime = 250;
				break;
			case EntityType::ENEMY7: 
				enemy_id = 7;
				enemy_attack.type = AT::CONTACT | AT::RANGED;
				enemy_attack.damage = 1;
				enemy_attack.fireRate = 2.5;
				break;
			case EntityType::ENEMY8: 
				enemy_id = 8;
				enemy_attack.id = 5;
				enemy_attack.type = AT::RANGED | AT::BLOCKING;
				enemy_attack.damage = 3;
				enemy_attack.range = 4;
				enemy_attack.speed = 0.5;
				enemy_attack.fireRate = 4;
				break;
			case EntityType::ENEMY9: 
				enemy_id = 9;
				enemy_attack.id = 6;
				enemy_attack.type = AT::BLOCKING;
				enemy_attack.damage = 3;
				enemy_attack.speed = 1;
				enemy_attack.fireRate = 10;
				break;
			case EntityType::ENEMY10: 
				enemy_id = 10;
				enemy_attack.id = 7;
				enemy_attack.type = AT::SIMPLE | AT::BLOCKING;
				enemy_attack.damage = 4;
				enemy_attack.fireRate = 0.7;
				enemy_attack.blockTime = 650;
				enemy_attack.speed = 1;
				break;*/
			default:
				break;
			}
		/*	if (enemy_id > 0) {
				auto enemy = new Game::Enemy(curPos, enemy_id);
				enemy->setAI(Game::ai_functions[level->getLevelSet()->getEnemyInfo(enemy_id).ai]);
				enemy->setSpeed(Game::Enemy::BASE_SPEED * level->getLevelSet()->getEnemyInfo(enemy_id).speed);
				enemy->attack.id = enemy_attack.id;
				enemy->attack.type = enemy_attack.type;
				enemy->attack.damage = enemy_attack.damage;
				enemy->attack.speed = enemy_attack.speed;
				enemy->attack.fireRate = enemy_attack.fireRate;
				enemy->attack.blockTime = enemy_attack.blockTime;
				enemy->attack.range = enemy_attack.range;
				movingEntities.push_back(enemy);
			}*/
		}
	}
}
