#pragma once

#include <vector>
#include <array>
#include <initializer_list>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/System/NonCopyable.hpp>
#include "Level.hpp"
#include "LifedMovingEntity.hpp"
#include "Player.hpp"
#include "Teleport.hpp"
#include "Temporary.hpp"
#include "Bomb.hpp"
#include "Bullet.hpp"
#include "Boss.hpp"
#include "Explosion.hpp"
#include "DroppingText.hpp"
#include "AlienSprite.hpp"
#include "Letter.hpp"
#include "Bonus.hpp"
#include "Clocked.hpp"
#include "FinalBoss.hpp"

namespace Game {

class SaveManager;

/**
 * The LevelRenderer manages a Level during the game, updating its
 * entities, time and so on.
 */
class LevelRenderer final : public Game::Clocked, private sf::NonCopyable {
	// This is a fixed-size array for faster lookup
	using FixedEntityList = std::array<Game::Entity*, LEVEL_WIDTH * LEVEL_HEIGHT>;
	using MovingEntityList = std::vector<Game::LifedMovingEntity*>;
	using TemporaryEntityList = std::vector<Game::Temporary*>;
	using BulletList = std::vector<Game::Bullet*>;
	using BossList = std::vector<Game::Boss*>;
	using ExplosionList = std::vector<Game::Explosion*>;
	using LetterList = std::vector<Game::Letter*>;

	friend class Game::SaveManager;

	/** The level this object is rendering */
	Game::Level *level = nullptr;

	/** The time counter */
	sftools::Chronometer levelTimeClock;

	bool hurryUp = false;
	bool hurryUpWarningGiven = false;

	bool extraGame = false;
	/** The number of coins in the level, used for EXTRA game. */
	unsigned short coinsNum = 0;
	sftools::Chronometer extraGameClock;
	/** The alien sprite displayed during EXTRA game */
	Game::AlienSprite alienSprite;

	Game::DroppingText hurryUpText,
	                   gameOverText,
	                   extraGameText;
	
	/** The fixed entities */
	FixedEntityList fixedEntities;

	/** The moving entities (except bullets) */
	MovingEntityList movingEntities;

	/** The temporary entities (flashes, boss explosions ...), except bomb explosions */
	TemporaryEntityList temporary;

	/** The bomb explosions */
	ExplosionList explosions;

	/** The bullets */
	BulletList bullets;

	/** The EXTRA letters */
	LetterList letters;
	
	/** The players' bombs */
	Matrix<Game::Bomb*, Game::MAX_PLAYERS, Game::Player::MAX_MAX_BOMBS> bombs;

	sf::Vector2f origin;

	/** The players belonging to a Level */
	std::array<Game::Player*, Game::MAX_PLAYERS> players;

	/** The players' pointers, guaranteed to always point to the players
	 *  given to the constructor, even if the level tilemap doesn't contain all
	 *  players, or a player is currently dead. This pointers become invalid
	 *  after a call to removePlayer(), or setPlayer(), since those methods delete
	 *  the i-th player.
	 */
	std::array<std::unique_ptr<Game::Player>, Game::MAX_PLAYERS> _players;

	/** The bosses, if any */
	BossList bosses;
	sftools::Chronometer bossShootClock;
	unsigned short bossClockCycle = 0;

	/** The Final Boss, if this is the last level */
	std::unique_ptr<Game::FinalBoss> finalBoss;

	/** The first Teleport of the level, if any. Keeping the
	 *  pointer to it allows us to traverse the Teleports'
	 *  linked list.
	 */
	Game::Teleport *firstTeleport = nullptr;

	bool gameOverEnded = false;


	/** Deletes all entities */
	void _clearEntities();

	void _pushTemporary(Temporary *const tmp);

	/** If entity in `src` has a line of sight with the one in `target`, 
	 *  returns the number of tiles separating them; else return -1.
	 */
	short _getDistance(const sf::Vector2i& src, const sf::Vector2i& target, bool axis_x);

	short _getPlayerIndex(const Game::Entity *const) const;

	sf::Vector2f _findNearestPlayer(const sf::Vector2f& pos) const;

	/** Destroys all breakable walls in the level, giving points 
	 *  to all players
	 */
	void _destroyAllWalls();

	/** Kills all enemies in the level and damages the bosses by 1; 
	 *  give due points to all players
	 */
	void _killAllEnemies();

	/** Transforms all enemies in harmless creatures which drop
	 *  EXTRA letters for 30 seconds.
	 */
	void _triggerExtraGame();

	void _spawnLetter(const sf::Vector2f& pos);
	void _spawnPoints(const sf::Vector2f& pos, const int amount, bool large = false);

	void _grabBonus(Game::MovingEntity *const entity, Game::Bonus *bonus, unsigned short idx);

	bool _isFinalLevel() const;

public:
	/** Create the LevelRenderer and hand it the ownership of the players */
	LevelRenderer(std::initializer_list<Game::Player*> players);
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

	float getLevelTime() const { return level->getTime() - levelTimeClock.getElapsedTime().asSeconds(); }

	const std::array<Game::Player*, Game::MAX_PLAYERS>& getPlayers() const { return players; }
	Game::Player* getPlayer(const unsigned short i) const { return players[i-1]; }
	
	const FixedEntityList& getFixedEntities() const { return fixedEntities; }
	const MovingEntityList& getMovingEntities() const { return movingEntities; }
	const BossList& getBosses() const { return bosses; }

	Game::Bomb* getBombAt(const unsigned short left, const unsigned short top) const;

	void setOrigin(const sf::Vector2f& _origin) { origin = _origin; }

	/** Checks if any moving entity's bounding box is intersecting
	 *  the rectangle at coordinates `pos`
	 */
	bool isEntityTouching(const sf::Vector2f& pos) const;

	bool isPlayer(const Entity *const) const;

	Game::Level* getLevel() const { return level; }

	void dropBomb(const unsigned short player_id);

	/** Checks all the bombs deployed to see if any of them has finished its countdown and,
	 *  if so, spawns an explosion at its place.
	 */
	void checkBombExplosions();

	/** Checks if a bomb or a moving entity is touching an explosion */
	void checkExplosionHits();

	/** Sets seeingPlayer and playerIsVisible flags for all enemies */
	void checkLinesOfSight();

	void moveBullets();

	/** Removes id-th player (starting from 1) from game and returns
	 *  true if at least another player is still in game, false otherwise.
	 *  If `overrideInternal` is true (default), also change _player, else
	 *  just remove the "external" one.
	 */
	bool removePlayer(const unsigned short id, bool overrideInternal = true);
	void setPlayer(const unsigned short id, Game::Player *player);

	void spawnDamage(const sf::Vector2f& pos, const int amount);

	void makeBossesShoot();

	void dropBonus(const sf::Vector2i& tile, const unsigned short type);

	int getTimeLeft() const;

	/** If level time is <= 30s, make the "Hurry Up!" text appear;
	 *  if level time is <= 0, enter Hurry Up mode (double enemies'
	 *  speed and fire rate)
	 */
	void checkHurryUp();

	void triggerGameOver();
	bool isGameOverEnded() const { return gameOverEnded; }

	void checkExtraGameEnd();

	/** During Extra Game, check for letters transitions */
	void cycleLetters();

	bool isExtraGame() const { return extraGame; }

	/** Gives points to player `player` and spawns a Game::Points text above it. */
	void givePointsTo(const Game::Player *const player, const sf::Vector2f& pos, const int amount);

	/** Returns true if all enemies and bosses are dead. */
	bool isLevelClear() const;

	/** Resets frame clocks of all moving entities */
	void resetFrameClocks();

	/** Resets all self and children's internal clocks */
	void resetClocks();

	/** Pauses all entities' and self clocks */
	void pauseClocks();
	void resumeClocks();
};

}
