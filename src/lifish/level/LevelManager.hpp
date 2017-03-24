#pragma once

#include <array>
#include <SFML/Graphics.hpp>
#include "BaseLevelManager.hpp"
#include "DroppingTextManager.hpp"
#include "LevelRenderer.hpp"
#include "LevelEffects.hpp"
#include "LevelTime.hpp"
#include "conf/player.hpp"
#include "Direction.hpp"

namespace lif {

struct SaveData;
class LevelLoader;
class LevelSet;
class SaveManager;
class WinLoseHandler;
class Player;
class Bomb;
class Level;
class AxisMoving;

/** The class that manages all the level's entities and events.
 *  In particular, its update() method updates all entities,
 *  the collisions and the game logic.
 */
class LevelManager final : public lif::BaseLevelManager, public sf::Drawable {

	friend class lif::LevelLoader;
	friend class lif::LevelRenderer;
	friend class lif::SaveManager;
	friend class lif::WinLoseHandler;

	/** The currently managed level */
	std::unique_ptr<lif::Level> level;
	lif::LevelRenderer renderer;
	lif::LevelEffects effects;
	std::shared_ptr<lif::LevelTime> levelTime;

	/** Whether hurry up has already been triggered or not */
	bool hurryUp = false;
	bool hurryUpWarningGiven = false;
	/** Whether EXTRA game was already triggered or not */
	bool extraGameTriggered = false;
	/** Whether we're currently in EXTRA game or not */
	bool extraGame = false;
	/** This is set to `true` as soon as no players are found alive. */
	bool gameOver = false;
	/** This is set to `true` when the last level is completed */
	bool gameWon = false;

	lif::DroppingTextManager dropTextManager;

	/** "Owned" pointers to players.
	 *  The players' lifecycle is the following:
	 *  1) players are created via `createNewPlayers` and added to `entities`; the LevelManager and the
	 *     EntityGroup share the ownership of the players via shared_ptr;
	 *  2) updates to players are managed by the EntityGroup;
	 *  3) on level change, `entities` gets cleared; the player entities survive, as LevelManager retains
	 *     their pointers, so the RC doesn't drop to 0;
	 *  5) the players are then readded to the EntityGroup.
	 */
	std::array<std::shared_ptr<lif::Player>, lif::MAX_PLAYERS> players;
	/** The players' score */
	std::array<int, MAX_PLAYERS> score;
	/** The remaining 'continues' per player */
	std::array<short, MAX_PLAYERS> playerContinues;


	/** Unowned references to bombs currently deployed by players */
	Matrix<std::weak_ptr<lif::Bomb>, lif::MAX_PLAYERS, lif::conf::player::MAX_MAX_BOMBS> bombs;


	/** Adds the given entity to `entities` */
	void _spawn(lif::Entity *e) override;
	void _spawnBomb(lif::Bomb *b);
	/** Checks if there are any player to resurrect. If no players are found alive,
	 *  sets the `gameOver` flag to `true`.
	 */
	void _checkResurrect();
	void _checkSpecialConditions();
	void _triggerHurryUp();
	void _triggerHurryUpWarning();
	void _triggerExtraGame();
	void _endExtraGame();
	bool _shouldTriggerExtraGame() const;
	bool _isBombAt(const sf::Vector2i& tile) const;

public:
	explicit LevelManager();

	/** Generates n players. If n > MAX_PLAYERS, only generate MAX_PLAYERS players. */
	void createNewPlayers(unsigned short n = lif::MAX_PLAYERS);

	bool isPlayer(const lif::Entity& e) const;
	/** Returns the id-th player (id starting from 1) */
	const std::shared_ptr<lif::Player> getPlayer(unsigned short id) const;
	void setPlayer(unsigned short id, std::shared_ptr<lif::Player> player);
	void removePlayer(unsigned short id);

	/** Adds `amt` points to player `id`'s score and returns new score (id starts from 1) */
	int addScore(unsigned short id, int amt);
	void addScoreToAll(int amt);
	int getScore(unsigned short id) const;

	short getPlayerContinues(unsigned short id) const;
	void setPlayerContinues(unsigned short id, short amt);
	void decPlayerContinues(unsigned short id);

	void loadGame(const lif::SaveData& saveData);

	const lif::Level* getLevel() const { return level != nullptr ? level.get() : nullptr; }
	/** Loads `lvnum`-th level from `ls` into this LevelManager. Destroys previous level if any */
	void setLevel(const lif::LevelSet& ls, unsigned short lvnum);
	/** Loads next level from `level->getLevelSet()`. Throws if `level` is currently null */
	void setNextLevel();

	const lif::LevelTime& getLevelTime() const { return *levelTime; }

	bool isGameOver() const { return gameOver; }
	bool isLevelClear() const;
	bool isGameWon() const { return gameWon; }

	/** Checks whether the owner of `am` can proceed along direction `dir` */
	bool canGo(const lif::AxisMoving& am, const lif::Direction dir) const;
	/** @return whether a player can deploy a bomb or not */
	bool canDeployBomb(const lif::Player& player) const;
	/** A bomb can be deployed in a tile only if no other bomb or explosion is currently in that tile */
	bool canDeployBombAt(const sf::Vector2i& tile) const;
	/** Returns the number of bombs currently deployed by id-th player */
	unsigned short bombsDeployedBy(unsigned short id) const;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void update() override;
	void reset() override;
	/** Resets player continues and score */
	void resetPlayerPersistentData();
};

}
