#include "LevelRenderer.hpp"
#include "EntityType.hpp"
#include "FixedWall.hpp"
#include "BreakableWall.hpp"
#include "Coin.hpp"
#include "Flash.hpp"
#include "BossExplosion.hpp"
#include "BossBullet.hpp"
#include "Bonus.hpp"
#include "Enemy.hpp"
#include "Points.hpp"
#include "utils.hpp"
#include <sstream>
#include <limits>

using Game::LevelRenderer;

LevelRenderer::LevelRenderer() {
	fixedEntities.fill(nullptr);
	players.fill(nullptr);
	for (unsigned short i = 0; i < bombs.size(); ++i)
		bombs[i].fill(nullptr);

	movingEntities.reserve(LEVEL_WIDTH * LEVEL_HEIGHT);
	temporary.reserve(LEVEL_WIDTH * LEVEL_HEIGHT);
	bosses.reserve(32);
	bullets.reserve(1024);
}

LevelRenderer::~LevelRenderer() {
	_clearEntities();
}

void LevelRenderer::_clearEntities() {
	for (auto& e : fixedEntities)
		if (e != nullptr) delete e;
	for (auto& e : movingEntities)
		delete e;
	for (auto& b : bosses)
		delete b;
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i)
		for (unsigned short j = 0; j < bombs[i].size(); ++j)
			if (bombs[i][j] != nullptr) {
				delete bombs[i][j];
				bombs[i][j] = nullptr;
			}
	for (auto it = temporary.begin(); it != temporary.end(); ++it) 
		delete *it;
	for (auto it = bullets.begin(); it != bullets.end(); ++it)
		delete *it;
	for (auto it = letters.begin(); it != letters.end(); ++it)
		delete *it;

	if (hurryUpText != nullptr) {
		delete hurryUpText;
		hurryUpText = nullptr;
	}

	if (gameOverText != nullptr) {
		delete gameOverText;
		gameOverText = nullptr;
	}

	if (extraGameText != nullptr) {
		delete extraGameText;
		extraGameText = nullptr;
	}

	players.fill(nullptr);
	fixedEntities.fill(nullptr);

	firstTeleport = nullptr;

	temporary.clear();
	movingEntities.clear();
	bullets.clear();
	bosses.clear();
	letters.clear();
}

void LevelRenderer::loadLevel(Game::Level *const _level) {
	if (level != nullptr) {
		_clearEntities();
	}
	level = _level;
	bossShootClock.restart();
	hurryUp = hurryUpWarningGiven = false;
	levelTimeClock.restart();
	coinsNum = 0;

	Game::Teleport *latest_teleport = nullptr;

	for (unsigned short top = 0; top < LEVEL_HEIGHT; ++top) {
		for (unsigned short left = 0; left < LEVEL_WIDTH; ++left) {
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

			switch (level->getTile(left, top)) {
				using AT = Game::Enemy::AttackType;
			case EntityType::FIXED: 
				fixedEntities[top * LEVEL_WIDTH + left] = 
					new Game::FixedWall(curPos, level->tileIDs.fixed);
				break;
			case EntityType::BREAKABLE:
				fixedEntities[top * LEVEL_WIDTH + left] = 
					new Game::BreakableWall(curPos, level->tileIDs.breakable);
				break;
			case EntityType::COIN:
				fixedEntities[top * LEVEL_WIDTH + left] = new Game::Coin(curPos);
				++coinsNum;
				break;
			case EntityType::PLAYER1: 
				{
					auto player = new Game::Player(curPos, 1);
					movingEntities.push_back(player);
					players[0] = player;
					break;
				}
			case EntityType::PLAYER2: 
				{
					auto player = new Game::Player(curPos, 2);
					movingEntities.push_back(player);
					players[1] = player;
					break;
				}
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
				bosses.push_back(new Game::Boss(curPos));
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
				enemy_attack.type = AT::SIMPLE;
				enemy_attack.damage = 2;
				enemy_attack.speed = 0.5;
				enemy_attack.fireRate = 1;
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
				enemy_attack.type = AT::SIMPLE;
				enemy_attack.damage = 3;
				enemy_attack.fireRate = 1;
				enemy_attack.speed = 0.6;
				break;
			case EntityType::ENEMY7: 
				enemy_id = 7;
				enemy_attack.type = AT::CONTACT;
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
				break;
			default:
				  break;
			}
			if (enemy_id > 0) {
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
			}
		}
	}
	
	letters.reserve(movingEntities.size() - 1);

	level->setOrigin(origin);
	for (auto& entity : fixedEntities) 
		if (entity != nullptr)
			entity->setOrigin(origin);
	for (auto& entity : movingEntities)
		entity->setOrigin(origin);
	for (auto& boss : bosses)
		boss->setOrigin(origin);
}

void LevelRenderer::renderFrame(sf::RenderWindow& window) {
	if (level == nullptr) return;

	level->draw(window);
	// Draw fixed entities
	for (unsigned short left = 0; left < LEVEL_WIDTH; ++left) {
		for (unsigned short top = 0; top < LEVEL_HEIGHT; ++top) {
			const unsigned short idx = top * LEVEL_WIDTH + left;
			const auto entity = fixedEntities[idx];
			if (entity == nullptr) continue;

			const auto e_type = level->getTile(left, top);
			switch (e_type) {
			case EntityType::BREAKABLE:
				if (!entity->transparentTo.enemies) {
					// A breakable wall
					auto bw = static_cast<Game::BreakableWall*>(entity);
					if (bw->isDestroyed()) {
						// Chance to drop bonus
						const unsigned short bonus_type = Game::bonusTypeDistribution(rng);
						if (bonus_type >= Game::Bonus::N_BONUS_TYPES) {
							// no bonus
							fixedEntities[idx] = nullptr;
						} else {
							dropBonus(Game::tile(entity->getPosition()), bonus_type);
						}
						delete bw;
					} else {
						bw->draw(window);
					}
				} else {
					// A bonus
					auto bonus = static_cast<Game::Bonus*>(entity);
					if (bonus->isExpired()) {
						delete bonus;
						fixedEntities[idx] = nullptr;
					} else {
						bonus->draw(window);
					}
				}
				break;
			case EntityType::COIN:
				{
					auto coin = static_cast<Game::Coin*>(entity);
					if (coin->isGrabbed()) {
						delete coin;
						fixedEntities[idx] = nullptr;
					} else {
						coin->draw(window);
					}
					break;
				}
			default:
				entity->draw(window);
			}
		}
	}

	// Draw letters
	for (auto& letter : letters) {
		letter->draw(window);
	}

	// Draw bombs
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
		for (unsigned short j = 0; j < bombs[i].size(); ++j) {
			auto bomb = bombs[i][j];
			if (bomb == nullptr) continue;
			if (bomb->isExploded()) {
				delete bomb;
				bombs[i][j] = nullptr;
			} else {
				bomb->draw(window);
			}
		}
	}
	
	// Draw moving entities
	for (auto it = movingEntities.begin(); it != movingEntities.end(); ) {
		auto entity = *it;
		if (entity->isDying() && !isPlayer(entity)) {
			entity->prepareDeathAnimation();
			if (entity->playDeathAnimation()) {
				if (extraGame)
					_spawnLetter(entity->getPosition());
				delete entity;
				it = movingEntities.erase(it);
				continue;
			}
		}
		entity->draw(window);
		++it;
	}

	// Draw bomb explosions
	for (auto it = explosions.begin(); it != explosions.end(); ) {
		if (!(*it)->isPlaying()) {
			delete *it;
			it = explosions.erase(it);
		} else {
			(*it)->draw(window);
			++it;
		}
	}

	// Draw bosses
	for (auto it = bosses.begin(); it != bosses.end(); ) {
		auto boss = *it;
		if (boss->isDead()) {
			for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
				if (players[i] == nullptr || players[i]->isDying())
					continue;
				Game::score[i] += boss->getPointsGiven();
			}

			_spawnPoints(boss->getPosition(), boss->getPointsGiven(), true);
			delete boss;
			it = bosses.erase(it);

			continue;
		} else if (boss->isDying()) {
			if (boss->explClock.getElapsedTime().asMilliseconds() >= 100) {
				boss->explClock.restart();
				// Calculate a random location inside the boss
				const auto bpos = boss->getPosition();
				std::uniform_real_distribution<float> dist(-0.5 * TILE_SIZE, TILE_SIZE * (Game::Boss::SIZE - 0.5));
				const float x = dist(rng),
				            y = dist(rng);
				_pushTemporary(new Game::BossExplosion(sf::Vector2f(bpos.x + x, bpos.y + y)));
			}
		}
		boss->draw(window);
		++it;
	}
	
	// Draw other temporaries
	for (auto it = temporary.begin(); it != temporary.end(); ) {
		if (!(*it)->isPlaying()) {
			delete *it;
			it = temporary.erase(it);
		} else {
			(*it)->draw(window);
			++it;
		}
	}

	// Draw bullets
	for (auto it = bullets.begin(); it != bullets.end(); ) {
		auto bullet = *it;
		if (bullet->isDestroyed()) {
			delete bullet;
			it = bullets.erase(it);
		} else {
			bullet->draw(window);
			++it;
		}
	}

	// Draw dropping texts

	if (hurryUpText != nullptr) {
		if (hurryUpText->isPlaying())
			hurryUpText->draw(window);
		else {
			delete hurryUpText;
			hurryUpText = nullptr;
		}
	}

	if (gameOverText != nullptr) {
		if (gameOverText->isPlaying()) 
			gameOverText->draw(window);
		else {
			delete gameOverText;
			gameOverText = nullptr;
			gameOverEnded = true;
		}
	}

	if (extraGameText != nullptr) {
		if (extraGameText->isPlaying())
			extraGameText->draw(window);
		else {
			delete extraGameText;
			extraGameText = nullptr;
		}
	}
}

void LevelRenderer::detectCollisions() {
	/* For each moving entity, check (towards its direction):
	 * 1) has it reached the level boundaries?
	 * 2) is there another non-trasparent entity occupying the cell ahead?
	 */
	std::vector<bool> checked(movingEntities.size(), false);
	
	auto collide = [] (const sf::Vector2f& pos, const sf::Vector2f& opos, const Game::Direction dir) {
		const unsigned short iposx = (unsigned short)pos.x,
			             iposy = (unsigned short)pos.y,
			             ioposx = (unsigned short)opos.x,
			             ioposy = (unsigned short)opos.y;
		switch (dir) {
		case Direction::UP:
			return iposx - TILE_SIZE < ioposx && ioposx < iposx + TILE_SIZE
				&& iposy >= ioposy && ioposy >= iposy - TILE_SIZE;
		case Direction::LEFT:
			return iposx >= ioposx && ioposx >= iposx - TILE_SIZE
				&& iposy - TILE_SIZE < ioposy && ioposy < iposy + TILE_SIZE;
		case Direction::DOWN:
			return iposx - TILE_SIZE < ioposx && ioposx < iposx + TILE_SIZE
				&& ioposy >= iposy && ioposy <= iposy + TILE_SIZE;
		case Direction::RIGHT:
			return ioposx >= iposx && ioposx <= iposx + TILE_SIZE
				&& iposy - TILE_SIZE < ioposy && ioposy < iposy + TILE_SIZE;
		default:
			return false;
		}
	};

	for (unsigned short i = 0, len = movingEntities.size(); i < len; ++i) {
		if (checked[i]) continue;
		MovingEntity *entity = movingEntities[i];
		if (entity->isDying()) continue;
		sf::Vector2f pos = entity->getPosition();
		const bool is_player = isPlayer(entity);

		// Check for teleports
		if (firstTeleport != nullptr && entity->canTeleport && entity->isAligned()) {
			const auto cur_tile = Game::tile(pos);

			if (level->getTile(cur_tile.x - 1, cur_tile.y - 1) == EntityType::TELEPORT && entity->prevAlign != cur_tile) {
				const unsigned short idx = (cur_tile.y - 1) * LEVEL_WIDTH + cur_tile.x - 1;

				// Get Teleport from fixed entities
				auto teleport = static_cast<Game::Teleport*>(fixedEntities[idx]);

				if (teleport != nullptr && !teleport->isDisabled()) {
					// Get destination Teleport
					Game::Teleport *next = teleport->next();
					if (next == nullptr) continue;
					if (next->isDisabled() || isEntityTouching(next->getPosition())) {
						Game::Teleport *self = next;
						bool viable = false;
						do {
							next = next->next();
							if (next == teleport || next->isDisabled() || isEntityTouching(next->getPosition()))
								next = next->next();
							else {
								viable = true;
								break;
							}
						} while (next != self);
						// Check if we've found an enabled destination
						if (!viable) continue;
					}

					_pushTemporary(new Game::Flash(teleport->getPosition()));
					_pushTemporary(new Game::Flash(next->getPosition()));

					// Teleport the entity
					entity->setPosition(next->getPosition());
					entity->prevAlign = Game::tile(next->getPosition());

					// Disable both source and destination for a while
					teleport->disable();
					next->disable();
				}
			}
		}
		
		// Check for letters
		if (extraGame && is_player) {
			for (auto it = letters.begin(); it != letters.end(); ) {
				auto letter = *it;
				sf::FloatRect letter_box(letter->getPosition().x, letter->getPosition().y,
						TILE_SIZE, TILE_SIZE);
				sf::FloatRect player_box(pos.x, pos.y, TILE_SIZE, TILE_SIZE);
				if (player_box.intersects(letter_box)) {
					// Grab the letter
					auto player = static_cast<Game::Player*>(entity);
					player->extra[letter->getId()] = true;
					_spawnPoints(letter->getPosition(), letter->getPointsGiven());

					// Check if EXTRA
					bool extra = true;
					for (unsigned short i = 0; i < player->extra.size(); ++i) {
						if (!player->extra[i]) {
							extra = false;
							break;
						}
					}
					if (extra) {
						player->extra.fill(false);
						player->setRemainingLives(player->getRemainingLives() + 1);
						const auto upText = new Game::Points(letter->getPosition() + sf::Vector2f((TILE_SIZE - 45) / 2., 0.f),
								std::to_string(_getPlayerIndex(player) + 1) + "UP", sf::Color(77, 184, 255, 255), 15);
						_pushTemporary(upText);
					}

					delete letter;
					it = letters.erase(it);
				} else {
					++it;
				}
			}
		}

		// Check impact with bullets
		if (!entity->transparentTo.bullets) {
			for (auto it = bullets.begin(); it != bullets.end(); ++it) {
				auto bullet = *it;
				if (entity != bullet->getSource() && bullet->hits(entity->getPosition())) {
					if (is_player && !(entity->hasShield() || entity->isDying())) {
						// Damage player
						auto player = static_cast<Game::Player*>(entity);
						player->decLife(bullet->getDamage());
						spawnDamage(player->getPosition(), bullet->getDamage());
						if (!player->isHurt()) {
							player->setHurt(true);
							player->giveShield(Game::DAMAGE_SHIELD_TIME);
						}
						if (player->getLife() <= 0)
							player->kill();
					} 
					bullet->destroy();	
				}
			}
		}

		entity->colliding = false;

		pos = entity->getPosition();
		const Direction dir = entity->getDirection();
		const unsigned short iposx = (unsigned short)pos.x,
		                     iposy = (unsigned short)pos.y;
		sf::Vector2i next_tile(iposx / TILE_SIZE - 1, iposy / TILE_SIZE - 1);
		bool at_limit = false;

		// Check for impacts with the borders
		switch (dir) {
		case Direction::UP:
			if (pos.y <= TILE_SIZE) {
				// Reached the top
				entity->colliding = true;
				continue;
			}
			at_limit = iposy % TILE_SIZE == 0;
			--next_tile.y;
			break;
		case Direction::LEFT:
			if (pos.x <= TILE_SIZE) {
				entity->colliding = true;
				continue;
			}
			at_limit = iposx % TILE_SIZE == 0;
			--next_tile.x;
			break;
		case Direction::DOWN:
			if (pos.y >= TILE_SIZE * LEVEL_HEIGHT) {
				entity->colliding = true;
				continue;
			}
			at_limit = iposy % TILE_SIZE == 0;
			++next_tile.y;
			break;
		case Direction::RIGHT:
			if (pos.x >= TILE_SIZE * LEVEL_WIDTH) {
				entity->colliding = true;
				continue;
			}
			at_limit = iposx % TILE_SIZE == 0;
			++next_tile.x;
			break;
		case Direction::NONE:
			break;
		}

		// Check for impacts with moving entities
		bool collision_detected = false;
		for (unsigned short j = 0; j < len; ++j) {
			if (i == j) continue;

			MovingEntity *other = movingEntities[j];
			sf::Vector2f opos = other->getPosition();

			if (collide(pos, opos, dir)) {
				checked[i] = true;

				bool opaque;
				Game::Player *player = nullptr;
				Game::Enemy *enemy = nullptr;
				if (!is_player) {
					// If opaque == false, other entity is a player.
					opaque = !other->transparentTo.enemies;
					if (opaque) {
						collision_detected = true;
						entity->colliding = true;
					} else {
						player = static_cast<Game::Player*>(other);
						enemy = static_cast<Game::Enemy*>(entity);
					}
				} else {
					// If opaque == false, other entity is an enemy.
					opaque = !other->transparentTo.players;
					if (opaque) {
						collision_detected = true;
						entity->colliding = true;
					} else {
						player = static_cast<Game::Player*>(entity);
						enemy = static_cast<Game::Enemy*>(other);
					}
				}

				if (!opaque && !(enemy->isDying() || player->isDying())) {
					if (!enemy->isMorphed() && (enemy->attack.type & Enemy::AttackType::CONTACT)) {
						enemy->shoot();
						enemy->attackAlign = Game::tile(player->getPosition());
					}

					if (!player->hasShield()) {
						if (enemy->attack.type & Enemy::AttackType::CONTACT) {
							player->decLife(enemy->attack.damage);
							spawnDamage(player->getPosition(), enemy->attack.damage);
						} else {
							player->decLife(1);
							spawnDamage(player->getPosition(), 1);
						}
						if (!player->isHurt()) {
							player->setHurt(true);
							player->giveShield(Game::DAMAGE_SHIELD_TIME);
						}
						if (player->getLife() <= 0)
							player->kill();
					}
				}

				if (other->getDirection() == Game::oppositeDirection(dir)) {
					if (opaque)
						other->colliding = true;
					checked[j] = true;
				}

				break;
			}
		}

		// Check for impacts with fixed entities
		if (!collision_detected && at_limit) {
			const unsigned short idx = next_tile.y * LEVEL_WIDTH + next_tile.x;
			if (idx >= fixedEntities.size()) {
				// Should never happen: means we're outside the grid
				entity->colliding = true;
				continue;
			}
			FixedEntity *other = fixedEntities[idx];
			if (other != nullptr) {
				if ((is_player && !other->transparentTo.players) 
						|| (!is_player && !other->transparentTo.enemies))
				{
					// Colliding with a wall
					entity->colliding = true;
				} else {
					// Either a coin, a bonus or a teleport. We can know for sure
					// what this entity is with a single lookup to level->tiles:
					// if we find a teleport or a coin, it means it's that entity;
					// else, if we find a breakable, it's a bonus.
					switch (level->getTile(next_tile.x, next_tile.y)) {
					case EntityType::COIN:
						{
							// Grab the coin
							if (!is_player) break;
							auto coin = static_cast<Game::Coin*>(other);
							if (!coin->isBeingGrabbed()) {
								coin->grab();
								Game::score[_getPlayerIndex(entity)] += coin->getPointsGiven();
								_spawnPoints(coin->getPosition(), coin->getPointsGiven());
								if (--coinsNum == 0) {
									_triggerExtraGame();
								}
							}
							break;
						}
					case EntityType::BREAKABLE:
						{
							// Grab the bonus
							if (!is_player) break;
							auto bonus = static_cast<Game::Bonus*>(other);
							const unsigned short i = _getPlayerIndex(entity);
							switch (bonus->getType()) {
								using B = Game::Bonus::Type;
							case B::ZAPPER:
								_destroyAllWalls();
								break;
							case B::SUDDEN_DEATH:
								_killAllEnemies();
								break;
							case B::MAX_BOMBS:
								if (players[i]->powers.maxBombs < Game::Player::MAX_MAX_BOMBS)
									players[i]->powers.maxBombs += 1;
								// TODO: update side window
								break;
							case B::QUICK_FUSE:
								if (players[i]->powers.bombFuseTime == Game::Bomb::DEFAULT_FUSE)
									players[i]->powers.bombFuseTime /= 2.;
								break;
							case B::MAX_RANGE:
								if (players[i]->powers.bombRadius < Game::Bomb::MAX_RADIUS)
									players[i]->powers.bombRadius += 1;
								break;
							case B::HEALTH_SMALL:
								if (players[i]->getLife() < Game::Player::MAX_LIFE)
									players[i]->decLife(-1);
								break;
							case B::HEALTH_FULL:
								if (players[i]->getLife() < Game::Player::MAX_LIFE)
									players[i]->setLife(players[i]->getMaxLife());
								break;
							case B::SHIELD:
								players[i]->giveShield(Game::Bonus::SHIELD_DURATION);
								break;
							case B::SPEEDY:
								players[i]->giveSpeedy(Game::Bonus::SPEEDY_DURATION);
								break;
							default:
								std::cerr << "[ LevelRenderer ] Warning: unknown bonus id "
									<< bonus->getType() << std::endl;
							}

							Game::score[i] += bonus->getPointsGiven();
							_spawnPoints(bonus->getPosition(), bonus->getPointsGiven());

							delete bonus;
							fixedEntities[idx] = nullptr;

							break;
						}
					default:
						break;
					}
				}
			}

			// Check for impacts with bosses
			for (const auto& boss : bosses) {
				if (!boss->isDying() && boss->occupies(next_tile + sf::Vector2i(1, 1))) {
					if (is_player) {
						if (!(entity->hasShield() || entity->isDying())) {
							entity->kill();
							break;
						}
					} else {
						entity->colliding = true;
						break;
					}
				}
			}
		}
	}

	// Check impact bullets - fixed/borders
	for (auto& bullet : bullets) {
		const auto pos = bullet->getPosition();
		const auto szpos = pos + sf::Vector2f(bullet->getSize(), bullet->getSize());
		if (szpos.x < TILE_SIZE || szpos.x >= (LEVEL_WIDTH + 1) * TILE_SIZE
				|| szpos.y < TILE_SIZE || szpos.y >= (LEVEL_HEIGHT + 1) * TILE_SIZE) {
			bullet->destroy();
			continue;
		}

		if (!bullet->isTransparentToWalls()) {
			const auto tile = Game::tile(pos);
			const unsigned short idx = (tile.y - 1) * LEVEL_WIDTH + tile.x - 1;
			if (idx >= fixedEntities.size()) {
				bullet->destroy();
				continue;
			}
			FixedEntity *other = fixedEntities[idx];
			if (other != nullptr && !other->transparentTo.bullets) {
				bullet->destroy();
				continue;
			}
		}
		
		for (const auto& boss : bosses) {
			if (!boss->isDying() && boss != bullet->getSource()
					&& boss->intersects(sf::FloatRect(szpos.x, szpos.y, bullet->getSize(), bullet->getSize()))) {
				bullet->destroy();
				break;	
			}
		}
	}
}

void LevelRenderer::selectEnemyMoves() {
	for (auto& entity : movingEntities) {
		if (isPlayer(entity) || entity->isDying())
			continue;

		auto enemy = static_cast<Game::Enemy*>(entity);
		if (!enemy->isAligned()) {
			if (enemy->colliding) {
				// Fix prevAligns
				switch (enemy->getDirection()) {
				case Direction::LEFT: case Direction::UP:
					enemy->prevAlign = Game::tile(enemy->getPosition());
					break;
				case Direction::RIGHT:
					enemy->prevAlign = Game::tile(enemy->getPosition()) + sf::Vector2i(1, 0);
					break;
				case Direction::DOWN:
					enemy->prevAlign = Game::tile(enemy->getPosition()) + sf::Vector2i(0, 1);
					break;
				case Direction::NONE: break;
				}
				enemy->setDashing(false);
				enemy->setDirection(oppositeDirection(entity->getDirection()));
			}
		} else if (enemy != nullptr && !enemy->isBlocked()) {
			enemy->setDirection(enemy->getAI()(this));
		}
	}
}

void LevelRenderer::moveBullets() {
	for (auto& bullet : bullets) {
		bullet->move();
	}
}

void LevelRenderer::applyEnemyMoves() {
	for (auto& entity : movingEntities) {
		if (isPlayer(entity) || entity->isDying()) 
			continue;

		if (entity->isAligned()) {
			const auto cur_align = Game::tile(entity->getPosition());
			entity->prevAlign = cur_align; 
		}

		auto enemy = static_cast<Game::Enemy*>(entity);

		// Check if this enemy should shoot
		if (!extraGame
				&& !(enemy->attack.type & Game::Enemy::AttackType::CONTACT)
				&& enemy->seeingPlayer == enemy->getDirection()) 
		{
			if (!enemy->isRecharging()) { 
				if (!(enemy->attack.type & Game::Enemy::AttackType::RANGED)
						|| enemy->distanceWithNearestPlayer <= enemy->attack.range)
				{
					enemy->shoot();
					auto bullet = new Game::Bullet(
								enemy->getPosition(), 
								enemy->getDirection(),
								enemy->attack.id,
								enemy->attack.speed,
								enemy->attack.damage,
								enemy->attack.range);
					bullet->setOrigin(origin);
					bullet->setSource(enemy);
					bullets.push_back(bullet);

					if (enemy->attack.type & Game::Enemy::AttackType::BLOCKING) {
						if (enemy->attack.type & Game::Enemy::AttackType::SIMPLE)
							enemy->block();
						else {
							enemy->stop();
							continue;
						}
					}
				}
			} else if (enemy->attack.type & Game::Enemy::AttackType::BLOCKING
					&& !(enemy->attack.type & Game::Enemy::AttackType::SIMPLE)) {
				enemy->stop();
				continue;
			}
		}
		entity->move();
	}
}

bool LevelRenderer::isEntityTouching(const sf::Vector2f& tile) const {
	sf::FloatRect tileRect(tile.x, tile.y, TILE_SIZE, TILE_SIZE);
	for (auto& entity : movingEntities) {
		sf::Vector2f pos = entity->getPosition();
		sf::FloatRect rect(pos.x, pos.y, TILE_SIZE, TILE_SIZE);
		if (rect.intersects(tileRect)) return true;
	}
	return false;
}

void LevelRenderer::_pushTemporary(Temporary *const tmp) {
	temporary.push_back(tmp);
	tmp->setOrigin(origin);
	tmp->play();
}

void LevelRenderer::dropBomb(const unsigned short id) {
	// Count how many bombs the player has already dropped
	unsigned short n_bombs = 0, idx = 0;
	const auto pl_tile = Game::tile(players[id]->getPosition());
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
		for (unsigned short j = 0; j < Game::Player::MAX_MAX_BOMBS; ++j) {
			if (bombs[i][j] == nullptr) {
				if (i == id) idx = j;
			} else {
				// Don't drop 2 bombs in the same tile
				if (Game::tile(bombs[i][j]->getPosition()) == pl_tile)
					return;
				if (i == id) ++n_bombs;
			}
		}
	}
	if (n_bombs == players[id]->powers.maxBombs) return;
	auto bomb = new Game::Bomb(
			players[id]->getPosition(),
			players[id],
			players[id]->powers.bombFuseTime,
			players[id]->powers.bombRadius);
	bombs[id][idx] = bomb;
	bomb->setOrigin(origin);
}

void LevelRenderer::checkBombExplosions() {
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i)
		for (unsigned short j = 0; j < bombs[i].size(); ++j)
			if (bombs[i][j] != nullptr && bombs[i][j]->isExploding()) {
				auto expl = new Game::Explosion(bombs[i][j]->getPosition(), bombs[i][j]->getRadius(), bombs[i][j]->getSourcePlayer());
				expl->propagate(this);
				expl->setOrigin(origin);
				expl->play();
				explosions.push_back(expl);
				bombs[i][j]->blowUp();
				expl->checkHit(this);
			}
}

void LevelRenderer::checkExplosionHits() {
	for (auto& expl : explosions)
		expl->checkHit(this);
}

Game::Bomb* LevelRenderer::getBombAt(const unsigned short left, const unsigned short top) const {
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i)
		for (unsigned short j = 0; j < bombs[i].size(); ++j) {
			if (bombs[i][j] == nullptr) continue;
			const auto tile = Game::tile(bombs[i][j]->getPosition());
			if (tile.x == left && tile.y == top) return bombs[i][j];
		}
	return nullptr;
}

void LevelRenderer::checkLinesOfSight() {
	std::array<sf::Vector2i, Game::MAX_PLAYERS> pos;
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i)
		if (players[i] != nullptr)
			pos[i] = Game::tile(players[i]->getPosition());

	for (auto& e : movingEntities) {
		if (isPlayer(e)) continue;
		auto enemy = static_cast<Game::Enemy*>(e);
		if (enemy->isDying() || enemy->isRecharging()) continue;

		enemy->seeingPlayer = Direction::NONE;
		const auto epos = Game::tile(e->getPosition());
		// set a distance greater than any possible one
		enemy->distanceWithNearestPlayer = 2 * LEVEL_WIDTH; 
		for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i) {
			if (players[i] == nullptr || players[i]->isDying()) continue;
			if (pos[i].x == epos.x) {
				const short dist = _getDistance(epos, pos[i], false);
				// _getDistance returns -1 if sight is blocked
				if (dist < 0) break;
				if (dist < enemy->distanceWithNearestPlayer) {
					enemy->seeingPlayer = epos.y < pos[i].y 
								? Direction::DOWN 
								: Direction::UP;
					enemy->distanceWithNearestPlayer = dist;
				}
			} else if (pos[i].y == epos.y) {
				const short dist = _getDistance(epos, pos[i], true);
				if (dist < 0) break;
				if (dist < enemy->distanceWithNearestPlayer) {
					enemy->seeingPlayer = epos.x < pos[i].x 
								? Direction::RIGHT 
								: Direction::LEFT;
					enemy->distanceWithNearestPlayer = dist;
				}
			}
		}
	}
}

short LevelRenderer::_getDistance(const sf::Vector2i& src, const sf::Vector2i& target, bool axis_x) {
	unsigned short start, end;
	// Assume src and target are on the same axis and:
	//   if no opaque entity is between them, return their distance
	//   else return -1.
	if (axis_x) {
		if (src.x < target.x)
			start = src.x, end = target.x;
		else
			start = target.x, end = src.x;

		for (unsigned short i = start + 1; i < end; ++i) {
			const unsigned short idx = (src.y - 1) * LEVEL_WIDTH + i - 1;
			if (idx >= fixedEntities.size()) 
				return -1;

			auto fxd = fixedEntities[idx];
			if (fxd != nullptr && !fxd->transparentTo.enemies)
				return -1;
		}
	} else {
		if (src.y < target.y)
			start = src.y, end = target.y;
		else
			start = target.y, end = src.y;

		for (unsigned short i = start + 1; i < end; ++i) {
			const unsigned short idx = (i - 1) * LEVEL_WIDTH + src.x - 1;
			if (idx >= fixedEntities.size()) 
				return -1;
			
			auto fxd = fixedEntities[idx];
			if (fxd != nullptr && !fxd->transparentTo.enemies)
				return -1; 
		}
	}
	return end - start;
}

bool LevelRenderer::isPlayer(const Entity *const e) const {
	for (unsigned short i = 0; i < players.size(); ++i)
		if (e == players[i]) return true;
	return false;
}

bool LevelRenderer::removePlayer(const unsigned short id) {
	//movingEntities.remove(players[id-1]);
	movingEntities.erase(std::remove(movingEntities.begin(), 
				movingEntities.end(), players[id-1]), movingEntities.end());
	delete players[id-1];
	players[id-1] = nullptr;
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i)
		if (players[i] != nullptr) return true;
	return false;
}

short LevelRenderer::_getPlayerIndex(const Game::Entity *const e) const {
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i)
		if (e == players[i]) return i;
	return -1;
}

void LevelRenderer::_spawnPoints(const sf::Vector2f& pos, const int amount, bool large) {
	// center the points in the tile
	short nletters = 1;
	int a = amount;
	while (a > 9) {
		a /= 10;
		++nletters;
	}
	auto width = Game::Points::CHARACTER_SIZE * nletters;
	if (large)
		_pushTemporary(new Game::Points(pos + sf::Vector2f((TILE_SIZE - width) / 2., 0.f), std::to_string(amount),
					sf::Color::Magenta, 20));
	else
		_pushTemporary(new Game::Points(pos + sf::Vector2f((TILE_SIZE - width) / 2., 0.f), std::to_string(amount)));
}

void LevelRenderer::spawnDamage(const sf::Vector2f& pos, const int amount) {
	short nletters = 2;
	int a = amount;
	while (a > 9) {
		a /= 10;
		++nletters;
	}
	auto width = Game::Points::CHARACTER_SIZE * nletters;
	auto dmg = new Game::Points(pos + sf::Vector2f((TILE_SIZE - width) / 2., 0.f), std::to_string(-amount));
	dmg->setColor(sf::Color::Red, sf::Color::White);
	_pushTemporary(dmg);
}

void LevelRenderer::makeBossesShoot() {
	if (bosses.size() == 0)
		return;

	unsigned short shootInterval = Game::Boss::SHOOT_INTERVAL;

	if (bossClockCycle != 0)
		shootInterval = Game::Boss::SHOOT_SHORT_INTERVAL;
	else if (hurryUp) 
		shootInterval *= 0.6;

	if (bossShootClock.getElapsedTime().asMilliseconds() < shootInterval)
		return;

	if (++bossClockCycle == 3)
		bossClockCycle = 0;

	bossShootClock.restart();
	for (auto& boss : bosses) {
		if (boss->isDying()) continue;
		const auto ppos = _findNearestPlayer(boss->getPosition());
		if (ppos.x < 0) {
			// no players found
			return;
		}
		
		const auto angles = boss->getShootingAngles(ppos);
		for (unsigned short i = 0; i < angles.size(); ++i) {
			const auto sp = boss->getShootingPoints()[i];
			if (Game::distance(sp, ppos) > Game::Boss::MAX_RANGE)
				continue;
			auto bullet = new Game::BossBullet(sp, angles[i]);
			bullet->setOrigin(origin);
			bullet->setSource(boss);
			bullets.push_back(bullet);
		}
	}
}

sf::Vector2f LevelRenderer::_findNearestPlayer(const sf::Vector2f& pos) const {
	sf::Vector2f nearest(-1.f, -1.f);

	for (const auto& player : players) {
		if (player == nullptr || player->isDying()) continue;
		const auto ppos = player->getPosition();
		if (nearest.x < 0 || Game::distance(pos, ppos) < Game::distance(pos, nearest)) {
			nearest = ppos;
		}
	}
	return nearest;
}

void LevelRenderer::dropBonus(const sf::Vector2i& tile, const unsigned short type) {
	const unsigned short idx = (tile.y - 1) * Game::LEVEL_WIDTH + tile.x - 1;
	auto bonus = new Game::Bonus(sf::Vector2f(tile.x * TILE_SIZE, tile.y * TILE_SIZE), type);
	bonus->setOrigin(origin);
	fixedEntities[idx] = bonus;
}

void LevelRenderer::_destroyAllWalls() {
	for (auto& fxd : fixedEntities) {
		// Here it's acceptable to use dynamic_cast, since this function
		// is called very rarely (only when the Zapper is found)
		auto bw = dynamic_cast<Game::BreakableWall*>(fxd);
		if (bw != nullptr) {
			bw->destroy();
			for (unsigned short i = 0; i < players.size(); ++i)
				if (players[i] != nullptr && players[i]->getRemainingLives() > 0)
					Game::score[i] += bw->getPointsGiven();
			_spawnPoints(bw->getPosition(), bw->getPointsGiven());
		}
	}
}

void LevelRenderer::_killAllEnemies() {
	for (auto& e : movingEntities) {
		if (isPlayer(e)) continue;
		_pushTemporary(new Game::Explosion(e->getPosition(), 0));
		e->kill();
		auto se = dynamic_cast<Game::Scored*>(e);
		if (se != nullptr) {
			for (unsigned short i = 0; i < players.size(); ++i)
				if (players[i] != nullptr && players[i]->getRemainingLives() > 0)
					Game::score[i] += se->getPointsGiven();
			_spawnPoints(e->getPosition(), se->getPointsGiven());
		}
		if (extraGame)
			_spawnLetter(e->getPosition());
	}

	for (auto& b : bosses) {
		if (!b->isDying()) {
			_pushTemporary(new Game::Explosion(b->getPosition(), 0));
			b->decLife(1);
			b->hurt();
		}
	}
}

void LevelRenderer::checkHurryUp() {
	if (hurryUp) return;
	int diff = level->getTime() - static_cast<int>(levelTimeClock.getElapsedTime().asSeconds());
	if (diff <= 0) {
		// Trigger Hurry Up	
		for (auto& e : movingEntities) {
			if (isPlayer(e)) continue;
			auto enemy = static_cast<Game::Enemy*>(e);
			enemy->giveSpeedy(std::numeric_limits<int>::max());
			enemy->attack.fireRate *= 2.;		
		}
		hurryUp = true;
	} else if (!hurryUpWarningGiven && diff <= 31) {
		hurryUpText = new Game::DroppingText(Game::getAsset("test", "hurryup.png"), sf::Vector2i(161, 30), 300.f);
		hurryUpText->setOrigin(origin);
		hurryUpWarningGiven = true;
	}
}

void LevelRenderer::triggerGameOver() {
	gameOverText = new Game::DroppingText(Game::getAsset("test", "gameover.png"), sf::Vector2i(311, 59));
	gameOverText->setOrigin(origin);
}

void LevelRenderer::_triggerExtraGame() {
	extraGameText = new Game::DroppingText(Game::getAsset("test", "extragame.png"), sf::Vector2i(223, 156), 250.f);
	extraGameText->setOrigin(origin);
	for (auto& e : movingEntities) {
		if (isPlayer(e)) continue;
		auto enemy = static_cast<Game::Enemy*>(e);
		enemy->setMorphed(true);
	}
	extraGame = true;
	extraGameClock.restart();
}

void LevelRenderer::checkExtraGameEnd() {
	if (!extraGame) return;
	if (extraGameClock.getElapsedTime().asSeconds() > 30) {
		extraGame = false;

		// Delete all letters
		for (auto it = letters.begin(); it != letters.end(); ) {
			delete *it;
			it = letters.erase(it);
		}

		// Change back enemies
		for (auto& e : movingEntities) {
			if (isPlayer(e)) continue;
			auto enemy = static_cast<Game::Enemy*>(e);
			enemy->setMorphed(false);
		}
	}
}

void LevelRenderer::_spawnLetter(const sf::Vector2f& pos) {
	auto letter = new Game::Letter(pos, Game::Letter::randomId());
	letter->setOrigin(origin);
	letters.push_back(letter);
}

void LevelRenderer::cycleLetters() {
	for (auto& letter : letters)
		letter->checkTransition();
}

void LevelRenderer::givePointsTo(const Game::Player *const player, const sf::Vector2f& pos, const int amount) {
	if (player == nullptr) return;
	_spawnPoints(pos, amount);
	Game::score[_getPlayerIndex(player)] += amount;
}
