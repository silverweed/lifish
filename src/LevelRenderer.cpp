#include "LevelRenderer.hpp"
#include "EntityType.hpp"
#include "FixedWall.hpp"
#include "BreakableWall.hpp"
#include "Coin.hpp"
#include <sstream>

using Game::LevelRenderer;

LevelRenderer::~LevelRenderer() {
	_clearEntities();
}

void LevelRenderer::_clearEntities() {
	for (auto& e : fixedEntities)
		delete e;
	for (auto& e : movingEntities)
		delete e;
	for (unsigned short i = 0; i < MAX_PLAYERS; ++i)
		players[i] = nullptr;
}

void LevelRenderer::loadLevel(Game::Level *const _level) {
	if (level != nullptr) {
		_clearEntities();
		fixedEntities.clear();
		movingEntities.clear();
	}
	level = _level;
		
	auto curPos = [] (unsigned short left, unsigned short top) {
		return sf::Vector2f((left+1) * TILE_SIZE, (top+1) * TILE_SIZE);
	};

	for (unsigned short left = 0; left < LEVEL_WIDTH; ++left) {
		for (unsigned short top = 0; top < LEVEL_HEIGHT; ++top) {
			switch (level->getTile(left, top)) {
			case EntityType::FIXED: 
				fixedEntities.push_back(new Game::FixedWall(
							curPos(left, top),
							level->tileIDs.fixed));
				break;
			case EntityType::BREAKABLE:
				fixedEntities.push_back(new Game::BreakableWall(
							curPos(left, top),
							level->tileIDs.breakable));
				break;
			case EntityType::COIN:
				fixedEntities.push_back(new Game::Coin(
							curPos(left, top),
							getAsset("graphics", "coin.png")));
				break;
			case EntityType::PLAYER1: {
				Game::Player *player = new Game::Player(curPos(left, top), 1);
				movingEntities.push_back(player);
				players[0] = player;
				break;
			}
			case EntityType::PLAYER2: {
				Game::Player *player = new Game::Player(curPos(left, top), 2);
				movingEntities.push_back(player);
				players[1] = player;
				break;
			}
			case EntityType::TELEPORT:
			case EntityType::ENEMY1:
			case EntityType::ENEMY2:
			case EntityType::ENEMY3:
			case EntityType::ENEMY4:
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

	level->draw(window);
	for (const auto& entity : fixedEntities)
		entity->draw(window);
	for (const auto& entity : movingEntities)
		entity->draw(window);
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
		case DIR_UP:
			return iposx - TILE_SIZE < ioposx && ioposx < iposx + TILE_SIZE
				&& iposy - TILE_SIZE <= ioposy && ioposy <= iposy;
		case DIR_LEFT:
			return iposx - TILE_SIZE <= ioposx && ioposx <= iposx
				&& iposy - TILE_SIZE < ioposy && ioposy < iposy + TILE_SIZE;
		case DIR_DOWN:
			return iposx - TILE_SIZE < ioposx && ioposx < iposx + TILE_SIZE
				&& iposy + TILE_SIZE <= ioposy && ioposy < iposy + 2 * TILE_SIZE;
		case DIR_RIGHT:
			return iposx + TILE_SIZE <= ioposx && ioposx < iposx + 2 * TILE_SIZE
				&& iposy - TILE_SIZE < ioposy && ioposy < iposy + TILE_SIZE;
		case DIR_NONE:
			return false;
		}
		return false;
	};

	for (unsigned short i = 0, len = movingEntities.size(); i < len; ++i) {
		if (checked[i]) continue;
		MovingEntity *entity = movingEntities[i];
		sf::Vector2f pos = entity->getPosition();
		const bool is_player = (entity == players[0] or entity == players[1]);
		std::forward_list<Game::Entity*> others;
		bool at_limit = false;

		for (unsigned short i = 0; i < 4; ++i)
			entity->colliding[i] = false;

		const Direction dir = entity->getDirection();
		const unsigned short iposx = (unsigned short)pos.x,
		                     iposy = (unsigned short)pos.y;

		switch (dir) {
		case DIR_UP:
			if (iposy == TILE_SIZE) {
				// Reached the top
				entity->colliding[DIR_UP] = true;
				continue;
			}

			at_limit = iposy % TILE_SIZE == 0;
			break;
		case DIR_LEFT:
			if (iposx == TILE_SIZE) {
				entity->colliding[DIR_LEFT] = true;
				continue;
			}
			at_limit = iposx % TILE_SIZE == 0;
			break;
		case DIR_DOWN:
			if (iposy == TILE_SIZE * LEVEL_HEIGHT) {
				entity->colliding[DIR_DOWN] = true;
				continue;
			}
			at_limit = iposy % TILE_SIZE == 0;
			break;
		case DIR_RIGHT:
			if (iposx == TILE_SIZE * LEVEL_WIDTH) {
				entity->colliding[DIR_RIGHT] = true;
				continue;
			}
			at_limit = iposx % TILE_SIZE == 0;
			break;
		case DIR_NONE:
			continue;
		}
		for (unsigned short j = i + 1; j < len; ++j) {
			MovingEntity *other = movingEntities[j];
			if ((is_player && other->transparentTo.players) || other->transparentTo.enemies)
				continue;
			sf::Vector2f opos = other->getPosition();

			if (collide(pos, opos, dir)) {
				entity->colliding[dir] = true;
				checked[i] = true;
				if (other->getDirection() == (dir + 2) % 4) {
					other->colliding[(dir + 2) % 4] = true;
					checked[j] = true;
				}
				break;
			}
		}
		if (at_limit) {
			for (auto& other : fixedEntities) {
				if ((is_player && other->transparentTo.players) || other->transparentTo.enemies)
					continue;
				sf::Vector2f opos = other->getPosition();
				if (collide(pos, opos, dir)) {
					entity->colliding[dir] = true;
					checked[i] = true;
					break;
				}
			}
		}
	}
}
