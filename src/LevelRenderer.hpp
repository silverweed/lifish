#pragma once
/**
 * The LevelRenderer manages a Level during the game, updating its
 * entities, time and so on.
 */
#include <SFML/Graphics.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <vector>
#include <forward_list>
#include <list>
#include <array>
#include "Level.hpp"
#include "FixedEntity.hpp"
#include "MovingEntity.hpp"
#include "Player.hpp"
#include "Teleport.hpp"
#include "Temporary.hpp"
#include "Bomb.hpp"

namespace Game {

class LevelRenderer : private sf::NonCopyable {
	/** For Fixed Entities we use a fixed-size array for faster lookup */
	using FixedEntityList = std::array<Game::FixedEntity*, LEVEL_WIDTH * LEVEL_HEIGHT>;
	using MovingEntityList = std::vector<Game::MovingEntity*>;
	using TemporaryEntityList = std::list<Game::Temporary*>;

	/** The level this object is rendering */
	Game::Level *level = nullptr;
	
	/** The fixed entities */
	FixedEntityList fixedEntities;

	/** The moving entities */
	MovingEntityList movingEntities;

	/** The temporary entities (flashes, explosions, ...) */
	TemporaryEntityList temporary;
	
	/** The players' bombs */
	Matrix<Game::Bomb*, Game::MAX_PLAYERS, Game::Player::MAX_MAX_BOMBS> bombs;

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

	void _pushTemporary(Temporary *const tmp);

	/** If entity in `src` has a line of sight with the one in `target`, 
	 *  returns the number of tiles separating them; else return -1.
	 */
	short _getDistance(const sf::Vector2i& src, const sf::Vector2i& target, bool axis_x);
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

	Game::Bomb* getBombAt(const unsigned short left, const unsigned short top) const;

	void setOrigin(const sf::Vector2f& _origin) { origin = _origin; }

	/** Checks if any moving entity's bounding box is intersecting
	 *  the rectangle at coordinates `pos`
	 */
	bool isEntityTouching(const sf::Vector2f& pos) const;

	/** Returns all entities (moving and fixed) touching the tile
	 *  at coordinate `pos`
	 */
	//std::list<Entity*> getEntitiesTouching(const sf::Vector2i& pos) const;

	const Level* getLevel() const { return level; }

	void dropBomb(const unsigned short player_id);

	/** Checks all the bombs deployed to see if any of them has finished its countdown and,
	 *  if so, spawns an explosion at its place.
	 */
	void checkBombExplosions();

	/** Checks if a bomb or a moving entity is touching an explosion */
	void checkExplosionHits();

	/** Sets seeingPlayer and playerIsVisible flags for all enemies */
	void checkLinesOfSight();
};

}
