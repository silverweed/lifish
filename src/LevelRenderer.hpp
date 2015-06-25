#pragma once
/**
 * The LevelRenderer manages a Level during the game, updating its
 * entities, time and so on.
 */
#include <SFML/Graphics.hpp>
#include <vector>
#include "Level.hpp"
#include "FixedEntity.hpp"
#include "MovingEntity.hpp"

namespace Game {

class LevelRenderer {
	using FixedEntityList = std::vector<Game::FixedEntity*>;
	using MovingEntityList = std::vector<Game::MovingEntity*>;

	/** The level this object is rendering */
	Game::Level *const level;
	
	/** The fixed entities */
	FixedEntityList fixedEntities;

	/** The moving entities */
	MovingEntityList movingEntities;

public:
	LevelRenderer(Game::Level *const _level) : level(_level) {}
	~LevelRenderer();

	/** Creates the initial entities based on this level's tilemap. */
	void loadEntities();
	void renderFrame(sf::RenderWindow& window);
};

}
