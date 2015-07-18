#include "LevelRenderer.hpp"
#include "EntityType.hpp"
#include "FixedWall.hpp"
#include "BreakableWall.hpp"
#include "Coin.hpp"
#include "Flash.hpp"
#include "Enemy.hpp"
#include "Explosion.hpp"
#include "Points.hpp"
#include "utils.hpp"
#include <sstream>

using Game::LevelRenderer;

LevelRenderer::LevelRenderer() {
	fixedEntities.fill(nullptr);
	players.fill(nullptr);
	for (unsigned short i = 0; i < bombs.size(); ++i)
		bombs[i].fill(nullptr);
}

LevelRenderer::~LevelRenderer() {
	_clearEntities();
}

void LevelRenderer::_clearEntities() {
	for (auto& e : fixedEntities)
		if (e != nullptr) delete e;
	for (auto& e : movingEntities)
		delete e;
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

	players.fill(nullptr);
	fixedEntities.fill(nullptr);

	firstTeleport = nullptr;

	temporary.clear();
	movingEntities.clear();
	bullets.clear();
}

void LevelRenderer::loadLevel(Game::Level *const _level) {
	if (level != nullptr) {
		_clearEntities();
	}
	level = _level;

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
				float speed;
				short range;
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
				// TODO
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
				enemy_attack.fireRate = 1.5;
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
				enemy->attack.range = enemy_attack.range;
				movingEntities.push_back(enemy);
			}
		}
	}
	
	level->setOrigin(origin);
	for (const auto& entity : fixedEntities) 
		if (entity != nullptr)
			entity->setOrigin(origin);
	for (const auto& entity : movingEntities)
		entity->setOrigin(origin);
}

void LevelRenderer::renderFrame(sf::RenderWindow& window) {
	if (level == nullptr) return;

	level->draw(window);
	for (unsigned short left = 0; left < LEVEL_WIDTH; ++left) {
		for (unsigned short top = 0; top < LEVEL_HEIGHT; ++top) {
			const unsigned short idx = top * LEVEL_WIDTH + left;
			const auto entity = fixedEntities[idx];
			if (entity == nullptr) continue;

			const auto e_type = level->getTile(left, top);
			switch (e_type) {
			case EntityType::BREAKABLE:
				{
					auto bw = static_cast<Game::BreakableWall*>(entity);
					if (bw->isDestroyed()) {
						delete bw;
						fixedEntities[idx] = nullptr;
					} else {
						bw->draw(window);
					}
					break;
				}
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
				// TODO: powerups
				entity->draw(window);
			}
		}
	}

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
	
	for (auto it = movingEntities.begin(); it != movingEntities.end(); ) {
		auto entity = *it;
		if (entity->isDying() && !isPlayer(entity)) {
			entity->prepareDeathAnimation();
			if (entity->playDeathAnimation()) {
				delete entity;
				it = movingEntities.erase(it);
				continue;
			}
		}
		entity->draw(window);
		++it;
	}
	
	for (auto it = temporary.begin(); it != temporary.end(); ) {
		if (!(*it)->isPlaying()) {
			delete *it;
			it = temporary.erase(it);
		} else {
			(*it)->draw(window);
			++it;
		}
	}

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

	unsigned short i = 0;
	for (auto it = movingEntities.begin(); it != movingEntities.end(); ++it, ++i) {
		if (checked[i]) continue;
		MovingEntity *entity = *it;
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

		// Check impact with bullets
		if (!entity->transparentTo.bullets) {
			for (auto it = bullets.begin(); it != bullets.end(); ++it) {
				auto bullet = *it;
				if (entity != bullet->getSource() && bullet->hits(entity->getPosition())) {
					if (is_player && !(entity->hasShield() || entity->isDying())) {
						// Damage player
						auto player = static_cast<Game::Player*>(entity);
						player->decLife(bullet->getDamage());
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
		unsigned short j = 0;
		for (auto jt = movingEntities.begin(); jt != movingEntities.end(); ++jt, ++j) {
			if (i == j) continue;

			MovingEntity *other = *jt;
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

				if (!opaque && !(enemy->isDying() || player->isDying() || player->hasShield())) {
					if (enemy->attack.type & Enemy::AttackType::CONTACT) {
						// TODO: attack sprite
						player->decLife(enemy->attack.damage);
					} else {
						player->decLife(1);
					}
					if (!player->isHurt()) {
						player->setHurt(true);
						player->giveShield(Game::DAMAGE_SHIELD_TIME);
					}
					if (player->getLife() <= 0)
						player->kill();
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
					// Either a coin, a powerup or a teleport. We can know for sure
					// what this entity is with a single lookup to level->tiles:
					// if we find a teleport or a coin, it means it's that entity;
					// else, if we find a breakable, it's a powerup.
					switch (level->getTile(next_tile.x, next_tile.y)) {
					case EntityType::COIN:
						{
							if (!is_player) break;
							auto coin = static_cast<Game::Coin*>(other);
							if (!coin->isBeingGrabbed()) {
								coin->grab();
								Game::score[_getPlayerIndex(entity)] += coin->getPointsGiven();
								spawnPoints(coin->getPosition(), coin->getPointsGiven());
							}
							break;
						}
					case EntityType::BREAKABLE:
						// TODO: grab the powerup
					default:
						break;
					}
				}
			}
		}
	}

	for (auto& bullet : bullets) {
		const auto pos = bullet->getPosition();
		const auto szpos = pos + sf::Vector2f(bullet->getSize(), bullet->getSize());
		if (szpos.x < TILE_SIZE || szpos.x >= (LEVEL_WIDTH + 1) * TILE_SIZE
				|| szpos.y < TILE_SIZE || szpos.y >= (LEVEL_HEIGHT + 1) * TILE_SIZE) {
			bullet->destroy();
			continue;
		}
		const auto tile = Game::tile(pos);
		const unsigned short idx = (tile.y - 1) * LEVEL_WIDTH + tile.x - 1;
		if (idx >= fixedEntities.size()) {
			bullet->destroy();
			continue;
		}
		FixedEntity *other = fixedEntities[idx];
		if (other != nullptr && !other->transparentTo.bullets) {
			bullet->destroy();
		}
	}
}

void LevelRenderer::selectEnemyMoves() {
	for (auto& entity : movingEntities) {
		if (isPlayer(entity) || entity->isDying())
			continue;

		if (!entity->isAligned()) {
			if (entity->colliding) {
				// Fix prevAligns
				switch (entity->getDirection()) {
				case Direction::LEFT: case Direction::UP:
					entity->prevAlign = Game::tile(entity->getPosition());
					break;
				case Direction::RIGHT:
					entity->prevAlign = Game::tile(entity->getPosition()) + sf::Vector2i(1, 0);
					break;
				case Direction::DOWN:
					entity->prevAlign = Game::tile(entity->getPosition()) + sf::Vector2i(0, 1);
					break;
				case Direction::NONE: break;
				}
				entity->setDirection(oppositeDirection(entity->getDirection()));
			}
			continue;
		}
		auto enemy = static_cast<Game::Enemy*>(entity);
		if (enemy != nullptr) {
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
		if (!(enemy->attack.type & Game::Enemy::AttackType::CONTACT)
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
						enemy->stop();
						continue;
					}
				}
			} else if (enemy->attack.type & Game::Enemy::AttackType::BLOCKING) {
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
			players[id]->powers.bombFuseTime,
			players[id]->powers.bombRadius);
	bombs[id][idx] = bomb;
	bomb->setOrigin(origin);
	bomb->ignite();
}

void LevelRenderer::checkBombExplosions() {
	for (unsigned short i = 0; i < Game::MAX_PLAYERS; ++i)
		for (unsigned short j = 0; j < bombs[i].size(); ++j)
			if (bombs[i][j] != nullptr && bombs[i][j]->isExploding()) {
				auto expl = new Game::Explosion(bombs[i][j]->getPosition(), bombs[i][j]->getRadius());
				expl->propagate(this);
				_pushTemporary(expl);
				bombs[i][j]->blowUp();
				expl->checkHit(this);
			}
}

void LevelRenderer::checkExplosionHits() {
	for (auto& tmp : temporary) {
		auto *expl = dynamic_cast<Game::Explosion*>(tmp);
		if (expl != nullptr)
			expl->checkHit(this);
	}
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
	// Check if src and target are on the same axis and:
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

			auto tile = level->getTile(i - 1, src.y - 1);
			if (tile == EntityType::FIXED) 
				return -1;

			auto fxd = fixedEntities[idx];
			if (fxd != nullptr && tile == EntityType::BREAKABLE)
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
			
			auto tile = level->getTile(src.x - 1, i - 1);
			if (tile == EntityType::FIXED)
				return -1;

			auto fxd = fixedEntities[idx];
			if (fxd != nullptr && tile == EntityType::BREAKABLE)
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
	movingEntities.remove(players[id-1]);
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

void LevelRenderer::spawnPoints(const sf::Vector2f& pos, const int amount) {
	// center the points in the tile
	short nletters = 1;
	int a = amount;
	while (a > 9) {
		a /= 10;
		++nletters;
	}
	auto width = Game::Points::CHARACTER_SIZE * nletters;
	_pushTemporary(new Game::Points(pos + sf::Vector2f((TILE_SIZE - width) / 2., 0.f), amount));
}
