#include "LevelRenderer.hpp"
#include "EntityType.hpp"
#include "FixedWall.hpp"
#include "BreakableWall.hpp"
#include "Coin.hpp"
#include "Flash.hpp"
#include "Enemy.hpp"
#include "utils.hpp"
#include <sstream>

using Game::LevelRenderer;

LevelRenderer::LevelRenderer() {
	fixedEntities.fill(nullptr);
	players.fill(nullptr);
}

LevelRenderer::~LevelRenderer() {
	_clearEntities();
}

void LevelRenderer::_clearEntities() {
	for (auto& e : fixedEntities)
		if (e != nullptr) delete e;
	for (auto& e : movingEntities)
		delete e;
	players.fill(nullptr);
	firstTeleport = nullptr;
}

void LevelRenderer::loadLevel(Game::Level *const _level) {
	if (level != nullptr) {
		_clearEntities();
		fixedEntities.fill(nullptr);
		movingEntities.clear();
	}
	level = _level;
		
	auto curPos = [] (unsigned short left, unsigned short top) {
		return sf::Vector2f((left+1) * TILE_SIZE, (top+1) * TILE_SIZE);
	};

	Game::Teleport *latest_teleport = nullptr;

	for (unsigned short left = 0; left < LEVEL_WIDTH; ++left) {
		for (unsigned short top = 0; top < LEVEL_HEIGHT; ++top) {
			switch (level->getTile(left, top)) {
			case EntityType::FIXED: 
				fixedEntities[top * LEVEL_WIDTH + left] = 
					new Game::FixedWall(curPos(left, top), level->tileIDs.fixed);
				break;
			case EntityType::BREAKABLE:
				fixedEntities[top * LEVEL_WIDTH + left] = 
					new Game::BreakableWall(curPos(left, top), level->tileIDs.breakable);
				break;
			case EntityType::COIN:
				fixedEntities[top * LEVEL_WIDTH + left] = 
					new Game::Coin(curPos(left, top), getAsset("graphics", "coin.png"));
				break;
			case EntityType::PLAYER1: 
				{
					Game::Player *player = new Game::Player(curPos(left, top), 1);
					movingEntities.push_back(player);
					players[0] = player;
					break;
				}
			case EntityType::PLAYER2: 
				{
					Game::Player *player = new Game::Player(curPos(left, top), 2);
					movingEntities.push_back(player);
					players[1] = player;
					break;
				}
			case EntityType::TELEPORT:
				{
					Game::Teleport *teleport = new Game::Teleport(curPos(left, top));
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
			case EntityType::ENEMY1:
				{
					Game::Enemy *enemy = new Game::Enemy(curPos(left, top), 1);
					enemy->setAI(Game::ai_random);
					movingEntities.push_back(enemy);
					break;
				}
			case EntityType::ENEMY2:
				{
					Game::Enemy *enemy = new Game::Enemy(curPos(left, top), 2);
					enemy->setAI(Game::ai_random);
					movingEntities.push_back(enemy);
					break;
				}
			case EntityType::ENEMY3:
				{
					Game::Enemy *enemy = new Game::Enemy(curPos(left, top), 3);
					enemy->setAI(Game::ai_random_forward);
					movingEntities.push_back(enemy);
					break;
				}
			case EntityType::ENEMY4:
				{
					Game::Enemy *enemy = new Game::Enemy(curPos(left, top), 4);
					enemy->setAI(Game::ai_random_forward);
					enemy->setSpeed(150.f);
					movingEntities.push_back(enemy);
					break;
				}
			case EntityType::ENEMY5:
			case EntityType::ENEMY6:
			case EntityType::ENEMY7:
			case EntityType::ENEMY8:
			case EntityType::ENEMY9:
			case EntityType::ENEMY10:
			case EntityType::BOSS:
			default:
				// TODO
				break;
			}
		}
	}
}

void LevelRenderer::renderFrame(sf::RenderWindow& window) {
	if (level == nullptr) return;

	level->setOrigin(origin);
	level->draw(window);
	for (const auto& entity : fixedEntities)
		if (entity != nullptr) {
			entity->setOrigin(origin);
			entity->draw(window);
		}
	for (const auto& entity : movingEntities) {
		entity->setOrigin(origin);
		entity->draw(window);
	}
	for (auto it = temporary.begin(); it != temporary.end(); ) {
		(*it)->setOrigin(origin);
		if (!(*it)->isPlaying()) {
			delete *it;
			it = temporary.erase(it);
		} else {
			(*it)->draw(window);
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
	
	static auto collide = [] (const sf::Vector2f& pos, const sf::Vector2f& opos, const Game::Direction dir) {
		unsigned short iposx = (unsigned short)pos.x,
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
		sf::Vector2f pos = entity->getPosition();
		const bool is_player = (entity == players[0] or entity == players[1]);
		std::forward_list<Game::Entity*> others;
		bool at_limit = false;

		entity->colliding = false;

		const Direction dir = entity->getDirection();
		const unsigned short iposx = (unsigned short)pos.x,
		                     iposy = (unsigned short)pos.y;
		sf::Vector2i next_tile(iposx / TILE_SIZE - 1, iposy / TILE_SIZE - 1);

		// Check for teleports
		if (firstTeleport != nullptr && entity->canTeleport && entity->isAligned()) {
			auto cur_tile = tile(pos);

			if (level->getTile(cur_tile.x - 1, cur_tile.y - 1) == EntityType::TELEPORT && entity->prevAlign != cur_tile) {
				unsigned short idx = (cur_tile.y - 1) * LEVEL_WIDTH + cur_tile.x - 1;

				// Get Teleport from fixed entities
				Game::Teleport *teleport = dynamic_cast<Teleport*>(fixedEntities[idx]);

				if (teleport == nullptr || teleport->isDisabled()) continue;
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

				// TODO: play animation
				_pushTemporary(new Game::Flash(teleport->getPosition()));
				_pushTemporary(new Game::Flash(next->getPosition()));

				// Teleport the entity
				entity->setPosition(next->getPosition());
				entity->prevAlign = tile(next->getPosition());

				// Disable both source and destination for a while
				teleport->disable();
				next->disable();
				continue;
			}
		}

		// Check for impacts with the borders
		switch (dir) {
		case Direction::UP:
			if (iposy <= TILE_SIZE) {
				// Reached the top
				entity->colliding = true;
				continue;
			}
			at_limit = iposy % TILE_SIZE == 0;
			--next_tile.y;
			break;
		case Direction::LEFT:
			if (iposx <= TILE_SIZE) {
				entity->colliding = true;
				continue;
			}
			at_limit = iposx % TILE_SIZE == 0;
			--next_tile.x;
			break;
		case Direction::DOWN:
			if (iposy >= TILE_SIZE * LEVEL_HEIGHT) {
				entity->colliding = true;
				continue;
			}
			at_limit = iposy % TILE_SIZE == 0;
			++next_tile.y;
			break;
		case Direction::RIGHT:
			if (iposx >= TILE_SIZE * LEVEL_WIDTH) {
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
			if ((is_player && other->transparentTo.players) || (!is_player && other->transparentTo.enemies))
				continue;
			
			sf::Vector2f opos = other->getPosition();

			if (collide(pos, opos, dir)) {
				entity->colliding = true;
				checked[i] = true;
				if (other->getDirection() == Game::oppositeDirection(dir)) {
					other->colliding = true;
					checked[j] = true;
				}
				collision_detected = true;
				break;
			}
		}

		// Check for impacts with fixed entities
		if (!collision_detected && at_limit) {
			unsigned short idx = next_tile.y * LEVEL_WIDTH + next_tile.x;
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
						// TODO: grab the coin
					case EntityType::BREAKABLE:
						// TODO: grab the powerup
					default:
						break;
					}
				}
			}
		}
	}
}

void LevelRenderer::selectEnemyMoves() {
	for (auto& entity : movingEntities) {
		if (entity == players[0] || entity == players[1] ||
				!(entity->isAligned() || entity->colliding)) 
		{
			continue;
		}
		Enemy *enemy = dynamic_cast<Enemy*>(entity);
		if (enemy != nullptr)
			enemy->setDirection(enemy->getAI()(this));

	}
}

void LevelRenderer::applyEnemyMoves() {
	for (auto& entity : movingEntities) {
		if (entity == players[0] || entity == players[1]) continue;
		if (entity->isAligned()) {
			auto cur_align = tile(entity->getPosition());
			if (entity->prevAlign != cur_align)
				entity->prevAlign = cur_align; 
		}
		entity->move();
	}
}

bool LevelRenderer::isEntityTouching(sf::Vector2f tile) const {
	sf::FloatRect tileRect(tile.x, tile.y, TILE_SIZE, TILE_SIZE);
	for (auto& entity : movingEntities) {
		sf::Vector2f pos = entity->getPosition();
		sf::FloatRect rect(pos.x, pos.y, TILE_SIZE, TILE_SIZE);
		if (rect.intersects(tileRect)) return true;
	}
	return false;
}

void LevelRenderer::tickTeleports() {
	Game::Teleport *ft = firstTeleport;
	if (ft == nullptr) return;
	ft->tick();
	for (Game::Teleport *t = ft->next(); t != nullptr && t != ft; t = t->next())
		t->tick();
}

void LevelRenderer::_pushTemporary(Temporary *const tmp) {
	temporary.push_back(tmp);
	tmp->play();
}
