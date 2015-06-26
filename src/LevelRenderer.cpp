#include "LevelRenderer.hpp"
#include "EntityType.hpp"
#include "FixedWall.hpp"
#include <sstream>

using Game::LevelRenderer;

LevelRenderer::~LevelRenderer() {
	for (auto& e : fixedEntities)
		delete e;
	for (auto& e : movingEntities)
		delete e;
}

void LevelRenderer::loadEntities() {
	auto getWallname = [this] (EntityType e) {
		std::stringstream fname;
		switch (e) {
			using E = EntityType;
		case E::FIXED: 
			fname << "fixed" << level->getFixedWallsTextureNum() << ".png";
			break;
		case E::BREAKABLE: 
			fname << "breakable" << level->getBreakableWallsTextureNum() << ".png"; 
			break;
		default: break;
		}
		return fname.str();
	};

	auto curPos = [] (unsigned short left, unsigned short top) {
		return sf::Vector2f((left+1) * TILE_SIZE, (top+1) * TILE_SIZE);
	};

	for (unsigned short left = 0; left < LEVEL_WIDTH; ++left) {
		for (unsigned short top = 0; top < LEVEL_HEIGHT; ++top) {
			switch (EntityType e = level->getTile(left, top)) {
			case EntityType::FIXED: 
				fixedEntities.push_back(new Game::FixedWall(
							curPos(left, top),
							getAsset("textures", getWallname(e))));
				break;
			case EntityType::BREAKABLE:
			case EntityType::PLAYER1:
			case EntityType::PLAYER2:
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
