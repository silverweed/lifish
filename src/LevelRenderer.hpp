#pragma once
/**
 * The LevelRenderer manages a Level during the game, updating its
 * entities, time and so on.
 */
#include <SFML/Graphics.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <vector>
#include <forward_list>
#include <array>
#include "Level.hpp"
#include "FixedEntity.hpp"
#include "MovingEntity.hpp"
#include "Player.hpp"
#include "Teleport.hpp"

namespace Game {

class LevelRenderer : private sf::NonCopyable {
	/** For Fixed Entities we use a fixed-size array for faster lookup */
	using FixedEntityList = std::array<Game::FixedEntity*, LEVEL_WIDTH * LEVEL_HEIGHT>;
	using MovingEntityList = std::vector<Game::MovingEntity*>;

	/** The level this object is rendering */
	Game::Level *level = nullptr;
	
	/** The fixed entities */
	FixedEntityList fixedEntities;

	/** The moving entities */
	MovingEntityList movingEntities;

	sf::Vector2f origin;

	/** The players */
	std::array<Game::Player*, Game::MAX_PLAYERS> players;

	/** The first Teleport of the level, if any. Keeping the
	 *  pointer to it allows us to traverse the Teleports'
	 *  linked list.
	 */
	Game::Teleport *firstTeleport = nullptr;


	/** Deletes all entities */
	void _clearEntities();
public:
	LevelRenderer();
	~LevelRenderer();

	/** Creates the initial entities based on this level's tilemap. */
	void loadLevel(Game::Level *const _level);
	void renderFrame(sf::RenderWindow& window);

	/** Calculates collisions based on the current entites layout, filling
	 *  the `colliding` array for each moving entity based on their transparency.
	 */
	void detectCollisions();

	/** Cycles through enemies and calls each one's AI function.
	 *  To actually move enemies, `applyEnemyMoves` must be called
	 *  after this (better yet, after `detectCollisions`). The
	 *  proper way to move enemies in fact is:
	 *  	selectEnemyMoves();
	 *  	detectCollisions();
	 *	applyEnemyMoves();
	 */
	void selectEnemyMoves();
	void applyEnemyMoves();

	std::array<Game::Player*, Game::MAX_PLAYERS> getPlayers() const { return players; }
	Player* getPlayer(const unsigned short i) const { return players[i-1]; }
	
	const FixedEntityList& getFixedEntities() const { return fixedEntities; }
	const MovingEntityList& getMovingEntities() const { return movingEntities; }

	void setOrigin(const sf::Vector2f& _origin) { origin = _origin; }

	/** Checks if any moving entity's bounding box is intersecting
	 *  the rectangle at coordinates (tile)
	 */
	bool isEntityTouching(sf::Vector2f tile) const;

	void tickTeleports();
};

}
