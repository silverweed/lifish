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
