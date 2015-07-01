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
#include "Player.hpp"

namespace Game {

class LevelRenderer {
	using FixedEntityList = std::vector<Game::FixedEntity*>;
	using MovingEntityList = std::vector<Game::MovingEntity*>;

	/** The level this object is rendering */
	Game::Level *level;
	
	/** The fixed entities */
	FixedEntityList fixedEntities;

	/** The moving entities */
	MovingEntityList movingEntities;

	/** The players */
	Game::Player *players[Game::MAX_PLAYERS] = { nullptr, nullptr };


	/** Deletes all entities */
	void _clearEntities();

public:
	LevelRenderer() {}
	~LevelRenderer();

	/** Creates the initial entities based on this level's tilemap. */
	void loadLevel(Game::Level *const _level);
	void renderFrame(sf::RenderWindow& window);
};

}
