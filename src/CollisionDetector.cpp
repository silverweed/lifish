#include "CollisionDetector.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include "Collider.hpp"
#include "Direction.hpp"
#include <algorithm>

using Game::Direction;
using Game::CollisionDetector;
using Game::TILE_SIZE;

// Checks if entity in `pos` and entity in `opos` collide, given that the one in `pos` has direction `dir`.
static bool _collide(const sf::Vector2f& pos, const sf::Vector2f& opos, const Game::Direction dir) {
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
}

void CollisionDetector::update() {
	/* For each moving entity, check (towards its direction):
	 * 1) has it reached the level boundaries?
	 * 2) is there another non-trasparent entity occupying the cell ahead?
	 */
	auto movingEntities = group.all<Game::Moving>();
	std::vector<bool> checked(group.size(), false);
	
	// Collision detection loop
	unsigned short i = 0;
	//for (unsigned short i = 0, len = movingEntities.size(); i < len; ++i) {
	for (auto& entity : movingEntities) {
		if (checked[i]) continue;
		//const auto moving = movingEntities[i];
		//auto entity = moving->getOwner();
		const auto collider = entity->get<Game::Collider>();
		// Only consider entities with a collider
		if (collider == nullptr) continue;

		//const bool is_player = entity->get<Game::Player>() != nullptr;
		//const auto killable = entity->get<Game::Killable>();
		//if (killable != nullptr && killable->isKilled()) continue;

		// Very simple check with all others
		for (unsigned short j = i + 1; j < len; ++j) {
			const auto othmoving = movingEntities[j];
			Game::Entity *othentity = othmoving->getOwner();
			const auto othcollider = othentity->get<Game::Collider>();
			if (othcollider == nullptr) continue;
			const auto othpos = othentity->getPosition();
			if (collider.contains(othcollider)) {
				collider->colliding = othcollider;
				othcollider->colliding = collider;
				checked[i] = checked[j] = true;
			}
		}

		// Check for teleports
		/*if (firstTeleport != nullptr && entity->canTeleport && entity->isAligned()) {
			const auto cur_tile = Game::tile(pos);

			if (level->getTile(cur_tile.x - 1, cur_tile.y - 1) == EntityType::TELEPORT 
					&& entity->prevAlign != cur_tile) 
			{
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
		}*/
		
		// Check for letters
		/*if (extraGame && is_player) {
			for (auto it = letters.begin(); it != letters.end(); ) {
				auto letter = *it;
				sf::FloatRect letter_box(letter->getPosition().x, letter->getPosition().y,
						TILE_SIZE, TILE_SIZE);
				sf::FloatRect player_box(pos.x, pos.y, TILE_SIZE, TILE_SIZE);
				if (player_box.intersects(letter_box)) {
					// Grab the letter
					auto player = static_cast<Game::Player*>(entity);
					player->extra[letter->getId()] = true;
					Game::cache.playSound(letter->getSoundFile());
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
						const auto upText = new Game::Points(letter->getPosition() 
								+ sf::Vector2f((TILE_SIZE - 45) / 2., 0.f),
								Game::to_string(_getPlayerIndex(player) + 1) + "UP",
								sf::Color(77, 184, 255, 255), 15);
						_pushTemporary(upText);
						Game::cache.playSound(Game::getAsset("test", Game::EXTRA_LIFE_SOUND));
					}

					delete letter;
					it = letters.erase(it);
				} else {
					++it;
				}
			}
		}*/

		// Check impact with bullets
	/*	if (!entity->transparentTo.bullets) {
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
							Game::cache.playSound(player->getSoundFile(Game::Sounds::HURT));
						}
						if (player->getLife() <= 0) {
							player->kill();
							Game::cache.playSound(player->getSoundFile(Game::Sounds::DEATH));
						}
					} 

					if (!bullet->isBeingDestroyed()) {
						Game::cache.playSound(bullet->getSoundFile(Game::Sounds::DEATH));
						bullet->destroy();	
					}
				}
			}
		}

		entity->colliding = false;

		pos = entity->getPosition();
		const Direction dir = entity->getDirection();
		const unsigned short iposx = (unsigned short)pos.x,
		                     iposy = (unsigned short)pos.y;
		sf::Vector2i next_tile(iposx / TILE_SIZE - 1, iposy / TILE_SIZE - 1);

		if (entity->isOverBoundaries(dir)) {
			entity->colliding = true;
			continue;
		}

		const bool at_limit = entity->isAtLimit(dir);
		
		// Check for impacts with the borders
		switch (dir) {
		case Direction::UP:
			--next_tile.y;
			break;
		case Direction::LEFT:
			--next_tile.x;
			break;
		case Direction::DOWN:
			++next_tile.y;
			break;
		case Direction::RIGHT:
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
						if (!enemy->isShooting()) {
							enemy->shoot();
							if (!(enemy->attack.type & Enemy::AttackType::RANGED)) {
								// Play the sound only if the attack is not "dashing"
								// (see Enemy.hpp#AttackType)
								Game::cache.playSound(enemy->getSoundFile(Game::Sounds::ATTACK));
							}
						}
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
							Game::cache.playSound(player->getSoundFile(Game::Sounds::HURT));
						}
						if (player->getLife() <= 0) {
							player->kill();
							Game::cache.playSound(player->getSoundFile(Game::Sounds::DEATH));
						}
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
			Game::Entity *other = fixedEntities[idx];
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
								Game::cache.playSound(coin->getSoundFile());
								Game::score[_getPlayerIndex(entity)] += coin->getPointsGiven();
								_spawnPoints(coin->getPosition(), coin->getPointsGiven());
								if (--coinsNum == 0) {
									_triggerExtraGame();
								}
							}
							break;
						}
					case EntityType::BREAKABLE:
					case EntityType::TRANSPARENT_WALL:
						// Grab the bonus
						if (!is_player) break;
						_grabBonus(entity, static_cast<Game::Bonus*>(other), idx);
						break;
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

			if (finalBoss != nullptr && !finalBoss->isDying() 
					&& finalBoss->occupies(next_tile + sf::Vector2i(1, 1))) {
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

	// Check final boss vs walls
	if (finalBoss != nullptr)
		finalBoss->detectCollisions(this);
		

	// Check impact bullets - fixed/borders
	for (auto& bullet : bullets) {
		if (bullet->isBeingDestroyed()) continue;
		const auto pos = bullet->getPosition();
		const auto szpos = pos + sf::Vector2f(bullet->getSize(), bullet->getSize());
		if (szpos.x < TILE_SIZE || szpos.x >= (LEVEL_WIDTH + 1) * TILE_SIZE
				|| szpos.y < TILE_SIZE || szpos.y >= (LEVEL_HEIGHT + 1) * TILE_SIZE) {
			bullet->destroy();
			Game::cache.playSound(bullet->getSoundFile(Game::Sounds::DEATH));
			continue;
		}

		if (!bullet->isTransparentToWalls()) {
			const auto tile = Game::tile(pos);
			const unsigned short idx = (tile.y - 1) * LEVEL_WIDTH + tile.x - 1;
			if (idx >= fixedEntities.size()) {
				bullet->destroy();
				Game::cache.playSound(bullet->getSoundFile(Game::Sounds::DEATH));
				continue;
			}
			Game::Entity *other = fixedEntities[idx];
			if (other != nullptr && !other->transparentTo.bullets) {
				bullet->destroy();
				Game::cache.playSound(bullet->getSoundFile(Game::Sounds::DEATH));
				continue;
			}
		}
		
		for (const auto& boss : bosses) {
			if (!boss->isDying() && boss != bullet->getSource()
					&& boss->intersects(sf::FloatRect(szpos.x, szpos.y, 
							bullet->getSize(), bullet->getSize()))) {
				bullet->destroy();
				Game::cache.playSound(bullet->getSoundFile(Game::Sounds::DEATH));
				break;	
			}
		}
	}*/
}
